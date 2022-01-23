#pragma once

#include <istream>
#include <AL/al.h>
#include <vorbis/codec.h>
#include <vorbis/vorbisfile.h>

class OGGAudioReader
{
public:
	explicit OGGAudioReader(std::istream& input);

	std::size_t read(char* buffer, std::size_t length);
	void seek(std::size_t offset);

	[[nodiscard]] constexpr std::int32_t getPcmSize() const { return m_pcmSize; }
	[[nodiscard]] constexpr ALenum getFormat() const { return m_format; }
	[[nodiscard]] constexpr std::int32_t getSampleRate() const { return m_sampleRate; }

private:
	static std::size_t read_ogg_callback(void* ptr, std::size_t size, std::size_t nmemb, void* datasource);
	static int seek_ogg_callback(void* datasource, ogg_int64_t offset, int whence);
	static long tell_ogg_callback(void* datasource);

	inline std::size_t read_ogg_callback_impl(void* ptr, std::size_t size, std::size_t nmemb);
	inline int seek_ogg_callback_impl(ogg_int64_t offset, int whence);
	inline long tell_ogg_callback_impl();

	std::istream& m_input;
	std::size_t m_fileSize;
	ALenum m_format;
	std::int32_t m_sampleRate;
	std::int32_t m_pcmSize;
	OggVorbis_File m_oggVorbisFile;
	std::int_fast32_t m_oggCurrentSection;
};
