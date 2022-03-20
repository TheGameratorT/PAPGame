#pragma once

#include <string>
#include <filesystem>

namespace ShaderLoader
{
	std::string loadSource(const std::filesystem::path& path);
};
