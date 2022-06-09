#pragma once

#include <string>
#include <unordered_map>

#include "filesystem/file.hpp"
#include "common/concepts.hpp"

class ConfigFile
{
public:
	void load(const Path& filePath);
	void reload();
	void save();

	std::string get(const std::string& key, const std::string& defValue);

	template<CC::Arithmetic T>
	T get(const std::string& key, T defValue);

	void set(const std::string& key, const std::string& value);

	template<CC::Arithmetic T>
	void set(const std::string& key, T value);

private:
	std::unordered_map<std::string, std::string> m_map;
	Path m_filePath;

	std::string serialize();
};
