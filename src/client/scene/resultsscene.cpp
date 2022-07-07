#include "resultsscene.hpp"

#include "game.hpp"
#include "titlescene.hpp"
#include "screen/loadingscreen.hpp"
#include "scene/lobbyscene.hpp"

#include "render/gle/render.hpp"
#include "render/fader.hpp"

#include "player.hpp"
#include "roundstats.hpp"
#include "util.hpp"

constexpr u32 TICK_RATE = 30;
constexpr float TICK_DURATION = 1.0f / TICK_RATE;

constexpr float BASE_WND_AREA = 640.0f * 480.0f;

constexpr Vec2i PANEL_IMG_SIZE = { 554, 240 };
constexpr float PANEL_IMG_ARATIO = float(PANEL_IMG_SIZE.x) / float(PANEL_IMG_SIZE.y);
constexpr Vec2i TITLE_IMG_SIZE = { 570, 64 };
constexpr float TITLE_IMG_ARATIO = float(TITLE_IMG_SIZE.x) / float(TITLE_IMG_SIZE.y);
constexpr Vec2i BTN_IMG_SIZE = { 123, 30 };
constexpr float BTN_IMG_ARATIO = float(BTN_IMG_SIZE.x) / float(BTN_IMG_SIZE.y);

IMPL_OBJECT(ResultsScene)

void ResultsScene::onCreate()
{
	Game::setTickRate(TICK_RATE);

	u32 playerPlace = getPlayerPlace();

	bgTexture.load("@/results/bg.png");

	static char titleTexPath[] = "@/results/titleX.png";
	titleTexPath[15] = char('1' + (playerPlace != 0));
	titleTexture.load(titleTexPath, GLE::TextureFilter::None);

	panelTexture.load("@/results/panel.png");
	btnNTexture.load("@/results/continueN.png");
	btnHTexture.load("@/results/continueH.png");

	bgWidget.setTexture(&bgTexture);
	titleWidget.setTexture(&titleTexture);
	panelWidget.setTexture(&panelTexture);
	btnWidget.setTexture(&btnNTexture);
	btnWidget.setHoverTexture(&btnHTexture);

	btnWidget.setOnClick([](){
		Game::switchScene<LobbyScene>();
	});

	bgWidget.setZIndex(0);
	panelWidget.setZIndex(1);
	btnWidget.setZIndex(2);
	scoreCanvas.setZIndex(3);

	canvas.addWidget(bgWidget);
	canvas.addWidget(titleWidget);
	canvas.addWidget(panelWidget);
	canvas.addWidget(btnWidget);
	canvas.addWidget(scoreCanvas);
	canvas.setZIndex(0);
	Game::getGUI().getContainer().addWidget(canvas);

	generateRenderables(Game::getResultStats());

	/*Game::createPlayer({ 0, u8"Tiago", 0, false });
	Game::createPlayer({ 1, u8"Pedro", 0, false });
	Game::createPlayer({ 2, u8"Diana", 0, false });

	RoundStats stats;
	auto& pinfo = stats.playerInfo;
	pinfo.emplace_back(0, 0, 100);
	pinfo.emplace_back(1, 1, 95);
	pinfo.emplace_back(2, 2, 90);
	generateRenderables(&stats);*/
}

void ResultsScene::onUpdate()
{
	if (switchingScene)
	{
		if (Fader::hasFadingFinished())
		{
			LoadingScreen::close();
			destroy();
		}
		return;
	}
}

void ResultsScene::onRender()
{
	auto area = Vec2i(Game::getFramebufferSize());

	float windowFactor = (float(area.x * area.y) / BASE_WND_AREA);

	bgWidget.setBounds({0, 0, area.x, area.y});

	float panelSizeGrowthMul = 4.5f;
	float panelSizeMul = (windowFactor / panelSizeGrowthMul) + (1.0f - (1.0f / panelSizeGrowthMul));

	i32 panelWidth = std::lroundl(PANEL_IMG_SIZE.x * panelSizeMul);
	i32 panelHeight = std::lroundl(1.0f / PANEL_IMG_ARATIO * float(panelWidth));

	i32 panelX = (area.x - panelWidth) / 2;
	i32 panelY = std::lroundl(134.0f * panelSizeMul);

	panelWidget.setBounds({
		panelX,
		panelY,
		panelWidth,
		panelHeight
	});

	// TITLE

	i32 titleWidth = std::lroundl(TITLE_IMG_SIZE.x * panelSizeMul);
	i32 titleHeight = std::lroundl(1.0f / TITLE_IMG_ARATIO * float(titleWidth));

	i32 titleX = (area.x - titleWidth) / 2;
	i32 titleY = std::lroundl(43.0f * panelSizeMul);

	titleWidget.setBounds({
		titleX,
		titleY,
		titleWidth,
		titleHeight
	});

	// SCORE LISTING

	i32 msgSpacing = std::lroundl(24.0f * panelSizeMul);
	i32 msgYPos = panelY + std::lroundl(19.0f * panelSizeMul);

	for (auto& scoreEntry : scoreEntries)
	{
		mGUI::TextBox& textBox = scoreEntry.tb;
		textBox.setBounds({
			panelX,
			msgYPos,
			panelWidth,
			std::lroundl(24.0f * panelSizeMul),
		});
		msgYPos += msgSpacing;
	}

	// CONTINUE BUTTON

	i32 btnWidth = std::lroundl(BTN_IMG_SIZE.x * panelSizeMul);
	i32 btnHeight = std::lroundl(1.0f / BTN_IMG_ARATIO * float(btnWidth));

	i32 btnX = (area.x - btnWidth) / 2;
	i32 btnY = std::lroundl(413.0f * panelSizeMul);

	btnWidget.setBounds({
		btnX,
		btnY,
		btnWidth,
		btnHeight
	});
}

void ResultsScene::onDestroy()
{
	canvas.clear();
	Game::getGUI().getContainer().removeWidget(canvas);

	scoreCanvas.clear();
	scoreEntries.clear();

	bgTexture.destroy();
	titleTexture.destroy();
	panelTexture.destroy();
	btnNTexture.destroy();
	btnHTexture.destroy();

	Game::clearResultStats();
}

void ResultsScene::onDestroyRequest()
{
	switchingScene = true;
}

void ResultsScene::generateRenderables(const RoundStats* stats)
{
	auto& pi = stats->playerInfo;
	scoreEntries.resize(pi.size());
	for (SizeT i = 0; i < pi.size(); i++)
	{
		auto& pe = pi[i];
		auto& tb = scoreEntries[i].tb;
		Player* p = Game::getPlayerByID(pe.playerID);
		if (p)
		{
			static const char* placeSuffix[] = { "st", "nd", "rd", "th" };
			u32 place = pe.place;

			U8String newStr = Util::stringAsUtf8(std::to_string(place + 1) + placeSuffix[place > 3 ? 3 : place] + " - ");
			newStr.append(p->getName());
			newStr.append(Util::stringAsUtf8(" - " + std::to_string(pe.points) + " points"));
			tb.setText(newStr);
			tb.setFont("smooth");
			tb.setReadOnly(true);
			tb.setTextAlign(mGUI::TextBox::AlignCenter);
		}
		scoreCanvas.addWidget(tb);
	}
}

u32 ResultsScene::getPlayerPlace()
{
	u32 playerID = Game::getPlayerID();
	const RoundStats* resultStats = Game::getResultStats();
	auto& playerStatInfos = resultStats->playerInfo;

	u32 playerPlace = 0;
	for (auto info : playerStatInfos)
	{
		if (info.playerID == playerID)
		{
			playerPlace = info.place;
			break;
		}
	}
	return playerPlace;
}
