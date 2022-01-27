#include "OGGAudioStreamSupplier.hpp"

#include <memory>

OGGAudioStreamSupplier::OGGAudioStreamSupplier(std::istream& input) :
	m_reader(input),
	m_buffers(),
	m_format(m_reader.getFormat()),
	m_sampleRate(m_reader.getSampleRate()),
	m_looping(false)
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
	getSource().setBuffer(NULL);
	alDeleteBuffers(NUM_BUFFERS, m_buffers);
}

void OGGAudioStreamSupplier::setLooping(bool looping)
{
	m_looping = looping;
}

bool OGGAudioStreamSupplier::getLooping()
{
	return m_looping;
}

void OGGAudioStreamSupplier::read(int bufferCount, const ALuint *buffers)
{
	std::size_t currentBuffer = 0;
	for (; currentBuffer < bufferCount; currentBuffer++)
	{
		std::unique_ptr<char[]> newData(new char[BUFFER_SIZE]);
		std::size_t readBufDataSize = 0;

		while (readBufDataSize < BUFFER_SIZE)
		{
			std::size_t result = m_reader.read(&newData[readBufDataSize], BUFFER_SIZE - readBufDataSize);
			if (result == 0)
			{
				if (getLooping())
					m_reader.seek(0);
				else
					break; // stop reading
			}
			readBufDataSize += result;
		}

		if (readBufDataSize != 0) // if data was read
		{
			alBufferData(buffers[currentBuffer], m_format, newData.get(), BUFFER_SIZE, m_sampleRate);
			if (readBufDataSize < BUFFER_SIZE)  // if data read is less than the buffer, assume read should end
			{
				currentBuffer++; // still queue this buffer which has some data
				break;
			}
		}
		else
		{
			// no more data left, get out of here
			break;
		}
	}

	if (currentBuffer != 0)
		alSourceQueueBuffers(getSource().c_obj(), static_cast<int>(currentBuffer), buffers);
}
