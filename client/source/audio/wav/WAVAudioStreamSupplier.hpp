#pragma once

#include <istream>
#include <al.h>

#include "../AudioSupplier.hpp"
#include "WAVAudioReader.hpp"

class WAVAudioStreamSupplier : public AudioSupplier
{
public:
	explicit WAVAudioStreamSupplier(std::istream& input);

	void setup() override;
	void supply() override;
	void destroy() override;

private:
	constexpr static std::size_t NUM_BUFFERS = 4;
	constexpr static std::size_t BUFFER_SIZE = 65536;

	void read(int bufferCount, const ALuint* buffers);

	WAVAudioReader m_reader;
	ALuint m_buffers[NUM_BUFFERS];
	ALenum m_format;
	std::int32_t m_sampleRate;
};
