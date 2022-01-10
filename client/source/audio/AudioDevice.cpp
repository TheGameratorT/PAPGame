#include "AudioDevice.hpp"

#include <stdexcept>

AudioDevice::AudioDevice(const char* deviceName)
{
	m_alDevice = alcOpenDevice(deviceName);
	if (!m_alDevice)
		throw std::runtime_error("Could not open audio device");
}

void AudioDevice::close()
{
	if (!alcCloseDevice(m_alDevice))
		throw std::runtime_error("Could not close audio device");
}
