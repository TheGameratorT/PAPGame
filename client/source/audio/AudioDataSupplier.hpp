#pragma once

#include <AL/al.h>

#include "AudioSupplier.hpp"

class AudioData;

class AudioDataSupplier : public AudioSupplier
{
public:
	constexpr explicit AudioDataSupplier(AudioData& audioData) :
		m_audioData(audioData),
		m_buffer(0)
	{}

	void setup() override;
	void supply() override;
	void destroy() override;

private:
	AudioData& m_audioData;
	ALuint m_buffer;
};
