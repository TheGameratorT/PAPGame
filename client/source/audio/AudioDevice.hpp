#pragma once

#include <AL/alc.h>

class AudioDevice
{
public:
	constexpr explicit AudioDevice(ALCdevice* alcDevice) :
		m_alcDevice(alcDevice)
	{}

	static AudioDevice open(const char* deviceName);
	void close();

	[[nodiscard]] constexpr ALCdevice* c_obj()
	{ return m_alcDevice; }

private:
	ALCdevice* m_alcDevice;
};
