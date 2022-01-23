#pragma once

#include <istream>
#include "../AudioData.hpp"
#include "OGGAudioStreamSupplier.hpp"

namespace OGGAudio
{
	AudioData readAll(std::istream& input);

	OGGAudioStreamSupplier makeStreamSupplier(std::istream& input);
}
