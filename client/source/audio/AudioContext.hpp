#pragma once

#include <AL/alc.h>

class AudioDevice;

class AudioContext
{
public:
	constexpr explicit AudioContext(ALCcontext* alcContext) :
		m_alcContext(alcContext)
	{}

	static AudioContext create(AudioDevice& device);

	inline void destroy()
	{ alcDestroyContext(m_alcContext); }

	void makeCurrent();

	[[nodiscard]] inline static AudioContext getCurrent()
	{ return AudioContext(alcGetCurrentContext()); }

	inline static void clearCurrent()
	{ alcMakeContextCurrent(nullptr); }

	[[nodiscard]] constexpr ALCcontext* c_obj()
	{ return m_alcContext; }

private:
	ALCcontext* m_alcContext;
};
