#pragma once

#include <string>
#include <vector>
#include <fstream>
#include <filesystem>

#include "AudioData.hpp"
#include "AudioSource.hpp"
#include "AudioContainer.hpp"

class GameAudio
{
public:
	GameAudio();
	~GameAudio();

	void loadMusic(const std::string& name, const std::filesystem::path& path, AudioContainer container);
	void loadSound(const std::string& name, const std::filesystem::path& path, AudioContainer container);

	void playMusic(AudioSource& source, const std::string& name);
	void playSound(AudioSource& source, const std::string& name);

	void setLooping(AudioSource& source, bool looping);

	void resetSource(AudioSource& source);
	void removeSoundInstance(std::size_t index);
	std::size_t getSoundInstanceBySource(AudioSource& source);

	void update();
	void destroy();

private:
	class LoadedMusic;
	class LoadedSound;
	class SoundInstance;

	const LoadedMusic* getLoadedMusic(const std::string& name);
	const LoadedSound* getLoadedSound(const std::string& name);

	std::vector<std::unique_ptr<LoadedSound>> m_loadedSounds;
	std::vector<std::unique_ptr<LoadedMusic>> m_loadedMusics;
	std::vector<std::unique_ptr<SoundInstance>> m_soundInstances;
};
