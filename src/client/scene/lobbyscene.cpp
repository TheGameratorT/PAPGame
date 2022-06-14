#include "lobbyscene.hpp"

#include "game.hpp"

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

	auto sendMsgLamda = [this](){ sendMessage(); };

	btn2Widget.setFont("smooth");
	btn2Widget.setFontScale(0.35f);
	btn2Widget.setTextSidePadding(1500.0f);
	btn2Widget.setTextYOffset(1600.0f);
	btn2Widget.setMaxCharacters(24);

	btn2Widget.setOnEnter(sendMsgLamda);
	btn3Widget.setOnClick(sendMsgLamda);

	bgWidget.setZIndex(0);

	canvas.addWidget(bgWidget);
	canvas.addWidget(mainPanelWidget);
	canvas.addWidget(btn1Widget);
	canvas.addWidget(btn2Widget);
	canvas.addWidget(btn3Widget);
	canvas.setZIndex(0);

	Game::getGUI().getContainer().addWidget(canvas);
}

void LobbyScene::onUpdate()
{

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
		i32(50.0f * mainPanelSizeMul),
		mainPanelWidth,
		mainPanelHeight
	});

	// READY BUTTON

	i32 btn1Size = std::lroundl(219.0f * mainPanelSizeMul);
	i32 btn1Width = btn1Size;
	i32 btn1Height = std::lroundl(1.0f / BTN1_IMG_ARATIO * float(btn1Size));

	btn1Widget.setBounds({
		panelWidgetX,
		i32(322.0f * mainPanelSizeMul),
		btn1Width,
		btn1Height
	});

	// CHAT TEXTBOX

	i32 btn2Size = std::lroundl(262.0f * mainPanelSizeMul);
	i32 btn2Width = btn2Size;
	i32 btn2Height = std::lroundl(1.0f / BTN2_IMG_ARATIO * float(btn2Size));

	btn2Widget.setBounds({
		panelWidgetX + i32(239.0f * mainPanelSizeMul),
		i32(322.0f * mainPanelSizeMul),
		btn2Width,
		btn2Height
	});

	// SEND CHAT BUTTON

	i32 btn3Size = std::lroundl(48.0f * mainPanelSizeMul);
	i32 btn3Width = btn3Size;
	i32 btn3Height = std::lroundl(1.0f / BTN3_IMG_ARATIO * float(btn3Size));

	btn3Widget.setBounds({
		panelWidgetX + i32(501.0f * mainPanelSizeMul),
		i32(322.0f * mainPanelSizeMul),
		btn3Width,
		btn3Height
	});
}

void LobbyScene::onDestroy()
{
	canvas.clear();
	Game::getGUI().getContainer().removeWidget(canvas);

	bgTexture.destroy();
}

void LobbyScene::onDestroyRequest()
{
	destroy();
}

void LobbyScene::sendMessage()
{
	btn2Widget.clear();
}
