#include "OGGAudioReader.hpp"

#include <limits>

OGGAudioReader::OGGAudioReader(std::istream& input) :
	m_input(input),
	m_fileSize(0),
	m_format(0),
	m_sampleRate(0),
	m_pcmSize(0),
	m_oggVorbisFile(),
	m_oggCurrentSection(0)
{
	m_input.seekg(0, std::ios_base::beg);
	m_input.ignore(std::numeric_limits<std::streamsize>::max());
	m_fileSize = m_input.gcount();
	m_input.clear();
	m_input.seekg(0, std::ios_base::beg);

	ov_callbacks oggCallbacks;
	oggCallbacks.read_func = read_ogg_callback;
	oggCallbacks.close_func = nullptr;
	oggCallbacks.seek_func = seek_ogg_callback;
	oggCallbacks.tell_func = tell_ogg_callback;

	if (ov_open_callbacks(this, &m_oggVorbisFile, nullptr, -1, oggCallbacks) < 0)
		throw std::runtime_error("Could not ov_open_callbacks");

	vorbis_info* vorbisInfo = ov_info(&m_oggVorbisFile, -1);

	std::int32_t channels = vorbisInfo->channels;
	m_sampleRate = vorbisInfo->rate;
	m_pcmSize = static_cast<std::int32_t>(ov_pcm_total(&m_oggVorbisFile, -1) * channels * 2);

	if (channels == 1)
		m_format = AL_FORMAT_MONO16;
	else if (channels == 2)
		m_format = AL_FORMAT_STEREO16;
	else
	{
		std::string ex =
			"Unrecognised OGG format: " +
			std::to_string(channels) + " channels";
		throw std::runtime_error(ex);
	}
}

std::size_t OGGAudioReader::read(char* buffer, std::size_t length)
{
	std::size_t bytesRead = 0;
	do {
		std::size_t bytesToRead = length - bytesRead;
		if (bytesToRead > 4096) {
			bytesToRead = 4096;
		}
		std::int32_t result = ov_read(
			&m_oggVorbisFile,
			&buffer[bytesRead],
			static_cast<int>(bytesToRead),
			0, 2, 1, &m_oggCurrentSection
		);
		if (result == OV_HOLE)
			throw std::runtime_error("OV_HOLE found while reading OGG file");
		if (result == OV_EBADLINK)
			throw std::runtime_error("OV_EBADLINK found while reading OGG file");
		if (result == OV_EINVAL)
			throw std::runtime_error("OV_EINVAL found while reading OGG file");
		if (result == 0)
			break;
		bytesRead += result;
	} while (bytesRead < length);
	return bytesRead;
}

void OGGAudioReader::seek(std::size_t offset)
{
	std::int32_t seekResult = ov_raw_seek(&m_oggVorbisFile, static_cast<ogg_int64_t>(offset));
	if (seekResult == OV_ENOSEEK)
		throw std::runtime_error("OV_ENOSEEK found when trying to seek");
	if (seekResult == OV_EINVAL)
		throw std::runtime_error("OV_EINVAL found when trying to seek");
	if (seekResult == OV_EREAD)
		throw std::runtime_error("OV_EREAD found when trying to seek");
	if (seekResult == OV_EFAULT)
		throw std::runtime_error("OV_EFAULT found when trying to seek");
	if (seekResult == OV_EOF)
		throw std::runtime_error("OV_EOF found when trying to seek");
	if (seekResult == OV_EBADLINK)
		throw std::runtime_error("OV_EBADLINK found when trying to seek");
	if (seekResult != 0)
		throw std::runtime_error("Unknown error in ov_raw_seek");
}

std::size_t OGGAudioReader::read_ogg_callback(void* ptr, std::size_t size, std::size_t nmemb, void* datasource)
{ return static_cast<OGGAudioReader*>(datasource)->read_ogg_callback_impl(ptr, size, nmemb); }

int OGGAudioReader::seek_ogg_callback(void* datasource, ogg_int64_t offset, int whence)
{ return static_cast<OGGAudioReader*>(datasource)->seek_ogg_callback_impl(offset, whence); }

long OGGAudioReader::tell_ogg_callback(void* datasource)
{ return static_cast<OGGAudioReader*>(datasource)->tell_ogg_callback_impl(); }

std::size_t OGGAudioReader::read_ogg_callback_impl(void* ptr, std::size_t size, std::size_t nmemb)
{
	std::size_t bytesToRead = size * nmemb;

	std::size_t posBeforeRead = m_input.tellg();
	if (posBeforeRead + bytesToRead > m_fileSize)
		bytesToRead = m_fileSize - posBeforeRead;

	if (!m_input.read(static_cast<char*>(ptr), static_cast<std::streamsize>(bytesToRead)))
	{
		if (m_input.eof())
			m_input.clear(); // just clear the error, we will resolve it later
		else
			throw std::runtime_error("Error reading OGG file");
	}
	return bytesToRead;
}

int OGGAudioReader::seek_ogg_callback_impl(ogg_int64_t offset, int whence)
{
	switch (whence)
	{
	case SEEK_SET:
		m_input.seekg(offset, std::ios_base::beg);
		break;
	case SEEK_CUR:
		m_input.seekg(offset, std::ios_base::cur);
		break;
	case SEEK_END:
		m_input.seekg(offset, std::ios_base::end);
		break;
	default:
		return -1;
	}
	return 0;
}

long OGGAudioReader::tell_ogg_callback_impl()
{
	return static_cast<long>(m_input.tellg());
}
