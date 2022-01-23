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
