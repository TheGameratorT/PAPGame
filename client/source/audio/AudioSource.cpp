#include "AudioSource.hpp"

#include "AudioStream.hpp"

AudioSource::AudioSource(ALuint source) :
	m_source(source),
	m_loop(false)
{}

AudioSource::~AudioSource()
{
	alDeleteSources(1, &m_source);
}

AudioSource AudioSource::create()
{
	ALuint source;
	alGenSources(1, &source);
	return AudioSource(source);
}

void AudioSource::play() const
{
	alSourcePlay(m_source);
}

void AudioSource::stop() const
{
	alSourceStop(m_source);
}

void AudioSource::pause() const
{
	alSourcePause(m_source);
}

void AudioSource::rewind() const
{
	alSourceRewind(m_source);
}

void AudioSource::setVolume(float value) const
{
	alSourcef(m_source, AL_GAIN, value);
}

float AudioSource::getVolume() const
{
	ALfloat value;
	alGetSourcef(m_source, AL_GAIN, &value);
	return value;
}

AudioSource::State AudioSource::getState() const
{
	ALint state;
	alGetSourcei(m_source, AL_SOURCE_STATE, &state);
	return static_cast<State>(state);
}

void AudioSource::setStream(AudioStream &stream)
{
	m_stream = &stream;
	m_stream->genBuffers(*this);
}
