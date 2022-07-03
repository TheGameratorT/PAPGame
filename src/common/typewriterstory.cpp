#include "typewriterstory.hpp"

#include "filesystem/file.hpp"
#include "util/log.hpp"
#include "util.hpp"

std::vector<TypeWriterStory> TypeWriterStory::loadStories(const Path& filePath)
{
	std::vector<TypeWriterStory> stories;

	File file(filePath, File::In | File::Text);
	if (!file.open())
		Log::error("TypeWriter", "Failed to load config file: " + filePath.toString());
	std::string text = file.readAllText();
	file.close();

	std::istringstream iss(text);
	std::string title;
	std::string story;
	SizeT charCount = 0;
	for (std::string line; std::getline(iss, line);)
	{
		if (line.empty()) // end of story
		{
			stories.emplace_back(Util::stringAsUtf8(title), Util::stringAsUtf8(story), charCount);
			story.clear();
			charCount = 0;
			continue;
		}
		if (line[0] == '-') // found a title
		{
			title = line.substr(1);
			continue;
		}
		story += line + '\n';
		charCount += line.size();
	}

	return stories;
}
