#include "utils.hpp"

namespace Utils
{
	std::string escape(const std::string& str)
	{
		std::string _str = str;
		for (size_t i = 0; i < _str.size(); i++)
		{
			char ch;
			switch (_str[i])
			{
			case '\r': ch = 'r'; goto putSimpleEscape;
			case '\n': ch = 'n'; goto putSimpleEscape;
			case '\t': ch = 't'; goto putSimpleEscape;
			case '\v': ch = 'v'; goto putSimpleEscape;
			case '\a': ch = 'a'; goto putSimpleEscape;
			}
			continue;

			putSimpleEscape:
			_str[i++] = '\\';
			_str.insert(_str.begin() + static_cast<int>(i), ch);
		}
		return _str;
	}

	std::string unescape(const std::string& str)
	{
		std::string _str = str;
		size_t pos = 0;
		while ((pos = _str.find('\\', pos)) != std::string::npos)
		{
			size_t rem = 1;
			char esc = _str[pos + 1];
			if (esc >= '0' && esc <= '7') {
				_str[pos] = static_cast<char>(std::stoul(&_str[pos + 1], &rem, 8));
			}
			else if (esc == 'x') {
				_str[pos] = static_cast<char>(std::stoul(&_str[pos + 2], &rem, 16));
				rem += 1;
			}
			else {
				rem = 1;
				switch (esc) {
				case 'r': _str[pos] = '\r'; break;
				case 'n': _str[pos] = '\n'; break;
				case 't': _str[pos] = '\t'; break;
				case 'v': _str[pos] = '\v'; break;
				case 'a': _str[pos] = '\a'; break;
				default:
					_str[pos] = esc;
					break;
				}
			}
			_str.erase(pos + 1, rem);
		}
		return _str;
	}
}
