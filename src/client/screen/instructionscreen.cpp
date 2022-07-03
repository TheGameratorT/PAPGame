#include "instructionscreen.hpp"

#include "game.hpp"
#include "render/texture.hpp"
#include "gui/container.hpp"
#include "gui/image.hpp"
#include "gui/button.hpp"

constexpr float BASE_WND_AREA = 640.0f * 480.0f;

constexpr Vec2i DIAG_IMG_SIZE = { 399, 270 };
constexpr float DIAG_IMG_ARATIO = float(DIAG_IMG_SIZE.x) / float(DIAG_IMG_SIZE.y);
constexpr Vec2i DIAGBTN_IMG_SIZE = { 104, 26 };
constexpr float DIAGBTN_IMG_ARATIO = float(DIAGBTN_IMG_SIZE.x) / float(DIAGBTN_IMG_SIZE.y);

namespace InstructionScreen {

static Texture diagBgTexture;
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
	diagBgTexture.load("@/instruct/panel.png", GLE::TextureFilter::None);
	diagBtnNTexture.load("@/instruct/btnN.png", GLE::TextureFilter::None);
	diagBtnHTexture.load("@/instruct/btnH.png", GLE::TextureFilter::None);
	diagBtnCTexture.load("@/instruct/btnC.png", GLE::TextureFilter::None);

	diagBgWidget.setTexture(&diagBgTexture);
	diagBtnWidget.setTexture(&diagBtnNTexture);
	diagBtnWidget.setHoverTexture(&diagBtnHTexture);
	diagBtnWidget.setHeldTexture(&diagBtnCTexture);

	diagBtnWidget.setOnClick([](){
		Game::onInstructed();
		InstructionScreen::close();
	});

	container.addWidget(diagBgWidget);
	container.addWidget(diagTxtWidget);
	container.addWidget(diagBtnWidget);
	container.setZIndex(2049); // below connection lost screen
	Game::getGUI().getContainer().addWidget(container);
}

void destroy()
{
	container.clear();
	Game::getGUI().getContainer().removeWidget(container);

	diagBgTexture.destroy();
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

	i32 diagSize = std::lroundl(DIAG_IMG_SIZE.x * diagSizeMul);
	i32 diagWidth = diagSize;
	i32 diagHeight = std::lroundl(1.0f / DIAG_IMG_ARATIO * float(diagSize));

	i32 diagX = (area.x - diagWidth) / 2;
	i32 diagY = (area.y - diagHeight) / 2;

	diagBgWidget.setBounds({diagX, diagY, diagWidth, diagHeight});
	diagTxtWidget.setBounds(diagBgWidget.getBounds());

	i32 btnSize = std::lroundl(DIAGBTN_IMG_SIZE.x * diagSizeMul);
	i32 btnWidth = btnSize;
	i32 btnHeight = std::lroundl(1.0f / DIAGBTN_IMG_ARATIO * float(btnSize));

	diagBtnWidget.setBounds({
		std::lroundl(148.0f * diagSizeMul) + diagX,
		std::lroundl(222.0f * diagSizeMul) + diagY,
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

void setInstructionTexture(Texture* texture)
{
	diagTxtWidget.setTexture(texture);
}

}
