#include "lobbyscene.hpp"

#include "game.hpp"
#include "player.hpp"
#include "network/gamenet.hpp"
#include "network/packet/pkt_c2s_lobbymsg.hpp"
#include "network/packet/pkt_c2s_playerready.hpp"
#include "screen/connectionlostscreen.hpp"
#include "util.hpp"
#include "render/fader.hpp"

constexpr u32 TICK_RATE = 30;
constexpr float TICK_DURATION = 1.0f / TICK_RATE;

constexpr float BASE_WND_AREA = 640.0f * 480.0f;

constexpr Vec2i MAINPANEL_IMG_SIZE = { 549, 261 };
constexpr float MAINPANEL_IMG_ARATIO = float(MAINPANEL_IMG_SIZE.x) / float(MAINPANEL_IMG_SIZE.y);
constexpr Vec2i BTN1_IMG_SIZE = { 219, 48 };
constexpr float BTN1_IMG_ARATIO = float(BTN1_IMG_SIZE.x) / float(BTN1_IMG_SIZE.y);
constexpr Vec2i BTN2_IMG_SIZE = { 262, 48 };
constexpr float BTN2_IMG_ARATIO = float(BTN2_IMG_SIZE.x) / float(BTN2_IMG_SIZE.y);
constexpr Vec2i BTN3_IMG_SIZE = { 48, 48 };
constexpr float BTN3_IMG_ARATIO = float(BTN3_IMG_SIZE.x) / float(BTN3_IMG_SIZE.y);

constexpr float ICON_IMG_ARATIO = 64.0f / 64.0f;

const SizeT MAX_DISPLAY_LOBBY_MSG_COUNT = 12;

IMPL_OBJECT(LobbyScene)

void LobbyScene::onCreate()
{
	Game::setTickRate(TICK_RATE);

	bgTexture.load("@/lobby/background.png");
	mainPanelTexture.load("@/lobby/main_panels.png", GLE::TextureFilter::None);
	btn1nTexture.load("@/lobby/btn1_n.png", GLE::TextureFilter::None);
	btn1hTexture.load("@/lobby/btn1_h.png", GLE::TextureFilter::None);
	btn1cTexture.load("@/lobby/btn1_c.png", GLE::TextureFilter::None);
	btn2nTexture.load("@/lobby/btn2_n.png", GLE::TextureFilter::None);
	btn2hTexture.load("@/lobby/btn2_h.png", GLE::TextureFilter::None);
	btn3nTexture.load("@/lobby/btn3_n.png", GLE::TextureFilter::None);
	btn3hTexture.load("@/lobby/btn3_h.png", GLE::TextureFilter::None);
	btn3cTexture.load("@/lobby/btn3_c.png", GLE::TextureFilter::None);
	btn3iTexture.load("@/lobby/btn3_i.png", GLE::TextureFilter::None);
	rcb0Texture.load("@/lobby/rcb0.png", GLE::TextureFilter::None);
	rcb1Texture.load("@/lobby/rcb1.png", GLE::TextureFilter::None);
	exitTexture.load("@/exit.png", GLE::TextureFilter::None);
	settingsTexture.load("@/settings.png", GLE::TextureFilter::None);

	bgWidget.setTexture(&bgTexture);
	mainPanelWidget.setTexture(&mainPanelTexture);
	btn1Widget.setTexture(&btn1nTexture);
	btn1Widget.setHoverTexture(&btn1hTexture);
	btn1Widget.setHeldTexture(&btn1cTexture);
	btn2Widget.setTexture(&btn2nTexture);
	btn2Widget.setHoverTexture(&btn2hTexture);
	btn3Widget.setTexture(&btn3nTexture);
	btn3Widget.setHoverTexture(&btn3hTexture);
	btn3Widget.setHeldTexture(&btn3cTexture);
	btn3iWidget.setTexture(&btn3iTexture);
	exitWidget.setTexture(&exitTexture);
	settingsWidget.setTexture(&settingsTexture);

	btn1Widget.setOnClick([](){ setPlayerReady(); });

	auto sendMsgLambda = [this](){ sendMessage(); };

	btn2Widget.setFont("smooth");
	btn2Widget.setFontScale(0.35f);
	btn2Widget.setTextSidePadding(1500.0f);
	btn2Widget.setTextYOffset(1600.0f);
	btn2Widget.setMaxCharacters(24);

	btn2Widget.setOnEnter(sendMsgLambda);
	btn3Widget.setOnClick(sendMsgLambda);

	playersTextWidget.setText(u8"Players");
	playersTextWidget.setFont("smooth");
	playersTextWidget.setReadOnly(true);

	lobbyChatTextWidget.setText(u8"Lobby Chat");
	lobbyChatTextWidget.setFont("smooth");
	lobbyChatTextWidget.setReadOnly(true);

	readyTextWidget.setText(u8"Ready");
	readyTextWidget.setFont("smooth");
	readyTextWidget.setReadOnly(true);

	U8String welcomeTxt = u8"Welcome!\nServer: ";
	welcomeTxt.append(Util::stringAsUtf8(GameNet::getServerAddress()));
	serverInfoTextWidget.setText(welcomeTxt);
	serverInfoTextWidget.setFont("smooth");
	serverInfoTextWidget.setReadOnly(true);

	bgWidget.setZIndex(0);
	btn3Widget.setZIndex(1050);
	btn3iWidget.setZIndex(1051);
	playersTextWidget.setZIndex(1051);
	lobbyChatTextWidget.setZIndex(1051);
	readyTextWidget.setZIndex(1051);
	serverInfoTextWidget.setZIndex(1051);

	canvas.addWidget(bgWidget);
	canvas.addWidget(mainPanelWidget);
	canvas.addWidget(btn1Widget);
	canvas.addWidget(btn2Widget);
	canvas.addWidget(btn3Widget);
	canvas.addWidget(btn3iWidget);
	canvas.addWidget(playersTextWidget);
	canvas.addWidget(lobbyChatTextWidget);
	canvas.addWidget(readyTextWidget);
	canvas.addWidget(serverInfoTextWidget);
	canvas.addWidget(exitWidget);
	canvas.addWidget(settingsWidget);
	canvas.setZIndex(0);

	lobbyMsgsCanvas.setZIndex(1051);
	canvas.addWidget(lobbyMsgsCanvas);

	playersCanvas.setZIndex(1051);
	canvas.addWidget(playersCanvas);

	Game::getGUI().getContainer().addWidget(canvas);
}

void LobbyScene::onUpdate()
{
	if (switchingScene)
	{
		if (Fader::hasFadingFinished())
			destroy();
		return;
	}

	lobbyMsgsCanvas.clear();
	renderMsgs.clear();
	auto& lobbyMsgs = Game::getLobbyMsgs();
	SizeT lobbyMsgCount = lobbyMsgs.size();
	SizeT lobbyDisplayMsgCount = Math::min(lobbyMsgCount, MAX_DISPLAY_LOBBY_MSG_COUNT);
	for (SizeT i = lobbyMsgCount - lobbyDisplayMsgCount; i < lobbyMsgCount; i++)
	{
		auto renderLobbyMsg = std::make_unique<RenderLobbyMsg>();
		mGUI::TextBox& tb = renderLobbyMsg->tb;
		tb.setFont("smooth");
		tb.setText(lobbyMsgs[i]);
		tb.setReadOnly(true);
		lobbyMsgsCanvas.addWidget(tb);
		renderMsgs.push_back(std::move(renderLobbyMsg));
	}

	playersCanvas.clear();
	renderPlayers.clear();
	for (auto& player : Game::getPlayers())
	{
		auto renderPlayer = std::make_unique<RenderPlayerEntry>();
		mGUI::TextBox& tb = renderPlayer->nameTextbox;
		tb.setFont("smooth");
		tb.setText(player->getName());
		tb.setReadOnly(true);
		playersCanvas.addWidget(tb);
		mGUI::Image& cb = renderPlayer->readyCheckbox;
		cb.setTexture(player->getReady() ? &rcb1Texture : &rcb0Texture);
		playersCanvas.addWidget(cb);
		renderPlayers.push_back(std::move(renderPlayer));
	}
}

void LobbyScene::onRender()
{
	auto area = Vec2i(Game::getFramebufferSize());

	float windowFactor = (float(area.x * area.y) / BASE_WND_AREA);

	bgWidget.setBounds({0, 0, area.x, area.y});

	// MAIN PANEL

	float mainPanelSizeGrowthMul = 4.5f;
	float mainPanelSizeMul = (windowFactor / mainPanelSizeGrowthMul) + (1.0f - (1.0f / mainPanelSizeGrowthMul));

	i32 mainPanelSize = std::lroundl(549.0f * mainPanelSizeMul);
	i32 mainPanelWidth = mainPanelSize;
	i32 mainPanelHeight = std::lroundl(1.0f / MAINPANEL_IMG_ARATIO * float(mainPanelSize));

	i32 panelWidgetX = (area.x - mainPanelWidth) / 2;

	mainPanelWidget.setBounds({
		panelWidgetX,
		std::lroundl(50.0f * mainPanelSizeMul),
		mainPanelWidth,
		mainPanelHeight
	});

	// READY BUTTON

	i32 btn1Size = std::lroundl(219.0f * mainPanelSizeMul);
	i32 btn1Width = btn1Size;
	i32 btn1Height = std::lroundl(1.0f / BTN1_IMG_ARATIO * float(btn1Size));

	btn1Widget.setBounds({
		panelWidgetX,
		std::lroundl(322.0f * mainPanelSizeMul),
		btn1Width,
		btn1Height
	});

	// CHAT TEXTBOX

	i32 btn2Size = std::lroundl(262.0f * mainPanelSizeMul);
	i32 btn2Width = btn2Size;
	i32 btn2Height = std::lroundl(1.0f / BTN2_IMG_ARATIO * float(btn2Size));

	btn2Widget.setBounds({
		panelWidgetX + std::lroundl(239.0f * mainPanelSizeMul),
		std::lroundl(322.0f * mainPanelSizeMul),
		btn2Width,
		btn2Height
	});

	// SEND CHAT BUTTON

	i32 btn3Size = std::lroundl(48.0f * mainPanelSizeMul);
	i32 btn3Width = btn3Size;
	i32 btn3Height = std::lroundl(1.0f / BTN3_IMG_ARATIO * float(btn3Size));

	RectI btn3bounds = {
		panelWidgetX + std::lroundl(501.0f * mainPanelSizeMul),
		std::lroundl(322.0f * mainPanelSizeMul),
		btn3Width,
		btn3Height
	};
	btn3Widget.setBounds(btn3bounds);
	btn3iWidget.setBounds(btn3bounds);

	// PLAYERS TEXT

	i32 textBaseHeight = std::lroundl(57.0f * mainPanelSizeMul);

	playersTextWidget.setBounds({
		panelWidgetX + std::lroundl(70.0f * mainPanelSizeMul),
		textBaseHeight,
		std::lroundl(77.0f * mainPanelSizeMul),
		std::lroundl(23.0f * mainPanelSizeMul)
	});

	// LOBBY CHAT TEXT

	lobbyChatTextWidget.setBounds({
		panelWidgetX + std::lroundl(336.0f * mainPanelSizeMul),
		textBaseHeight,
		std::lroundl(120.0f * mainPanelSizeMul),
		std::lroundl(23.0f * mainPanelSizeMul)
	});

	// READY TEXT

	readyTextWidget.setBounds({
		panelWidgetX + std::lroundl(75.0f * mainPanelSizeMul),
		std::lroundl(330.0f * mainPanelSizeMul),
		std::lroundl(77.0f * mainPanelSizeMul),
		std::lroundl(23.0f * mainPanelSizeMul)
	});

	// LOBBY CHAT MESSAGES

	i32 msgSpacing = std::lroundl(16.0f * mainPanelSizeMul);
	i32 msgYPos = std::lroundl(100.0f * mainPanelSizeMul);

	for (auto& renderMsg : renderMsgs)
	{
		mGUI::TextBox& textBox = renderMsg->tb;
		textBox.setBounds({
			panelWidgetX + std::lroundl(253.0f * mainPanelSizeMul),
			msgYPos,
			std::lroundl(282.0f * mainPanelSizeMul),
			std::lroundl(16.0f * mainPanelSizeMul),
		});
		msgYPos += msgSpacing;
	}

	// PLAYER LIST

	i32 plnameSpacing = std::lroundl(16.0f * mainPanelSizeMul);
	i32 plnameYPos = std::lroundl(100.0f * mainPanelSizeMul);

	i32 plcbYPos = std::lroundl(105.0f * mainPanelSizeMul);

	for (auto& renderPl : renderPlayers)
	{
		mGUI::TextBox& textBox = renderPl->nameTextbox;
		textBox.setBounds({
			panelWidgetX + std::lroundl(16.0f * mainPanelSizeMul),
			plnameYPos,
			std::lroundl(282.0f * mainPanelSizeMul),
			std::lroundl(16.0f * mainPanelSizeMul),
		});
		mGUI::Image& checkBox = renderPl->readyCheckbox;
		checkBox.setBounds({
			panelWidgetX + std::lroundl(186.0f * mainPanelSizeMul),
			plcbYPos,
			std::lroundl(14.0f * mainPanelSizeMul),
			std::lroundl(14.0f * mainPanelSizeMul),
		});
		plnameYPos += plnameSpacing;
		plcbYPos += plnameSpacing;
	}

	// EXIT + SETTINGS BUTTON

	float buttonSizeGrowthMul2 = 4.5f;
	float buttonSizeMul2 = (windowFactor / buttonSizeGrowthMul2) + (1.0f - (1.0f / buttonSizeGrowthMul2));

	i32 buttonSize2 = std::lroundl(64.0f * buttonSizeMul2);
	i32 buttonWidth2 = buttonSize2;
	i32 buttonHeight2 = std::lroundl(1.0f / ICON_IMG_ARATIO * float(buttonSize2));

	i32 buttonXSpacing = std::lroundl(16 * buttonSizeMul2);
	i32 buttonY2 = area.y - buttonSize2 - std::lroundl(12.0f * buttonSizeMul2);

	settingsWidget.setBounds({area.x - buttonWidth2 - buttonXSpacing, buttonY2, buttonWidth2, buttonHeight2});

	exitWidget.setBounds({area.x - (buttonWidth2 * 2) - (buttonXSpacing * 2), buttonY2, buttonWidth2, buttonHeight2});

	// SERVER INFO TEXT

	i32 siTextSize = std::lroundl(28.0f * buttonSizeMul2);
	i32 siTextWidth = siTextSize;
	i32 siTextHeight = siTextSize;

	serverInfoTextWidget.setBounds({
		std::lroundl(32 * buttonSizeMul2),
		area.y - std::lroundl(72.0f * buttonSizeMul2),
		siTextWidth,
		siTextHeight
	});
}

void LobbyScene::onDestroy()
{
	canvas.clear();
	Game::getGUI().getContainer().removeWidget(canvas);

	bgTexture.destroy();
	mainPanelTexture.destroy();
	btn1nTexture.destroy();
	btn1hTexture.destroy();
	btn1cTexture.destroy();
	btn2nTexture.destroy();
	btn2hTexture.destroy();
	btn3nTexture.destroy();
	btn3hTexture.destroy();
	btn3cTexture.destroy();
	btn3iTexture.destroy();
	rcb0Texture.destroy();
	rcb1Texture.destroy();
	exitTexture.destroy();
	settingsTexture.destroy();
}

void LobbyScene::onDestroyRequest()
{
	switchingScene = true;
}

void LobbyScene::onConnectionLost()
{
	ConnectionLostScreen::show();
}

void LobbyScene::setPlayerReady()
{
	using PReadyType = PKT_C2S_PlayerReady::ReadyType;

	Player* player = Game::getPlayer();
	GameNet::getConnection()->sendPacket<PKT_C2S_PlayerReady>(player->getReady() ? PReadyType::NotReady : PReadyType::Ready);
}

void LobbyScene::sendMessage()
{
	const U8String& text = btn2Widget.getText();
	if (text.empty())
		return;
	GameNet::getConnection()->sendPacket<PKT_C2S_LobbyMsg>(text);
	btn2Widget.clear();
}
