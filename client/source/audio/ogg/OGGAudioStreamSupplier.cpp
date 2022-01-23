#include "OGGAudioStreamSupplier.hpp"

#include <memory>

OGGAudioStreamSupplier::OGGAudioStreamSupplier(std::istream& input) :
	m_reader(input),
	m_buffers(),
	m_format(m_reader.getFormat()),
	m_sampleRate(m_reader.getSampleRate())
{}

void OGGAudioStreamSupplier::setup()
{
	alGenBuffers(NUM_BUFFERS, m_buffers);
	read(NUM_BUFFERS, m_buffers);
}

void OGGAudioStreamSupplier::supply()
{
	ALuint source = getSource().c_obj();
	ALint buffersProcessed = 0;
	alGetSourcei(source, AL_BUFFERS_PROCESSED, &buffersProcessed);
	if (buffersProcessed <= 0)
		return;

	ALuint buffers[NUM_BUFFERS];
	alSourceUnqueueBuffers(source, buffersProcessed, buffers);
	read(buffersProcessed, buffers);
}

void OGGAudioStreamSupplier::destroy()
{
	alDeleteBuffers(4, m_buffers);
}

void OGGAudioStreamSupplier::read(int bufferCount, const ALuint *buffers)
{
	for (std::size_t i = 0; i < bufferCount; i++)
	{
		std::unique_ptr<char[]> newData(new char[BUFFER_SIZE]);
		std::size_t readBufDataSize = 0;

		while (readBufDataSize < BUFFER_SIZE)
		{
			std::size_t result = m_reader.read(&newData[readBufDataSize], BUFFER_SIZE - readBufDataSize);
			if (result == 0)
				m_reader.seek(0);
			readBufDataSize += result;
		}

		alBufferData(buffers[i], m_format, newData.get(), BUFFER_SIZE, m_sampleRate);
	}

	alSourceQueueBuffers(getSource().c_obj(), bufferCount, buffers);
}
