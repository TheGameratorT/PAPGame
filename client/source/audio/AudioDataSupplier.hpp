#pragma once

#include <AL/al.h>

#include "AudioSupplier.hpp"

class AudioData;

class AudioDataSupplier : public AudioSupplier
{
public:
	constexpr explicit AudioDataSupplier(const AudioData& audioData) :
		m_audioData(audioData),
		m_buffer(0)
	{}

	void setup() override;
	void supply() override;
	void destroy() override;

	void setLooping(bool looping) override;
	bool getLooping() override;

private:
	const AudioData& m_audioData;
	ALuint m_buffer;
};
