#pragma once

#include <AL/al.h>
#include <memory>

#include "AudioDataSupplier.hpp"

class AudioData
{
public:
	explicit AudioData(std::unique_ptr<char[]> data, std::size_t dataSize, std::int32_t sampleRate, ALenum format) :
		m_data(std::move(data)),
		m_dataSize(dataSize),
		m_sampleRate(sampleRate),
		m_format(format)
	{}

	[[nodiscard]] inline char* getData() const
	{ return m_data.get(); }

	[[nodiscard]] constexpr std::size_t getDataSize() const
	{ return m_dataSize; }

	[[nodiscard]] constexpr std::int32_t getSampleRate() const
	{ return m_sampleRate; }

	[[nodiscard]] constexpr ALenum getFormat() const
	{ return m_format; }

	constexpr AudioDataSupplier makeSupplier()
	{ return AudioDataSupplier(*this); }

private:
	std::unique_ptr<char[]> m_data;
	std::size_t m_dataSize;
	std::int32_t m_sampleRate;
	ALenum m_format;
};
