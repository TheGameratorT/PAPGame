#pragma once

#include "scene.hpp"
#include "render/texture.hpp"
#include "gui/image.hpp"
#include "gui/button.hpp"
#include "gui/textbox.hpp"
#include "gui/container.hpp"

class LobbyScene : public Scene
{
public:
	DECL_OBJECT(LobbyScene)

	void onCreate() override;
	void onUpdate() override;
	void onRender() override;
	void onDestroy() override;
	void onDestroyRequest() override;

private:
	mGUI::Container canvas;

	Texture bgTexture;
	Texture mainPanelTexture;
	Texture btn1nTexture;
	Texture btn1hTexture;
	Texture btn1cTexture;
	Texture btn2nTexture;
	Texture btn2hTexture;
	Texture btn3nTexture;
	Texture btn3hTexture;
	Texture btn3cTexture;

	mGUI::Image bgWidget;
	mGUI::Image mainPanelWidget;
	mGUI::Button btn1Widget;
	mGUI::TextBox btn2Widget;
	mGUI::Button btn3Widget;

	mGUI::TextBox playersTextWidget;
	mGUI::TextBox lobbyChatTextWidget;

	void sendMessage();
};
