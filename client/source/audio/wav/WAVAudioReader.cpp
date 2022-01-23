#include "WAVAudioReader.hpp"

#include <limits>
#include <cstdint>
#include <cstring>
#include <stdexcept>

struct WAVFileHeader
{
	char riff[4];
	std::int32_t fileSize;
	char wave[4];
	char fmt[4];
	std::int32_t fmtSize;
	std::int16_t pcmFormat;
	std::int16_t channelCount;
	std::int32_t sampleRate;
	std::int32_t unk1C;
	std::int16_t unk20;
	std::int16_t bitsPerSample;
	char data[4];
	std::int32_t dataSize;
};

WAVAudioReader::WAVAudioReader(std::istream& input) :
	m_input(input)
{
	input.seekg(0);

	WAVFileHeader header{};
	input.read(reinterpret_cast<char*>(&header), WAV_HEADER_SIZE);
	if (input.fail()) {
		throw std::runtime_error("Could not read WAV file header");
	}

	if (std::strncmp(header.riff, "RIFF", 4) != 0)
		throw std::runtime_error("Error reading WAV file, RIFF header mismatch");
	if (std::strncmp(header.wave, "WAVE", 4) != 0)
		throw std::runtime_error("Error reading WAV file, WAVE header mismatch");
	if (std::strncmp(header.fmt, "fmt ", 4) != 0)
		throw std::runtime_error("Error reading WAV file, FMT header mismatch");
	if (std::strncmp(header.data, "data", 4) != 0)
		throw std::runtime_error("Error reading WAV file, DATA header mismatch");

	std::int16_t channels = header.channelCount;
	std::int16_t bitsPerSample = header.bitsPerSample;
	m_sampleRate = header.sampleRate;
	m_pcmSize = header.dataSize;
	m_dataEndPos = WAV_HEADER_SIZE + m_pcmSize;

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

std::size_t WAVAudioReader::read(char* buffer, std::size_t length)
{
	std::size_t bytesToRead = length;
	std::size_t posBeforeRead = m_input.tellg();
	if (posBeforeRead + bytesToRead > m_dataEndPos)
		bytesToRead = m_dataEndPos - posBeforeRead;

	if (bytesToRead != 0) {
		if (!m_input.read(buffer, static_cast<std::streamsize>(bytesToRead)))
			throw std::runtime_error("Error reading WAV buffer");
	}
	return bytesToRead;
}

void WAVAudioReader::seek(std::size_t offset)
{
	if (!m_input.seekg(static_cast<std::streamsize>(WAV_HEADER_SIZE + offset)))
		throw std::runtime_error("Failed to seek WAV file");
}
