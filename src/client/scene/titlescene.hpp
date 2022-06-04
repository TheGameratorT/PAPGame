#pragma once

#include "scene.hpp"
#include "render/texture.hpp"
#include "gui/image.hpp"
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
	mGUI::Image titleWidget;
	mGUI::Image bgWidget;
	mGUI::Image createGameButtonWidget;
	mGUI::Image joinGameButtonWidget;
	mGUI::Image createGameTextWidget;
	mGUI::Image joinGameTextWidget;
	mGUI::Image exitWidget;
	mGUI::Image aboutWidget;
	mGUI::Image settingsWidget;
	mGUI::Image edBgWidget;
	mGUI::Image edDtWidget;
	mGUI::Image edbYWidget;
	mGUI::Image edbNWidget;

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
