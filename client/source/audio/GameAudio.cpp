#include "GameAudio.hpp"

#include <istream>
#include <stdexcept>

#include "wav/WAVAudio.hpp"
#include "ogg/OGGAudio.hpp"

namespace fs = std::filesystem;

GameAudio::GameAudio() = default;
GameAudio::~GameAudio() = default;

class GameAudio::LoadedSound
{
public:
	explicit LoadedSound(AudioData data, std::string name) :
		m_data(std::move(data)),
		m_name(std::move(name))
	{}

	[[nodiscard]] constexpr const AudioData& getData() const { return m_data; }
	[[nodiscard]] constexpr const std::string& getName() const { return m_name; }

private:
	AudioData m_data;
	std::string m_name;
};

class GameAudio::LoadedMusic
{
public:
	explicit LoadedMusic(std::string name, std::filesystem::path path, AudioContainer container) :
		m_name(std::move(name)),
		m_path(std::move(path)),
		m_container(container)
	{}

	[[nodiscard]] constexpr const std::string& getName() const { return m_name; }
	[[nodiscard]] constexpr const std::filesystem::path& getPath() const { return m_path; }
	[[nodiscard]] constexpr AudioContainer getContainer() const { return m_container; }

private:
	std::string m_name;
	std::filesystem::path m_path;
	AudioContainer m_container;
};

class GameAudio::SoundInstance
{
public:
	explicit SoundInstance(AudioSource& source, AudioSupplier* supplier, std::ifstream* input) :
		m_source(source),
		m_supplier(supplier),
		m_input(input)
	{}

	[[nodiscard]] constexpr AudioSource& getSource() { return m_source; }
	[[nodiscard]] constexpr AudioSupplier* getSupplier() { return m_supplier; }
	[[nodiscard]] constexpr std::ifstream* getInput() { return m_input; }

private:
	AudioSource& m_source;
	AudioSupplier* m_supplier;
	std::ifstream* m_input;
};

static inline std::ifstream openAudioFile(const fs::path& path)
{
	std::ifstream input(path, std::ios_base::binary);
	if (!input.is_open()) {
		throw std::runtime_error("Could not open audio file: " + path.string());
	}
	return input;
}

static inline AudioData readAllForContainer(std::istream& istream, AudioContainer container)
{
	switch (container)
	{
	case AudioContainer::WAV: return WAVAudio::readAll(istream);
	case AudioContainer::OGG: return OGGAudio::readAll(istream);
	default:
		throw std::runtime_error("Invalid container specified when loading audio");
	}
}

static inline AudioSupplier* makeStreamSupplier(std::istream& istream, AudioContainer container)
{
	switch (container)
	{
	case AudioContainer::WAV: return new WAVAudioStreamSupplier(WAVAudio::makeStreamSupplier(istream));
	case AudioContainer::OGG: return new OGGAudioStreamSupplier(OGGAudio::makeStreamSupplier(istream));
	default:
		throw std::runtime_error("Invalid container specified when loading audio");
	}
}

void GameAudio::init()
{
	m_audioDevice = AudioDevice::open(nullptr);
	m_audioContext = AudioContext::create(m_audioDevice);
	m_audioContext.makeCurrent();
	m_mainSource = AudioSource::generate();
	m_stopUpdater = false;
	m_updaterThread = std::thread([this](){
		while (!m_stopUpdater) {
			update();
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
		}
	});
}

void GameAudio::destroy()
{
	m_stopUpdater = true;
	m_updaterThread.join();

	for (int i = 0; i < m_soundInstances.size(); i++)
		removeSoundInstance(0);
	m_mainSource.destroy();

	AudioContext::clearCurrent();
	m_audioContext.destroy();
	m_audioDevice.close();
}

void GameAudio::loadMusic(const std::string &name, const std::filesystem::path &path, AudioContainer container)
{
	m_loadedMusics.push_back(std::make_unique<LoadedMusic>(name, path, container));
}

void GameAudio::loadSound(const std::string& name, const fs::path& path, AudioContainer container)
{
	std::ifstream input = openAudioFile(path);
	AudioData data = readAllForContainer(input, container);
	input.close();
	m_loadedSounds.push_back(std::make_unique<LoadedSound>(std::move(data), name));
}

void GameAudio::playMusic(AudioSource& source, const std::string& name)
{
	resetSource(source);

	const LoadedMusic* music = getLoadedMusic(name);
	if (music == nullptr)
		throw std::runtime_error("The music \"" + name + "\" is not loaded");

	auto* input = new std::ifstream(openAudioFile(music->getPath()));
	AudioSupplier* supplier = makeStreamSupplier(*input, music->getContainer());
	supplier->attach(source);
	supplier->setup();
	source.play();
	m_soundInstances.push_back(std::make_unique<SoundInstance>(source, supplier, input));
}

void GameAudio::playSound(AudioSource& source, const std::string& name)
{
	resetSource(source);

	const LoadedSound* sound = getLoadedSound(name);
	if (sound == nullptr)
		throw std::runtime_error("The sound \"" + name + "\" is not loaded");

	AudioSupplier* supplier = new AudioDataSupplier(sound->getData().makeSupplier());
	supplier->attach(source);
	supplier->setup();
	source.play();
	m_soundInstances.push_back(std::make_unique<SoundInstance>(source, supplier, nullptr));
}

void GameAudio::setLooping(AudioSource& source, bool looping)
{
	size_t soundInstanceIndex = getSoundInstanceBySource(source);
	if (soundInstanceIndex == -1)
		throw std::runtime_error("Tried to change loop flag on a source that isn't played by the audio manager");
	SoundInstance& instance = *m_soundInstances[soundInstanceIndex];
	AudioSupplier* supplier = instance.getSupplier();
	supplier->setLooping(looping);
}

void GameAudio::resetSource(AudioSource& source)
{
	if (source.getState() != AudioSource::State::Initial)
	{
		std::size_t soundInstanceIndex = getSoundInstanceBySource(source);
		if (soundInstanceIndex == -1)
			throw std::runtime_error("Tried to reset a source that isn't played by the audio manager");
		removeSoundInstance(soundInstanceIndex);
	}
	source.setLooping(false);
}

void GameAudio::removeSoundInstance(std::size_t index)
{
	SoundInstance& instance = *m_soundInstances[index];
	AudioSource& source = instance.getSource();
	if (source.getState() != AudioSource::State::Initial)
		source.rewind();
	AudioSupplier* supplier = instance.getSupplier();
	supplier->destroy();
	delete supplier;
	std::ifstream* input = instance.getInput();
	if (input != nullptr)
	{
		input->close();
		delete input;
	}
	m_soundInstances.erase(m_soundInstances.begin() + static_cast<int>(index));
}

std::size_t GameAudio::getSoundInstanceBySource(AudioSource& source)
{
	for (std::size_t i = 0; i < m_soundInstances.size(); i++)
	{
		if (&m_soundInstances[i]->getSource() == &source)
			return i;
	}
	return -1;
}

void GameAudio::update()
{
	for (int i = 0; i < m_soundInstances.size(); i++)
	{
		SoundInstance& soundInstance = *m_soundInstances[i];
		AudioSource& source = soundInstance.getSource();
		AudioSupplier* supplier = soundInstance.getSupplier();
		AudioSource::State sourceState = source.getState();
		if (sourceState == AudioSource::State::Playing)
		{
			supplier->supply();
		}
		else if (sourceState == AudioSource::State::Stopped)
		{
			removeSoundInstance(i);
			i--;
		}
	}
}

const GameAudio::LoadedMusic* GameAudio::getLoadedMusic(const std::string& name)
{
	for (const auto& loadedMusic : m_loadedMusics)
	{
		if (loadedMusic->getName() == name)
			return loadedMusic.get();
	}
	return nullptr;
}

const GameAudio::LoadedSound* GameAudio::getLoadedSound(const std::string& name)
{
	for (const auto& loadedSound : m_loadedSounds)
	{
		if (loadedSound->getName() == name)
			return loadedSound.get();
	}
	return nullptr;
}
