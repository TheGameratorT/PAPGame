#pragma once

#include "AudioSource.hpp"

class AudioSupplier
{
public:
	constexpr AudioSupplier() :
		m_source(0)
	{}

	virtual ~AudioSupplier() = default;

	constexpr void attach(AudioSource source)
	{ m_source = source; }

	virtual void setup() = 0;
	virtual void supply() = 0;
	virtual void destroy() = 0;

protected:
	[[nodiscard]] constexpr AudioSource getSource()
	{ return m_source; }

private:
	AudioSource m_source;
};
