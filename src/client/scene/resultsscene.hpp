#pragma once

#include "scene.hpp"

#include "render/gle/texture2d.hpp"

#include "render/texture.hpp"
#include "input/keyhandle.hpp"
#include "gui/container.hpp"
#include "gui/image.hpp"
#include "gui/button.hpp"
#include "gui/textbox.hpp"

class RoundStats;

class ResultsScene : public Scene
{
public:
	DECL_OBJECT(ResultsScene)

	void onCreate() override;
	void onUpdate() override;
	void onRender() override;
	void onDestroy() override;
	void onDestroyRequest() override;

private:
	mGUI::Container canvas;
	mGUI::Container scoreCanvas;

	Texture bgTexture;
	Texture titleTexture;
	Texture panelTexture;
	Texture btnNTexture;
	Texture btnHTexture;

	mGUI::Image bgWidget;
	mGUI::Image titleWidget;
	mGUI::Image panelWidget;
	mGUI::Button btnWidget;

	bool switchingScene = false;

	struct PlayerScoreEntry
	{
		mGUI::TextBox tb;
	};

	std::vector<PlayerScoreEntry> scoreEntries;

	void generateRenderables(const RoundStats* stats);
	static u32 getPlayerPlace();
};
