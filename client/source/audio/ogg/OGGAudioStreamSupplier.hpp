#pragma once

#include <istream>

#include "../AudioSupplier.hpp"
#include "OGGAudioReader.hpp"

class OGGAudioStreamSupplier : public AudioSupplier
{
public:
	explicit OGGAudioStreamSupplier(std::istream& input);

	void setup() override;
	void supply() override;
	void destroy() override;

	void read(int bufferCount, const ALuint* buffers);

private:
	constexpr static std::size_t NUM_BUFFERS = 4;
	constexpr static ALsizei BUFFER_SIZE = 65536;

	OGGAudioReader m_reader;
	ALuint m_buffers[NUM_BUFFERS];
	ALenum m_format;
	ALsizei m_sampleRate;
};
