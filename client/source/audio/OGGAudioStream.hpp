#pragma once

#include <istream>
#include <vorbis/codec.h>
#include <vorbis/vorbisfile.h>

#include "AudioSource.hpp"
#include "AudioStream.hpp"

class OGGAudioStream : public AudioStream
{
public:
	explicit OGGAudioStream(std::istream& input);

	void genBuffers(AudioSource& source) override;
	void read(AudioSource& source, int bufferCount, const ALuint* buffers) override;
	void update(AudioSource& source) override;

private:
	constexpr static std::size_t NUM_BUFFERS = 4;
	constexpr static ALsizei BUFFER_SIZE = 65536;

	static std::size_t read_ogg_callback(void* ptr, std::size_t size, std::size_t nmemb, void* datasource);
	static int seek_ogg_callback(void* datasource, ogg_int64_t offset, int whence);
	static long tell_ogg_callback(void* datasource);

	std::size_t read_ogg_callback_impl(void* ptr, std::size_t size, std::size_t nmemb);
	int seek_ogg_callback_impl(ogg_int64_t offset, int whence);
	long tell_ogg_callback_impl();

	std::istream& m_input;
	std::size_t m_fileSize;
	ALenum m_format;
	std::int32_t m_sampleRate;
	OggVorbis_File m_oggVorbisFile;
	std::int_fast32_t m_oggCurrentSection = 0;
	double m_duration;
};
