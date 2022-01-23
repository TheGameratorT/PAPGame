#include "WAVAudio.hpp"

namespace WAVAudio
{
	AudioData readAll(std::istream& input)
	{
		WAVAudioReader reader(input);
		std::size_t dataSize = reader.getPcmSize();
		std::unique_ptr<char[]> data = std::make_unique<char[]>(dataSize);
		reader.read(data.get(), dataSize);
		return AudioData(std::move(data), dataSize, reader.getSampleRate(), reader.getFormat());
	}

	WAVAudioStreamSupplier makeStreamSupplier(std::istream& input)
	{
		return WAVAudioStreamSupplier(input);
	}
}
