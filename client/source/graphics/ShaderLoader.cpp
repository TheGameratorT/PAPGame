#include "ShaderLoader.hpp"

#include <fstream>
#include <sstream>

std::string ShaderLoader::loadSource(const std::filesystem::path& path)
{
	std::ifstream input(path, std::ios_base::binary);
	std::ostringstream sstream;
	sstream << input.rdbuf();
	return sstream.str();
}
