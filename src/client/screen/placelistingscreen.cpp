#include "placelistingscreen.hpp"

#include "game.hpp"
#include "player.hpp"
#include "roundstats.hpp"
#include "render/texture.hpp"
#include "gui/container.hpp"
#include "gui/image.hpp"
#include "gui/button.hpp"
#include "gui/textbox.hpp"
#include "util.hpp"

constexpr float BASE_WND_AREA = 640.0f * 480.0f;

constexpr Vec2i DIAG_IMG_SIZE = { 307, 300 };
constexpr float DIAG_IMG_ARATIO = float(DIAG_IMG_SIZE.x) / float(DIAG_IMG_SIZE.y);
constexpr Vec2i DIAGTXT_IMG_SIZE = { 1228, 176 };
constexpr float DIAGTXT_IMG_ARATIO = float(DIAGTXT_IMG_SIZE.x) / float(DIAGTXT_IMG_SIZE.y);
constexpr Vec2i DIAGBTN_IMG_SIZE = { 179, 26 };
constexpr float DIAGBTN_IMG_ARATIO = float(DIAGBTN_IMG_SIZE.x) / float(DIAGBTN_IMG_SIZE.y);

namespace PlaceListingScreen {

static Texture diagBgTexture;
static Texture diagTitle1Texture;
static Texture diagTitle2Texture;
static Texture diagBtnNTexture;
static Texture diagBtnHTexture;
static Texture diagBtnCTexture;
static Texture diagBtnTxt1Texture;
static Texture diagBtnTxt2Texture;

static float dialogAnimTimer = 0.0f;
static float lastDialogAnimTimer = 0.0f;

static mGUI::Container container;
static mGUI::Container scoreContainer;
static mGUI::Image diagBgWidget;
static mGUI::Image diagTxtWidget;
static mGUI::Button diagBtnWidget;
static mGUI::Image diagBtnTxtWidget;

static const RoundStats* sFinalStats;

static bool isVisible = false;
static bool nextIsLastScreen = false;

struct PlayerScoreEntry
{
	mGUI::TextBox tb;
};

static std::vector<PlayerScoreEntry> scoreEntries;

static void switchToLastScreen();
static void generateRenderables(const RoundStats* stats);
static void clearRenderables();

void init()
{
	diagBgTexture.load("@/listing/panel.png", GLE::TextureFilter::None);
	diagTitle1Texture.load("@/listing/round_finished.png");
	diagTitle2Texture.load("@/listing/game_finished.png");
	diagBtnNTexture.load("@/listing/btnN.png", GLE::TextureFilter::None);
	diagBtnHTexture.load("@/listing/btnH.png", GLE::TextureFilter::None);
	diagBtnCTexture.load("@/listing/btnC.png", GLE::TextureFilter::None);
	diagBtnTxt1Texture.load("@/listing/ready1.png");
	diagBtnTxt2Texture.load("@/listing/ready2.png");

	diagBgWidget.setTexture(&diagBgTexture);
	diagBtnWidget.setTexture(&diagBtnNTexture);
	diagBtnWidget.setHoverTexture(&diagBtnHTexture);
	diagBtnWidget.setHeldTexture(&diagBtnCTexture);

	diagBtnWidget.setOnClick([](){
		if (nextIsLastScreen)
		{
			nextIsLastScreen = false;
			switchToLastScreen();
			return;
		}
		Game::onPlaceListingClosed();
		PlaceListingScreen::close();
	});

	scoreContainer.setZIndex(1050);

	container.addWidget(diagBgWidget);
	container.addWidget(diagTxtWidget);
	container.addWidget(diagBtnWidget);
	container.addWidget(diagBtnTxtWidget);
	container.addWidget(scoreContainer);
	container.setZIndex(2049); // below connection lost screen
	Game::getGUI().getContainer().addWidget(container);
}

void destroy()
{
	container.clear();
	Game::getGUI().getContainer().removeWidget(container);

	diagBgTexture.destroy();
	diagTitle1Texture.destroy();
	diagTitle2Texture.destroy();
	diagBtnNTexture.destroy();
	diagBtnHTexture.destroy();
	diagBtnCTexture.destroy();
	diagBtnTxt1Texture.destroy();
	diagBtnTxt2Texture.destroy();
}

void update()
{
	if (!isVisible)
		return;

	lastDialogAnimTimer = dialogAnimTimer;
	if (dialogAnimTimer < 1.0f)
	{
		dialogAnimTimer += 3.0f * Game::getTickDuration();
		if (dialogAnimTimer > 1.0f)
			dialogAnimTimer = 1.0f;
	}
}

void render()
{
	if (!isVisible)
		return;

	auto area = Vec2i(Game::getFramebufferSize());
	float windowFactor = (float(area.x * area.y) / BASE_WND_AREA);

	float diagSizeGrowthMul = 4.5f;
	float diagSizeMul = (windowFactor / diagSizeGrowthMul) + (1.0f - (1.0f / diagSizeGrowthMul));

	diagSizeMul *= Math::lerp(lastDialogAnimTimer, dialogAnimTimer, Game::getTickAlpha());

	i32 diagSize = std::lroundl(DIAG_IMG_SIZE.x * diagSizeMul);
	i32 diagWidth = diagSize;
	i32 diagHeight = std::lroundl(1.0f / DIAG_IMG_ARATIO * float(diagSize));

	i32 diagX = (area.x - diagWidth) / 2;
	i32 diagY = (area.y - diagHeight) / 2;

	diagBgWidget.setBounds({diagX, diagY, diagWidth, diagHeight});

	i32 diagTxtWidth = std::lroundl(DIAG_IMG_SIZE.x * diagSizeMul);
	i32 diagTxtHeight = std::lroundl(1.0f / DIAGTXT_IMG_ARATIO * float(diagTxtWidth));
	diagTxtWidget.setBounds({diagX, diagY, diagTxtWidth, diagTxtHeight});

	i32 btnSize = std::lroundl(DIAGBTN_IMG_SIZE.x * diagSizeMul);
	i32 btnWidth = btnSize;
	i32 btnHeight = std::lroundl(1.0f / DIAGBTN_IMG_ARATIO * float(btnSize));

	diagBtnWidget.setBounds({
		std::lroundl(66.0f * diagSizeMul) + diagX,
		std::lroundl(256.0f * diagSizeMul) + diagY,
		btnWidth,
		btnHeight
	});
	diagBtnTxtWidget.setBounds(diagBtnWidget.getBounds());

	// SCORE LISTING

	i32 msgSpacing = std::lroundl(16.0f * diagSizeMul);
	i32 msgYPos = diagY + std::lroundl(65.0f * diagSizeMul);

	for (auto& scoreEntry : scoreEntries)
	{
		mGUI::TextBox& textBox = scoreEntry.tb;
		textBox.setBounds({
			diagX + std::lroundl(19.0f * diagSizeMul),
			msgYPos,
			std::lroundl(267.0f * diagSizeMul),
			std::lroundl(16.0f * diagSizeMul),
		});
		msgYPos += msgSpacing;
	}
}

void show(bool lastRound, const RoundStats* stats, const RoundStats* finalStats)
{
	if (isVisible)
		return;

	nextIsLastScreen = lastRound;
	sFinalStats = finalStats ? new RoundStats(*finalStats) : nullptr;

	dialogAnimTimer = 0.0f;
	lastDialogAnimTimer = 0.0f;

	diagTxtWidget.setTexture(&diagTitle1Texture);
	diagBtnTxtWidget.setTexture(&diagBtnTxt1Texture);
	generateRenderables(stats);

	container.setVisible(true);
	isVisible = true;
}

void close()
{
	if (!isVisible)
		return;

	clearRenderables();
	delete sFinalStats;
	sFinalStats = nullptr;

	container.setVisible(false);
	isVisible = false;
}

static void switchToLastScreen()
{
	diagTxtWidget.setTexture(&diagTitle2Texture);
	diagBtnTxtWidget.setTexture(&diagBtnTxt2Texture);
	generateRenderables(sFinalStats);
	delete sFinalStats;
	sFinalStats = nullptr;
}

static void generateRenderables(const RoundStats* stats)
{
	clearRenderables();

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

			U8String newStr = p->getName();
			newStr.append(Util::stringAsUtf8(" - " + std::to_string(place + 1) + placeSuffix[place > 3 ? 3 : place] + " place"));
			tb.setText(newStr);
			tb.setFont("smooth");
			tb.setReadOnly(true);
		}
		scoreContainer.addWidget(tb);
	}
}

static void clearRenderables()
{
	scoreContainer.clear();
	scoreEntries.clear();
}

}
