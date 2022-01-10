#pragma once

#include <AL/al.h>
#include <AL/alc.h>

class AudioDevice
{
public:
	explicit AudioDevice(const char* deviceName);
	void close();

	[[nodiscard]] ALCdevice* c_obj()
	{ return m_alDevice; }

private:
	ALCdevice* m_alDevice;
};
