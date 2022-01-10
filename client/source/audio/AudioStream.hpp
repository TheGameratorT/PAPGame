#pragma once

#include <AL/al.h>
#include <istream>

class AudioSource;

class AudioStream
{
public:
	virtual void genBuffers(AudioSource& source) = 0;
	virtual void read(AudioSource& source, int bufferCount, const ALuint* buffers) = 0;
	virtual void update(AudioSource& source) = 0;
};
