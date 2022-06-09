#pragma once

#include "scene.hpp"
#include "render/texture.hpp"
#include "gui/image.hpp"
#include "gui/button.hpp"
#include "gui/textbox.hpp"
#include "gui/container.hpp"

class TitleScene : public Scene
{
public:
	DECL_OBJECT(TitleScene)

	void onCreate() override;
	void onUpdate() override;
	void onRender() override;
	void onDestroy() override;
	void onDestroyRequest() override;

private:
	mGUI::Container canvas;
	mGUI::Container exitDialogCanvas;
	mGUI::Container nicknameDialogCanvas;
	Texture titleTexture;
	Texture bgTexture;
	Texture buttonTexture;
	Texture buttonHeldTexture;
	Texture buttonHoverTexture;
	Texture createGameTextTexture;
	Texture joinGameTextTexture;
	Texture exitTexture;
	Texture aboutTexture;
	Texture settingsTexture;
	Texture edBgTexture;
	Texture edDtTexture;
	Texture edb1Texture;
	Texture edb2Texture;
	Texture edb3Texture;
	Texture nndBgTexture;
	Texture nndDtTexture;
	Texture nndTbTexture;
	Texture nndb1Texture;
	Texture nndb1cTexture;
	Texture nndb1hTexture;
	Texture nndb2Texture;
	Texture nndb2cTexture;
	Texture nndb2hTexture;
	mGUI::Image titleWidget;
	mGUI::Image bgWidget;
	mGUI::Button createGameButtonWidget;
	mGUI::Button joinGameButtonWidget;
	mGUI::Image createGameTextWidget;
	mGUI::Image joinGameTextWidget;
	mGUI::Button exitWidget;
	mGUI::Button aboutWidget;
	mGUI::Button settingsWidget;
	mGUI::Image edBgWidget;
	mGUI::Image edDtWidget;
	mGUI::Button edbYWidget;
	mGUI::Button edbNWidget;
	mGUI::Image nndBgWidget;
	mGUI::Image nndDtWidget;
	mGUI::TextBox nndTbWidget;
	mGUI::Button nndbYWidget;
	mGUI::Button nndbNWidget;

	float dialogAnimTimer = 0.0f;
	float lastDialogAnimTimer = 0.0f;

	bool exitDialogOpen = false;
	bool nicknameDialogOpen = false;

	void setCommonWidgetsEnabled(bool enabled);

	void renderExitDialog(const Vec2i& area, float windowFactor);
	void showExitDialog();
	void closeExitDialog();

	void renderNicknameDialog(const Vec2i& area, float windowFactor);
	void showNicknameDialog();
	void closeNicknameDialog();
};
