#include "AudioContext.hpp"

#include <stdexcept>

AudioContext* AudioContext::sm_current;

AudioContext::AudioContext(AudioDevice& device)
{
	m_alcContext = alcCreateContext(device.c_obj(), nullptr);
	if (!m_alcContext)
		throw std::runtime_error("Could not create audio context");
}

AudioContext::~AudioContext()
{
	if (isCurrent())
		clearCurrent();
	alcDestroyContext(m_alcContext);
}

void AudioContext::makeCurrent()
{
	if (!alcMakeContextCurrent(m_alcContext))
		throw std::runtime_error("Could not make audio context current");
	sm_current = this;
}

bool AudioContext::isCurrent()
{
	return sm_current == this;
}

void AudioContext::clearCurrent()
{
	alcMakeContextCurrent(nullptr);
	sm_current = nullptr;
}
