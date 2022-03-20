#pragma once

#include <string>
#include <vector>
#include <thread>
#include <fstream>
#include <filesystem>

#include "AudioData.hpp"
#include "AudioSource.hpp"
#include "AudioContext.hpp"
#include "AudioDevice.hpp"
#include "AudioContainer.hpp"

class GameAudio
{
public:
	GameAudio();
	~GameAudio();

	void init();
	void destroy();

	void loadMusic(const std::string& name, const std::filesystem::path& path, AudioContainer container);
	void loadSound(const std::string& name, const std::filesystem::path& path, AudioContainer container);

	void playMusic(AudioSource& source, const std::string& name);
	void playSound(AudioSource& source, const std::string& name);

	void setLooping(AudioSource& source, bool looping);

	void resetSource(AudioSource& source);
	void removeSoundInstance(std::size_t index);
	std::size_t getSoundInstanceBySource(AudioSource& source);

	void update();

	[[nodiscard]] constexpr AudioSource& getMainSource()
	{ return m_mainSource; }

private:
	class LoadedMusic;
	class LoadedSound;
	class SoundInstance;

	const LoadedMusic* getLoadedMusic(const std::string& name);
	const LoadedSound* getLoadedSound(const std::string& name);

	AudioDevice m_audioDevice;
	AudioContext m_audioContext;
	AudioSource m_mainSource;
	std::vector<std::unique_ptr<LoadedSound>> m_loadedSounds;
	std::vector<std::unique_ptr<LoadedMusic>> m_loadedMusics;
	std::vector<std::unique_ptr<SoundInstance>> m_soundInstances;
	std::atomic<bool> m_stopUpdater;
	std::thread m_updaterThread;
};
