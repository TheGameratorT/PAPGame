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
	mGUI::Container lobbyMsgsCanvas;
	mGUI::Container playersCanvas;

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
	Texture btn3iTexture;
	Texture rcb0Texture;
	Texture rcb1Texture;
	Texture exitTexture;
	Texture settingsTexture;

	mGUI::Image bgWidget;
	mGUI::Image mainPanelWidget;
	mGUI::Button btn1Widget;
	mGUI::TextBox btn2Widget;
	mGUI::Button btn3Widget;
	mGUI::Image btn3iWidget;
	mGUI::Button exitWidget;
	mGUI::Button settingsWidget;

	mGUI::TextBox playersTextWidget;
	mGUI::TextBox lobbyChatTextWidget;
	mGUI::TextBox readyTextWidget;
	mGUI::TextBox serverInfoTextWidget;

	struct RenderLobbyMsg
	{
		mGUI::TextBox tb;
	};

	struct RenderPlayerEntry
	{
		mGUI::TextBox nameTextbox;
		mGUI::Image readyCheckbox;
	};

	std::vector<std::unique_ptr<RenderLobbyMsg>> renderMsgs;
	std::vector<std::unique_ptr<RenderPlayerEntry>> renderPlayers;

	static void setPlayerReady();
	void sendMessage();
};
