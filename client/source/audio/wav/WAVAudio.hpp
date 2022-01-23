#pragma once

#include <istream>
#include "../AudioData.hpp"
#include "WAVAudioStreamSupplier.hpp"

namespace WAVAudio
{
	AudioData readAll(std::istream& input);

	WAVAudioStreamSupplier makeStreamSupplier(std::istream& input);
}
