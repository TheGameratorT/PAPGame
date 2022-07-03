#pragma once

#include <string>
#include <vector>

#include "filesystem/path.hpp"
#include "locale/unicodestring.hpp"

class TypeWriterStory
{
public:
	TypeWriterStory(U8String title, U8String text, SizeT charCount) :
		m_title(std::move(title)), m_text(std::move(text)), m_charCount(charCount)
	{}

	[[nodiscard]] constexpr const U8String& getTitle() const { return m_title; }
	[[nodiscard]] constexpr const U8String& getText() const { return m_text; }
	[[nodiscard]] constexpr SizeT getCharCount() const { return m_charCount; }

	static std::vector<TypeWriterStory> loadStories(const Path& filePath);

private:
	U8String m_title;
	U8String m_text;
	SizeT m_charCount;
};
