#pragma once

#include <istream>
#include <al.h>

#include "AudioSource.hpp"
#include "AudioStream.hpp"

class WAVAudioStream : public AudioStream
{
public:
	explicit WAVAudioStream(std::istream& input);

	void genBuffers(AudioSource& source) override;
	void read(AudioSource& source, int bufferCount, const ALuint* buffers) override;
	void update(AudioSource& source) override;

private:
	constexpr static std::size_t NUM_BUFFERS = 4;
	constexpr static std::size_t BUFFER_SIZE = 65536;
	constexpr static std::size_t DATA_START_POS = 0x2C;

	void load(void* out);

	std::istream& m_input;
	ALenum m_format;
	std::int32_t m_sampleRate;
	std::size_t m_dataEndPos;
};
