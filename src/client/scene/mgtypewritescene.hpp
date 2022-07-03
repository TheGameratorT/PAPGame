#pragma once

#include "scene.hpp"

#include "gui/container.hpp"
#include "gui/image.hpp"
#include "gui/textbox.hpp"
#include "render/texture.hpp"
#include "typewriterstory.hpp"
#include "typestatestats.hpp"

class RoundStats;

class MGTypeWriteScene : public Scene
{
public:
	DECL_OBJECT(MGTypeWriteScene)

	void onCreate() override;
	void onUpdate() override;
	void onRender() override;
	void onDestroy() override;
	void onDestroyRequest() override;
	void onConnectionLost() override;
	void onInstructed() override;
	void onPlaceListingClosed() override;

	void onTypingStart(u32 storyID);
	void onTypingStats(const TypeStateStats& stats);
	void onRoundEnd(const RoundStats* stats, const RoundStats* finalStats);

private:
	mGUI::Container canvas;
	mGUI::Container typeCanvas;

	Texture bgTexture;
	Texture mainPanelTexture;
	Texture instructionTexture;
	Texture cntDwnTexture[3];

	mGUI::Image bgWidget;
	mGUI::Image mainPanelWidget;
	mGUI::TextBox storyTitleWidget;
	mGUI::TextBox textboxWidget;
	mGUI::TextBox typeTextboxWidget;

	mGUI::TextBox lettersTypedWidget;
	mGUI::TextBox playerLeadingWidget;
	mGUI::TextBox playerPlaceWidget;
	mGUI::TextBox youareBehindWidget;

	mGUI::TextBox typefastWidget;
	mGUI::Image cntDwnWidget;

	bool switchingScene = false;
	bool isFadingInScene = true;

	u32 curCntDwnTex = 3;

	std::vector<TypeWriterStory> stories;
	u32 curStory;
	bool playingStory = false;

	u32 charOffset;
	u32 lettersTyped;
	u32 lettersToType;

	TypeStateStats curStats;

	bool onPlayerType(Unicode::Codepoint cp);
	void finishStoryType();
	void loadStories();
	void updateTextboxes();
	void doCountDown();
};
