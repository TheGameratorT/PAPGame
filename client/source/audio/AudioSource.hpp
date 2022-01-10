#pragma once

#include <AL/al.h>
#include <AL/alc.h>
#include <utility>

class AudioStream;

class AudioSource
{
public:
	~AudioSource();

	AudioSource(const AudioSource& obj) = delete;
	AudioSource& operator=(const AudioSource& obj) = delete;

	AudioSource(AudioSource&& other) noexcept
	{
		m_source = other.m_source;
		m_loop = other.m_loop;
		m_stream = other.m_stream;
	}

	AudioSource& operator=(AudioSource&& other) noexcept
	{
		m_source = other.m_source;
		m_loop = other.m_loop;
		m_stream = other.m_stream;
		return *this;
	}

	static AudioSource create();

	enum class State
	{
		Initial = 0x1011,
		Playing = 0x1012,
		Paused = 0x1013,
		Stopped = 0x1014
	};

	void play() const;
	void stop() const;
	void pause() const;
	void rewind() const;
	void setVolume(float value) const;
	[[nodiscard]] float getVolume() const;
	[[nodiscard]] State getState() const;

	void setLooping(bool value)
	{ m_loop = value; }

	[[nodiscard]] bool getLooping() const
	{ return m_loop; }

	void setStream(AudioStream& stream);

	[[nodiscard]] AudioStream& getStream() const
	{ return *m_stream; }

	[[nodiscard]] ALuint c_obj() const
	{ return m_source; }

private:
	explicit AudioSource(ALuint source);

protected:
	ALuint m_source;
	bool m_loop;
	AudioStream* m_stream;
};
