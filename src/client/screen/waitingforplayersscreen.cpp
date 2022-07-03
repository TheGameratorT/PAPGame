#include "instructionscreen.hpp"

#include "game.hpp"
#include "render/texture.hpp"
#include "gui/container.hpp"
#include "gui/image.hpp"

constexpr float BASE_WND_AREA = 640.0f * 480.0f;

constexpr Vec2i DIAG_IMG_SIZE = { 399, 92 };
constexpr float DIAG_IMG_ARATIO = float(DIAG_IMG_SIZE.x) / float(DIAG_IMG_SIZE.y);

namespace WaitingForPlayersScreen {

static Texture diagBgTexture;
static Texture diagTdTexture;

static float dialogAnimTimer = 0.0f;
static float lastDialogAnimTimer = 0.0f;

static mGUI::Container container;
static mGUI::Image diagBgWidget;
static mGUI::Image diagTxtWidget;

static bool isVisible = false;

void init()
{
	diagBgTexture.load("@/wfp_panel.png", GLE::TextureFilter::None);
	diagTdTexture.load("@/wfp_text.png");

	diagBgWidget.setTexture(&diagBgTexture);
	diagTxtWidget.setTexture(&diagTdTexture);

	container.addWidget(diagBgWidget);
	container.addWidget(diagTxtWidget);
	container.setZIndex(2049); // below connection lost screen
	Game::getGUI().getContainer().addWidget(container);
}

void destroy()
{
	container.clear();
	Game::getGUI().getContainer().removeWidget(container);

	diagBgTexture.destroy();
	diagTdTexture.destroy();
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
	diagTxtWidget.setBounds(diagBgWidget.getBounds());
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
