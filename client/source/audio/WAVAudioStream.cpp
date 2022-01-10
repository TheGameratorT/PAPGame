#include "WAVAudioStream.hpp"

#include <iostream>
#include <limits>
#include <cstring>
#include <memory>

struct WAVInfo
{
	std::int_fast32_t fileSize;
	std::int_fast16_t format;
	std::int_fast16_t channels;
	std::int_fast32_t sampleRate;
	std::int_fast16_t bitsPerSample;
	std::int_fast32_t dataSize;
};

WAVAudioStream::WAVAudioStream(std::istream& input) :
	m_input(input)
{
	WAVInfo info{};
	load(&info);

	std::int16_t channels = info.channels;
	m_sampleRate = info.sampleRate;
	std::int16_t bitsPerSample = info.bitsPerSample;
	m_dataEndPos = DATA_START_POS + info.dataSize;

	if (channels == 1 && bitsPerSample == 8)
		m_format = AL_FORMAT_MONO8;
	else if (channels == 1 && bitsPerSample == 16)
		m_format = AL_FORMAT_MONO16;
	else if (channels == 2 && bitsPerSample == 8)
		m_format = AL_FORMAT_STEREO8;
	else if (channels == 2 && bitsPerSample == 16)
		m_format = AL_FORMAT_STEREO16;
	else
	{
		std::string ex =
				"Unrecognised WAV format: " +
				std::to_string(channels) + " channels, " +
				std::to_string(bitsPerSample) + " bps";
		throw std::runtime_error(ex);
	}
}

void WAVAudioStream::load(void* out)
{
	auto* info = static_cast<WAVInfo*>(out);

	m_input.seekg(0, std::ios_base::beg);

	char buffer[4];
	if (!m_input.read(buffer, 4))
		throw std::runtime_error("Error reading WAV file at RIFF header");
	if (std::strncmp(buffer, "RIFF", 4) != 0)
		throw std::runtime_error("Error reading WAV file, RIFF header mismatch");
	if (!m_input.read(buffer, 4))
		throw std::runtime_error("Error reading WAV file size");
	info->fileSize = *reinterpret_cast<std::int32_t*>(buffer);
	if (!m_input.read(buffer, 4))
		throw std::runtime_error("Error reading WAV file at WAVE header");
	if (std::strncmp(buffer, "WAVE", 4) != 0)
		throw std::runtime_error("Error reading WAV file, WAVE header mismatch");
	if (!m_input.read(buffer, 4))
		throw std::runtime_error("Error reading WAV file at FMT header");
	if (std::strncmp(buffer, "fmt ", 4) != 0)
		throw std::runtime_error("Error reading WAV file, FMT header mismatch");
	if (!m_input.read(buffer, 4))
		throw std::runtime_error("Error reading WAV file at FMT chunk size");
	if (!m_input.read(buffer, 2))
		throw std::runtime_error("Error reading WAV file at PCM format");
	info->format = *reinterpret_cast<std::int16_t*>(buffer);
	if (!m_input.read(buffer, 2))
		throw std::runtime_error("Error reading WAV file at channel count");
	info->channels = *reinterpret_cast<std::int16_t*>(buffer);
	if (!m_input.read(buffer, 4))
		throw std::runtime_error("Error reading WAV file at sample rate");
	info->sampleRate = *reinterpret_cast<std::int32_t*>(buffer);
	if (!m_input.read(buffer, 4))
		throw std::runtime_error("Error reading WAV file at 0x1C");
	if (!m_input.read(buffer, 2))
		throw std::runtime_error("Error reading WAV file at 0x20");
	if (!m_input.read(buffer, 2))
		throw std::runtime_error("Error reading WAV file at bits per sample");
	info->bitsPerSample = *reinterpret_cast<std::int16_t*>(buffer);
	if (!m_input.read(buffer, 4))
		throw std::runtime_error("Error reading WAV file at DATA header");
	if (std::strncmp(buffer, "data", 4) != 0)
		throw std::runtime_error("Error reading WAV file, DATA header mismatch");
	if (!m_input.read(buffer, 4))
		throw std::runtime_error("Error reading WAV file at DATA chunk size");
	info->dataSize = *reinterpret_cast<std::int32_t*>(buffer);
}

void WAVAudioStream::genBuffers(AudioSource& source)
{
	ALuint buffers[NUM_BUFFERS];
	alGenBuffers(NUM_BUFFERS, buffers);
	read(source, NUM_BUFFERS, buffers);
}

void WAVAudioStream::read(AudioSource& source, int bufferCount, const ALuint* buffers)
{
	for (std::size_t i = 0; i < bufferCount; i++)
	{
		std::unique_ptr<char[]> newData(new char[BUFFER_SIZE]);
		std::size_t readBufDataSize = 0;

		while (readBufDataSize < BUFFER_SIZE)
		{
			std::size_t bytesToRead = BUFFER_SIZE - readBufDataSize;
			std::size_t posBeforeRead = m_input.tellg();
			if (posBeforeRead + bytesToRead > m_dataEndPos)
				bytesToRead = m_dataEndPos - posBeforeRead;

			if (!m_input.read(&newData[readBufDataSize], static_cast<std::streamsize>(bytesToRead)))
				throw std::runtime_error("Error reading WAV buffer");

			if (m_input.tellg() >= m_dataEndPos)
			{
				if (source.getLooping())
				{
					if (!m_input.seekg(DATA_START_POS, std::ios_base::beg))
						throw std::runtime_error("Error looping WAV file while trying to seek to beginning");
				}
				else
				{
					// stop logic goes here
				}
			}
			readBufDataSize += bytesToRead;
		}

		alBufferData(buffers[i], m_format, newData.get(), BUFFER_SIZE, m_sampleRate);
	}

	alSourceQueueBuffers(source.c_obj(), bufferCount, buffers);
}

void WAVAudioStream::update(AudioSource& source)
{
	ALint buffersProcessed = 0;
	alGetSourcei(source.c_obj(), AL_BUFFERS_PROCESSED, &buffersProcessed);
	if (buffersProcessed <= 0)
		return;

	ALuint buffers[NUM_BUFFERS];
	alSourceUnqueueBuffers(source.c_obj(), buffersProcessed, buffers);
	read(source, buffersProcessed, buffers);
}
