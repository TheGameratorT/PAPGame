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
	void onConnectionLost() override;

private:
	mGUI::Container canvas;
	mGUI::Container exitDialogCanvas;
	mGUI::Container textDialogCanvas;
	mGUI::Container connDialogCanvas;
	mGUI::Container creditsDialogCanvas;

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

	Texture txtBgTexture;
	Texture nndDtTexture;
	Texture ipdDtTexture;
	Texture txtTbTexture;
	Texture txtb1Texture;
	Texture txtb1cTexture;
	Texture txtb1hTexture;
	Texture txtb2Texture;
	Texture txtb2cTexture;
	Texture txtb2hTexture;

	Texture connBgTexture;
	Texture connDt1Texture;
	Texture connDt2Texture;

	Texture creditsBgTexture;
	Texture creditsDtTexture;

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

	mGUI::Image txtBgWidget;
	mGUI::Image txtDtWidget;
	mGUI::TextBox txtTbWidget;
	mGUI::Button txtbYWidget;
	mGUI::Button txtbNWidget;

	mGUI::Image connBgWidget;
	mGUI::Image connDtWidget;

	mGUI::Image creditsBgWidget;
	mGUI::Image creditsDtWidget;
	mGUI::Button creditsOkWidget;

	float dialogAnimTimer = 0.0f;
	float lastDialogAnimTimer = 0.0f;

	bool isCreating = false;

	bool exitDialogOpen = false;
	bool textDialogOpen = false;
	bool connDialogOpen = false;
	bool creditsDialogOpen = false;

	bool switchingScene = false;

	void setCommonWidgetsEnabled(bool enabled);

	void renderExitDialog(const Vec2i& area, float windowFactor);
	void showExitDialog();
	void closeExitDialog();

	void renderTextDialog(const Vec2i& area, float windowFactor);
	void showTextDialog();
	void closeTextDialog();

	void showNicknameDialog();
	void showIPDialog();
	void connectToServer();

	void renderConnDialog(const Vec2i& area, float windowFactor);
	void showConnDialog();
	void closeConnDialog();

	void renderCreditsDialog(const Vec2i& area, float windowFactor);
	void showCreditsDialog();
	void closeCreditsDialog();

	void resetDialogAnim();
	[[nodiscard]] bool isDialogOpen() const;

	static void createServerGame();
};
