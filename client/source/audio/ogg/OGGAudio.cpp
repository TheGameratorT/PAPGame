#include "OGGAudio.hpp"

#include "OGGAudioReader.hpp"

namespace OGGAudio
{
	AudioData readAll(std::istream& input)
	{
		OGGAudioReader reader(input);
		std::size_t dataSize = reader.getPcmSize();
		std::unique_ptr<char[]> data = std::make_unique<char[]>(dataSize);
		reader.read(data.get(), dataSize);
		return AudioData(std::move(data), dataSize, reader.getSampleRate(), reader.getFormat());
	}

	OGGAudioStreamSupplier makeStreamSupplier(std::istream& input)
	{
		return OGGAudioStreamSupplier(input);
	}
}
