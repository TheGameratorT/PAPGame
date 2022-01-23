#include "WAVAudioStreamSupplier.hpp"

#include <iostream>
#include <limits>
#include <cstring>
#include <memory>

WAVAudioStreamSupplier::WAVAudioStreamSupplier(std::istream& input) :
	m_reader(input),
	m_buffers(),
	m_format(m_reader.getFormat()),
	m_sampleRate(m_reader.getSampleRate())
{}

void WAVAudioStreamSupplier::setup()
{
	alGenBuffers(NUM_BUFFERS, m_buffers);
	read(NUM_BUFFERS, m_buffers);
}

void WAVAudioStreamSupplier::supply()
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

void WAVAudioStreamSupplier::destroy()
{
	alDeleteBuffers(4, m_buffers);
}

void WAVAudioStreamSupplier::read(int bufferCount, const ALuint* buffers)
{
	for (std::size_t i = 0; i < bufferCount; i++)
	{
		std::unique_ptr<char[]> newData(new char[BUFFER_SIZE]);
		std::size_t readBufDataSize = 0;

		while (readBufDataSize < BUFFER_SIZE)
		{
			std::size_t bytesRead = m_reader.read(&newData[readBufDataSize], BUFFER_SIZE - readBufDataSize);

			if (bytesRead == 0)
			{
				if (/*source.getLooping()*/ true)
				{
					m_reader.seek(0);
				}
				else
				{
					// stop logic goes here
				}
			}
			readBufDataSize += bytesRead;
		}

		alBufferData(buffers[i], m_format, newData.get(), BUFFER_SIZE, m_sampleRate);
	}

	alSourceQueueBuffers(getSource().c_obj(), bufferCount, buffers);
}
