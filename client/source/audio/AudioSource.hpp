#pragma once

#include <AL/al.h>

class AudioSource
{
public:
	enum class State
	{
		Initial = 0x1011,
		Playing = 0x1012,
		Paused = 0x1013,
		Stopped = 0x1014
	};

	constexpr AudioSource() :
		m_source(0)
	{}

	constexpr explicit AudioSource(ALuint source) :
		m_source(source)
	{}

	static AudioSource generate();
	inline void destroy() { alDeleteSources(1, &m_source); }

	inline void play() const { alSourcePlay(m_source); }
	inline void stop() const { alSourceStop(m_source); }
	inline void pause() const { alSourcePause(m_source); }
	inline void rewind() const { alSourceRewind(m_source); }

	inline void setVolume(float value) const
	{ alSourcef(m_source, AL_GAIN, value); }

	[[nodiscard]] inline float getVolume() const
	{
		ALfloat value;
		alGetSourcef(m_source, AL_GAIN, &value);
		return value;
	}

	inline void setLooping(bool value) const
	{ alSourcei(m_source, AL_LOOPING, value); }

	[[nodiscard]] inline bool getLooping() const
	{
		ALint value;
		alGetSourcei(m_source, AL_LOOPING, &value);
		return value;
	}

	inline void setBuffer(ALuint buffer) const
	{ alSourcei(m_source, AL_BUFFER, static_cast<ALint>(buffer)); }

	[[nodiscard]] inline ALuint getBuffer() const
	{
		ALint value;
		alGetSourcei(m_source, AL_BUFFER, &value);
		return static_cast<ALuint>(value);
	}

	inline void setPosition(float x, float y, float z) const
	{
		float value[] = {x, y, z};
		alSourcefv(m_source, AL_POSITION, value);
	}

	[[nodiscard]] inline State getState() const
	{
		ALint state;
		alGetSourcei(m_source, AL_SOURCE_STATE, &state);
		return static_cast<State>(state);
	}

	[[nodiscard]] constexpr ALuint c_obj() const
	{ return m_source; }

private:
	ALuint m_source;
};
