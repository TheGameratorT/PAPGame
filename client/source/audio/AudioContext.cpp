#include "AudioContext.hpp"

#include <stdexcept>
#include "AudioDevice.hpp"

AudioContext AudioContext::create(AudioDevice& device)
{
	ALCcontext* alcContext = alcCreateContext(device.c_obj(), nullptr);
	if (!alcContext)
		throw std::runtime_error("Could not create audio context");
	return AudioContext(alcContext);
}

void AudioContext::makeCurrent()
{
	if (!alcMakeContextCurrent(m_alcContext))
		throw std::runtime_error("Could not make audio context current");
}
