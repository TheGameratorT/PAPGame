#include "configfile.hpp"

#include <sstream>

#include "util/string.hpp"
#include "util/log.hpp"

void ConfigFile::load(const Path& filePath)
{
	m_filePath = filePath;
	reload();
}

void ConfigFile::reload()
{
	m_map.clear();
	if (!m_filePath.exists())
		return;
	File file(m_filePath, File::In | File::Text);
	if (!file.open())
		Log::error("ConfigFile", "Failed to load config file: " + m_filePath.toString());
	std::string text = file.readAllText();
	file.close();
	std::istringstream iss(text);
	int lineNum = 1;
	for (std::string line; std::getline(iss, line); lineNum++)
	{
		std::size_t lineSize = line.size();
		if (lineSize < 3) // If line is empty
			continue;
		std::size_t sepIndex = line.find('=');
		std::size_t valIndex;
		if (sepIndex == std::string::npos || (valIndex = sepIndex + 1) == lineSize)
		{
			Log::warn("ConfigFile", "Line " + std::to_string(lineNum) + " is invalid");
			continue;
		}
		std::string key = line.substr(0, sepIndex);
		std::string value = line.substr(valIndex, lineSize - valIndex);
		m_map.insert({key, value});
	}
	Log::info("ConfigFile", "Loaded config file: " + m_filePath.toString());
}

void ConfigFile::save()
{
	std::string data = serialize();
	File file(m_filePath, File::Out | File::Text);
	if (!file.open())
		Log::error("ConfigFile", "Failed to save config file: " + m_filePath.toString());
	file.write(data);
	file.close();
}

std::string ConfigFile::get(const std::string& key, const std::string& defValue)
{
	if (!m_map.contains(key))
	{
		m_map.insert({key, defValue});
		return defValue;
	}
	return m_map.at(key);
}

template<Arithmetic T>
T ConfigFile::get(const std::string& key, T defValue)
{
	if (Equal<int, T>)
		return static_cast<int>(get<long long>(key, defValue)); // Reuse the "long long" getter function
	else if (Equal<float, T>)
		return static_cast<float>(get<double>(key, defValue)); // Reuse the "double" getter function

	if (!m_map.contains(key))
	{
		if (Equal<bool, T>)
			m_map.insert({key, String::fromBool(defValue)});
		else
			m_map.insert({key, std::to_string(defValue)});
		return defValue;
	}
	std::string valueStr = m_map.at(key);
	if (Equal<bool, T>)
		return valueStr == "true";
	else if (Equal<long long, T>)
		return std::stol(valueStr);
	return std::stod(valueStr);
}

template int ConfigFile::get<int>(const std::string&, int);
template long long ConfigFile::get<long long>(const std::string&, long long);
template float ConfigFile::get<float>(const std::string&, float);
template double ConfigFile::get<double>(const std::string&, double);
template bool ConfigFile::get<bool>(const std::string&, bool);

void ConfigFile::set(const std::string& key, const std::string& value)
{
	m_map.insert_or_assign(key, value);
}

template<Arithmetic T>
void ConfigFile::set(const std::string& key, T value)
{
	if (Equal<int, T>)
	{
		set<long long>(key, value); // Reuse the "long long" setter function
		return;
	}
	else if (Equal<float, T>)
	{
		set<double>(key, value); // Reuse the "double" getter function
		return;
	}

	std::string valueStr;
	if (Equal<bool, T>)
		valueStr = String::fromBool(value);
	else if (Integer<T> || Equal<double, T>)
		valueStr = std::to_string(value);
	m_map.insert_or_assign(key, valueStr);
}

template void ConfigFile::set<int>(const std::string&, int);
template void ConfigFile::set<long long>(const std::string&, long long);
template void ConfigFile::set<float>(const std::string&, float);
template void ConfigFile::set<double>(const std::string&, double);
template void ConfigFile::set<bool>(const std::string&, bool);

std::string ConfigFile::serialize()
{
	std::ostringstream cfgStrm;
	for (const auto& [key, value] : m_map)
		cfgStrm << key << '=' << value << '\n';
	return cfgStrm.str();
}
