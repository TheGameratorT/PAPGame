#pragma once

#include <vector>

#include "../minigame.hpp"
#include "typewriterstory.hpp"

class Player;

class MGTypeWriter : public MiniGame
{
public:
	DECL_MINIGAME()

	void onCreate() override;
	void onUpdate() override;
	void onDestroy() override;
	void onClientsReady() override;

	[[nodiscard]] constexpr u32 getStoryTextSize() const { return lettersToType; }

private:
	std::vector<TypeWriterStory> stories;
	u32 curStory;
	u32 lettersToType;
	u32 playedRounds = 0;

	bool playingStory = false;

	bool* playedStoryList;

	void decideStory();
	void startStory();
};
