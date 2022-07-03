#include "connectionlostscreen.hpp"

#include "game.hpp"
#include "render/texture.hpp"
#include "gui/container.hpp"
#include "gui/image.hpp"
#include "gui/button.hpp"

constexpr float BASE_WND_AREA = 640.0f * 480.0f;

constexpr Vec2i CONN_IMG_SIZE = { 302, 121 };
constexpr float CONN_IMG_ARATIO = float(CONN_IMG_SIZE.x) / float(CONN_IMG_SIZE.y);
constexpr Vec2i CONNBTN_IMG_SIZE = { 140, 25 };
constexpr float CONNBTN_IMG_ARATIO = float(CONNBTN_IMG_SIZE.x) / float(CONNBTN_IMG_SIZE.y);

namespace ConnectionLostScreen {

static Texture diagBgTexture;
static Texture diagTxtTexture;
static Texture diagBtnNTexture;
static Texture diagBtnHTexture;
static Texture diagBtnCTexture;

static float dialogAnimTimer = 0.0f;
static float lastDialogAnimTimer = 0.0f;

static mGUI::Container container;
static mGUI::Image diagBgWidget;
static mGUI::Image diagTxtWidget;
static mGUI::Button diagBtnWidget;

static bool isVisible = false;

void init()
{
	diagBgTexture.load("@/conn_lost.png", GLE::TextureFilter::None);
	diagTxtTexture.load("@/conn_lost_txt.png");
	diagBtnNTexture.load("@/conn_lost_btnN.png", GLE::TextureFilter::None);
	diagBtnHTexture.load("@/conn_lost_btnH.png", GLE::TextureFilter::None);
	diagBtnCTexture.load("@/conn_lost_btnC.png", GLE::TextureFilter::None);

	diagBgWidget.setTexture(&diagBgTexture);
	diagTxtWidget.setTexture(&diagTxtTexture);
	diagBtnWidget.setTexture(&diagBtnNTexture);
	diagBtnWidget.setHoverTexture(&diagBtnHTexture);
	diagBtnWidget.setHeldTexture(&diagBtnCTexture);

	diagBtnWidget.setOnClick([](){ Game::reload(); });

	container.addWidget(diagBgWidget);
	container.addWidget(diagTxtWidget);
	container.addWidget(diagBtnWidget);
	container.setZIndex(2050); // on top of loading screen
	Game::getGUI().getContainer().addWidget(container);
}

void destroy()
{
	container.clear();
	Game::getGUI().getContainer().removeWidget(container);

	diagBgTexture.destroy();
	diagTxtTexture.destroy();
	diagBtnNTexture.destroy();
	diagBtnHTexture.destroy();
	diagBtnCTexture.destroy();
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

	i32 diagSize = std::lroundl(512.0f * diagSizeMul);
	i32 diagWidth = diagSize;
	i32 diagHeight = std::lroundl(1.0f / CONN_IMG_ARATIO * float(diagSize));

	float diagScaleDiffPix = (float(diagSize) / float(CONN_IMG_SIZE.x));

	i32 diagX = (area.x - diagWidth) / 2;
	i32 diagY = (area.y - diagHeight) / 2;

	diagBgWidget.setBounds({diagX, diagY, diagWidth, diagHeight});
	diagTxtWidget.setBounds(diagBgWidget.getBounds());

	i32 btnSize = std::lroundl((float(CONNBTN_IMG_SIZE.x) * 512.0f) / float(CONN_IMG_SIZE.x) * diagSizeMul);
	i32 btnWidth = btnSize;
	i32 btnHeight = std::lroundl(1.0f / CONNBTN_IMG_ARATIO * float(btnSize));

	diagBtnWidget.setBounds({
		std::lroundl(136.0f * diagScaleDiffPix) + diagX,
		std::lroundl(77.0f * diagScaleDiffPix) + diagY,
		btnWidth,
		btnHeight
	});
}

void show()
{
	if (isVisible)
		return;

	dialogAnimTimer = 0.0f;
	lastDialogAnimTimer = 0.0f;

	container.setVisible(true);
	isVisible = true;
}

void close()
{
	if (!isVisible)
		return;

	container.setVisible(false);
	isVisible = false;
}

}
