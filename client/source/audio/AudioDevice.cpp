#include "AudioDevice.hpp"

#include <stdexcept>

AudioDevice AudioDevice::open(const char* deviceName)
{
	ALCdevice* alcDevice = alcOpenDevice(deviceName);
	if (!alcDevice)
		throw std::runtime_error("Could not open audio device");
	return AudioDevice(alcDevice);
}

void AudioDevice::close()
{
	if (!alcCloseDevice(m_alcDevice))
		throw std::runtime_error("Could not close audio device");
}
