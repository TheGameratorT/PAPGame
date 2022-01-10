#include "OGGAudioStream.hpp"

#include <iostream>
#include <limits>
#include <memory>

OGGAudioStream::OGGAudioStream(std::istream& input) :
	m_input(input)
{
	m_input.seekg(0, std::ios_base::beg);
	m_input.ignore(std::numeric_limits<std::streamsize>::max());
	m_fileSize = static_cast<ALsizei>(m_input.gcount());
	m_input.clear();
	m_input.seekg(0,std::ios_base::beg);

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
	m_duration = ov_time_total(&m_oggVorbisFile, -1);

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

void OGGAudioStream::genBuffers(AudioSource& source)
{
	ALuint buffers[NUM_BUFFERS];
	alGenBuffers(NUM_BUFFERS, buffers);
	read(source, NUM_BUFFERS, buffers);
}

void OGGAudioStream::read(AudioSource& source, int bufferCount, const ALuint* buffers)
{
	for (std::size_t i = 0; i < bufferCount; i++)
	{
		std::unique_ptr<char[]> newData(new char[BUFFER_SIZE]);
		std::size_t readBufDataSize = 0;

		while (readBufDataSize < BUFFER_SIZE)
		{
			std::int32_t result = ov_read(
				&m_oggVorbisFile,
				&newData[readBufDataSize],
				static_cast<int>(BUFFER_SIZE - readBufDataSize),
				0, 2, 1, &m_oggCurrentSection
			);

			if (result == OV_HOLE)
			{
				std::cerr << "ERROR: OV_HOLE found in update of buffer " << std::endl;
				break;
			}
			else if (result == OV_EBADLINK)
			{
				std::cerr << "ERROR: OV_EBADLINK found in update of buffer " << std::endl;
				break;
			}
			else if (result == OV_EINVAL)
			{
				std::cerr << "ERROR: OV_EINVAL found in update of buffer " << std::endl;
				break;
			}
			else if (result == 0)
			{
				std::int32_t seekResult = ov_raw_seek(&m_oggVorbisFile, 0);
				if (seekResult == OV_ENOSEEK)
					std::cerr << "ERROR: OV_ENOSEEK found when trying to loop" << std::endl;
				else if (seekResult == OV_EINVAL)
					std::cerr << "ERROR: OV_EINVAL found when trying to loop" << std::endl;
				else if (seekResult == OV_EREAD)
					std::cerr << "ERROR: OV_EREAD found when trying to loop" << std::endl;
				else if (seekResult == OV_EFAULT)
					std::cerr << "ERROR: OV_EFAULT found when trying to loop" << std::endl;
				else if (seekResult == OV_EOF)
					std::cerr << "ERROR: OV_EOF found when trying to loop" << std::endl;
				else if (seekResult == OV_EBADLINK)
					std::cerr << "ERROR: OV_EBADLINK found when trying to loop" << std::endl;

				if (seekResult != 0)
				{
					std::cerr << "ERROR: Unknown error in ov_raw_seek" << std::endl;
					return;
				}
			}
			readBufDataSize += result;
		}

		alBufferData(buffers[i], m_format, newData.get(), BUFFER_SIZE, m_sampleRate);
	}

	alSourceQueueBuffers(source.c_obj(), bufferCount, buffers);
}

void OGGAudioStream::update(AudioSource& source)
{
	ALint buffersProcessed = 0;
	alGetSourcei(source.c_obj(), AL_BUFFERS_PROCESSED, &buffersProcessed);
	if (buffersProcessed <= 0)
		return;

	ALuint buffers[NUM_BUFFERS];
	alSourceUnqueueBuffers(source.c_obj(), buffersProcessed, buffers);
	read(source, buffersProcessed, buffers);
}

std::size_t OGGAudioStream::read_ogg_callback(void* ptr, std::size_t size, std::size_t nmemb, void* datasource)
{ return static_cast<OGGAudioStream*>(datasource)->read_ogg_callback_impl(ptr, size, nmemb); }

int OGGAudioStream::seek_ogg_callback(void* datasource, ogg_int64_t offset, int whence)
{ return static_cast<OGGAudioStream*>(datasource)->seek_ogg_callback_impl(offset, whence); }

long OGGAudioStream::tell_ogg_callback(void* datasource)
{ return static_cast<OGGAudioStream*>(datasource)->tell_ogg_callback_impl(); }

std::size_t OGGAudioStream::read_ogg_callback_impl(void* ptr, std::size_t size, std::size_t nmemb)
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

int OGGAudioStream::seek_ogg_callback_impl(ogg_int64_t offset, int whence)
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

long OGGAudioStream::tell_ogg_callback_impl()
{
	return static_cast<long>(m_input.tellg());
}
