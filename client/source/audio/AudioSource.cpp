#include "AudioSource.hpp"

AudioSource AudioSource::generate()
{
	ALuint source;
	alGenSources(1, &source);

	alSourcef(source, AL_PITCH, 1);
	alSourcef(source, AL_GAIN, 1.0f);
	alSource3f(source, AL_POSITION, 0, 0, 0);
	alSource3f(source, AL_VELOCITY, 0, 0, 0);
	alSourcei(source, AL_LOOPING, AL_FALSE);

	return AudioSource(source);
}
