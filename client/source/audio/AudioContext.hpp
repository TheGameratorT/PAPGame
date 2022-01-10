#pragma once

#include <AL/alc.h>
#include <utility>

#include "AudioDevice.hpp"

class AudioContext
{
public:
	AudioContext(const AudioContext& obj) = delete;
	AudioContext& operator=(const AudioContext& obj) = delete;

	AudioContext(AudioContext&& other) noexcept
	{ m_alcContext = other.m_alcContext; }

	AudioContext& operator=(AudioContext&& other) noexcept
	{
		m_alcContext = other.m_alcContext;
		return *this;
	}

	explicit AudioContext(AudioDevice& device);
	~AudioContext();

	void makeCurrent();
	bool isCurrent();
	static void clearCurrent();

	static AudioContext* getCurrent()
	{ return sm_current; }

	[[nodiscard]] ALCcontext* c_obj()
	{ return m_alcContext; }

private:
	static AudioContext* sm_current;

	ALCcontext* m_alcContext;
};
