#pragma once

#include <istream>
#include <AL/al.h>

class WAVAudioReader
{
public:
	explicit WAVAudioReader(std::istream& input);

	std::size_t read(char* buffer, std::size_t length);
	void seek(std::size_t offset);

	[[nodiscard]] constexpr std::int32_t getPcmSize() const { return m_pcmSize; }
	[[nodiscard]] constexpr ALenum getFormat() const { return m_format; }
	[[nodiscard]] constexpr std::int32_t getSampleRate() const { return m_sampleRate; }

private:
	constexpr static std::size_t WAV_HEADER_SIZE = 0x2C;

	std::istream& m_input;
	std::size_t m_dataEndPos;
	ALenum m_format;
	std::int32_t m_sampleRate;
	std::int32_t m_pcmSize;
};
