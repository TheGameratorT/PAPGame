#include "AudioDataSupplier.hpp"

#include "AudioData.hpp"
#include "AudioSource.hpp"

void AudioDataSupplier::setup()
{
	alGenBuffers(1, &m_buffer);
	alBufferData(
		m_buffer,
		m_audioData.getFormat(),
		m_audioData.getData(),
		static_cast<ALsizei>(m_audioData.getDataSize()),
		m_audioData.getSampleRate()
	);
	alSourcei(getSource().c_obj(), AL_BUFFER, static_cast<ALint>(m_buffer));
}

void AudioDataSupplier::supply()
{
	// do nothing because the buffer is static
}

void AudioDataSupplier::destroy()
{
	getSource().setBuffer(0);
	alDeleteBuffers(1, &m_buffer);
}

void AudioDataSupplier::setLooping(bool looping)
{
	getSource().setLooping(true);
}

bool AudioDataSupplier::getLooping()
{
	return getSource().getLooping();
}
