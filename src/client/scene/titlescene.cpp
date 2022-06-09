#include "titlescene.hpp"

#include "game.hpp"

constexpr u32 TICK_RATE = 30;
constexpr float TICK_DURATION = 1.0f / TICK_RATE;

constexpr float BASE_WND_AREA = 640.0f * 480.0f;

constexpr float TITLE_IMG_ARATIO = 512.0f / 256.0f;
constexpr float BUTTON_IMG_ARATIO = 216.0f / 53.0f;
constexpr float ICON_IMG_ARATIO = 64.0f / 64.0f;
constexpr float EXITDIAG_IMG_ARATIO = 302.0f / 121.0f;
constexpr float EXITDIAGBTN_IMG_ARATIO = 53.0f / 25.0f;

constexpr Vec2i NICKNDIAG_IMG_SIZE = { 302, 121 };
constexpr float NICKNDIAG_IMG_ARATIO = float(NICKNDIAG_IMG_SIZE.x) / float(NICKNDIAG_IMG_SIZE.y);
constexpr Vec2i NICKNDIAGTB_IMG_SIZE = { 275, 34 };
constexpr float NICKNDIAGTB_IMG_ARATIO = float(NICKNDIAGTB_IMG_SIZE.x) / float(NICKNDIAGTB_IMG_SIZE.y);
constexpr Vec2i NICKNDIAGBTN1_IMG_SIZE = { 53, 25 };
constexpr float NICKNDIAGBTN1_IMG_ARATIO = float(NICKNDIAGBTN1_IMG_SIZE.x) / float(NICKNDIAGBTN1_IMG_SIZE.y);
constexpr Vec2i NICKNDIAGBTN2_IMG_SIZE = { 62, 25 };
constexpr float NICKNDIAGBTN2_IMG_ARATIO = float(NICKNDIAGBTN2_IMG_SIZE.x) / float(NICKNDIAGBTN2_IMG_SIZE.y);

IMPL_OBJECT(TitleScene)

void TitleScene::onCreate()
{
	Game::setTickRate(TICK_RATE);

	titleTexture.load("@/title.png", GLE::TextureFilter::None);
	bgTexture.load("@/title/background.png");
	buttonTexture.load("@/title/button.png", GLE::TextureFilter::None);
	buttonHeldTexture.load("@/title/button_held.png", GLE::TextureFilter::None);
	buttonHoverTexture.load("@/title/button_hover.png", GLE::TextureFilter::None);
	createGameTextTexture.load("@/title/creategame.png");
	joinGameTextTexture.load("@/title/joingame.png");
	exitTexture.load("@/title/exit.png", GLE::TextureFilter::None);
	aboutTexture.load("@/title/about.png", GLE::TextureFilter::None);
	settingsTexture.load("@/title/settings.png", GLE::TextureFilter::None);
	edBgTexture.load("@/title/exit_dialog_bg.png", GLE::TextureFilter::None);
	edDtTexture.load("@/title/exit_dialog_text.png");
	edb1Texture.load("@/title/edb1.png", GLE::TextureFilter::None);
	edb2Texture.load("@/title/edb2.png", GLE::TextureFilter::None);
	edb3Texture.load("@/title/edb3.png", GLE::TextureFilter::None);
	nndBgTexture.load("@/title/name_diag_bg.png", GLE::TextureFilter::None);
	nndDtTexture.load("@/title/name_diag_dt.png", GLE::TextureFilter::None);
	nndTbTexture.load("@/title/nndtb.png", GLE::TextureFilter::None);
	nndb1Texture.load("@/title/nndb1.png", GLE::TextureFilter::None);
	nndb1cTexture.load("@/title/nndb1_c.png", GLE::TextureFilter::None);
	nndb1hTexture.load("@/title/nndb1_h.png", GLE::TextureFilter::None);
	nndb2Texture.load("@/title/nndb2.png", GLE::TextureFilter::None);
	nndb2cTexture.load("@/title/nndb2_c.png", GLE::TextureFilter::None);
	nndb2hTexture.load("@/title/nndb2_h.png", GLE::TextureFilter::None);

	titleWidget.setTexture(&titleTexture);
	bgWidget.setTexture(&bgTexture);
	createGameButtonWidget.setTexture(&buttonTexture);
	createGameButtonWidget.setHeldTexture(&buttonHeldTexture);
	createGameButtonWidget.setHoverTexture(&buttonHoverTexture);
	joinGameButtonWidget.setTexture(&buttonTexture);
	joinGameButtonWidget.setHeldTexture(&buttonHeldTexture);
	joinGameButtonWidget.setHoverTexture(&buttonHoverTexture);
	createGameTextWidget.setTexture(&createGameTextTexture);
	joinGameTextWidget.setTexture(&joinGameTextTexture);
	exitWidget.setTexture(&exitTexture);
	aboutWidget.setTexture(&aboutTexture);
	settingsWidget.setTexture(&settingsTexture);
	edBgWidget.setTexture(&edBgTexture);
	edDtWidget.setTexture(&edDtTexture);
	edbYWidget.setTexture(&edb1Texture);
	edbYWidget.setHoverTexture(&edb2Texture);
	edbYWidget.setHeldTexture(&edb3Texture);
	edbNWidget.setTexture(&edb1Texture);
	edbNWidget.setHoverTexture(&edb2Texture);
	edbNWidget.setHeldTexture(&edb3Texture);
	nndBgWidget.setTexture(&nndBgTexture);
	nndDtWidget.setTexture(&nndDtTexture);
	nndTbWidget.setTexture(&nndTbTexture);
	nndbYWidget.setTexture(&nndb1Texture);
	nndbYWidget.setHoverTexture(&nndb1hTexture);
	nndbYWidget.setHeldTexture(&nndb1cTexture);
	nndbNWidget.setTexture(&nndb2Texture);
	nndbNWidget.setHoverTexture(&nndb2hTexture);
	nndbNWidget.setHeldTexture(&nndb2cTexture);

	bgWidget.setZIndex(0);
	createGameTextWidget.setZIndex(1010);
	joinGameTextWidget.setZIndex(1010);

	edBgWidget.setZIndex(1020);
	edDtWidget.setZIndex(1022);
	edbYWidget.setZIndex(1021);
	edbNWidget.setZIndex(1021);

	nndBgWidget.setZIndex(1020);
	nndDtWidget.setZIndex(1021);
	nndTbWidget.setZIndex(1022);
	nndbYWidget.setZIndex(1022);
	nndbNWidget.setZIndex(1022);

	nndTbWidget.setFontScale(0.70f);
	nndTbWidget.setTextSidePadding(768.0f);
	nndTbWidget.setMaxCharacters(16);

	exitWidget.setOnClick([this](){ showExitDialog(); });
	edbYWidget.setOnClick([](){ Game::quit(); });
	edbNWidget.setOnClick([this](){ closeExitDialog(); });

	joinGameButtonWidget.setOnClick([this](){
		showNicknameDialog();
	});

	nndbNWidget.setOnClick([this](){ closeNicknameDialog(); });

	canvas.addWidget(titleWidget);
	canvas.addWidget(bgWidget);
	canvas.addWidget(createGameButtonWidget);
	canvas.addWidget(joinGameButtonWidget);
	canvas.addWidget(createGameTextWidget);
	canvas.addWidget(joinGameTextWidget);
	canvas.addWidget(exitWidget);
	canvas.addWidget(aboutWidget);
	canvas.addWidget(settingsWidget);
	canvas.setZIndex(0);

	exitDialogCanvas.addWidget(edBgWidget);
	exitDialogCanvas.addWidget(edDtWidget);
	exitDialogCanvas.addWidget(edbYWidget);
	exitDialogCanvas.addWidget(edbNWidget);
	exitDialogCanvas.setZIndex(1051);
	exitDialogCanvas.setVisible(false);
	canvas.addWidget(exitDialogCanvas);

	nicknameDialogCanvas.addWidget(nndBgWidget);
	nicknameDialogCanvas.addWidget(nndDtWidget);
	nicknameDialogCanvas.addWidget(nndTbWidget);
	nicknameDialogCanvas.addWidget(nndbYWidget);
	nicknameDialogCanvas.addWidget(nndbNWidget);
	nicknameDialogCanvas.setZIndex(1051);
	nicknameDialogCanvas.setVisible(false);
	canvas.addWidget(nicknameDialogCanvas);

	Game::getGUI().getContainer().addWidget(canvas);
}

void TitleScene::onUpdate()
{
	if (exitDialogOpen || nicknameDialogOpen)
	{
		lastDialogAnimTimer = dialogAnimTimer;
		if (dialogAnimTimer < 1.0f)
		{
			dialogAnimTimer += 3.0f * TICK_DURATION;
			if (dialogAnimTimer > 1.0f)
				dialogAnimTimer = 1.0f;
		}
	}
}

void TitleScene::onRender()
{
	auto area = Vec2i(Game::getFramebufferSize());

	bgWidget.setBounds({0, 0, area.x, area.y});

	float windowFactor = (float(area.x * area.y) / BASE_WND_AREA);

	float titleSizeGrowthMul = 4.5f;
	float titleSizeMul = (windowFactor / titleSizeGrowthMul) + (1.0f - (1.0f / titleSizeGrowthMul));

	i32 titleSize = std::lroundl(512.0f * titleSizeMul);
	i32 titleWidth = titleSize;
	i32 titleHeight = std::lroundl(1.0f / TITLE_IMG_ARATIO * float(titleSize));

	titleWidget.setBounds({(area.x - titleWidth) / 2, 0, titleWidth, titleHeight});

	float buttonSizeGrowthMul = 4.5f;
	float buttonSizeMul = (windowFactor / buttonSizeGrowthMul) + (1.0f - (1.0f / buttonSizeGrowthMul));

	i32 buttonSize = std::lroundl(256.0f * buttonSizeMul);
	i32 buttonWidth = buttonSize;
	i32 buttonHeight = std::lroundl(1.0f / BUTTON_IMG_ARATIO * float(buttonSize));

	i32 buttonSpacing = std::lroundl(16.0f * buttonSizeMul);

	RectI createGameButtonBounds = {
		(area.x - buttonWidth) / 2,
		titleHeight,
		buttonWidth,
		buttonHeight
	};
	createGameButtonWidget.setBounds(createGameButtonBounds);
	createGameTextWidget.setBounds(createGameButtonBounds);

	RectI joinGameButtonBounds = {
		(area.x - buttonWidth) / 2,
		titleHeight + buttonHeight + buttonSpacing,
		buttonWidth,
		buttonHeight
	};
	joinGameButtonWidget.setBounds(joinGameButtonBounds);
	joinGameTextWidget.setBounds(joinGameButtonBounds);

	float buttonSizeGrowthMul2 = 3.0f;
	float buttonSizeMul2 = (windowFactor / buttonSizeGrowthMul2) + (1.0f - (1.0f / buttonSizeGrowthMul2));

	i32 buttonSize2 = std::lroundl(64.0f * buttonSizeMul2);
	i32 buttonWidth2 = buttonSize2;
	i32 buttonHeight2 = std::lroundl(1.0f / ICON_IMG_ARATIO * float(buttonSize2));

	i32 buttonXSpacing = std::lroundl(16 * buttonSizeMul2);
	i32 buttonY2 = area.y - buttonSize2 - std::lroundl(12.0f * buttonSizeMul2);

	aboutWidget.setBounds({buttonXSpacing, buttonY2, buttonWidth2, buttonHeight2});

	settingsWidget.setBounds({area.x - buttonWidth2 - buttonXSpacing, buttonY2, buttonWidth2, buttonHeight2});

	exitWidget.setBounds({area.x - (buttonWidth2 * 2) - (buttonXSpacing * 2), buttonY2, buttonWidth2, buttonHeight2});

	if (exitDialogOpen)
		renderExitDialog(area, windowFactor);

	if (nicknameDialogOpen)
		renderNicknameDialog(area, windowFactor);
}

void TitleScene::onDestroy()
{
	canvas.clear();
	Game::getGUI().getContainer().removeWidget(canvas);

	titleTexture.destroy();
	bgTexture.destroy();
	buttonTexture.destroy();
	buttonHeldTexture.destroy();
	buttonHoverTexture.destroy();
	createGameTextTexture.destroy();
	joinGameTextTexture.destroy();
	exitTexture.destroy();
	aboutTexture.destroy();
	settingsTexture.destroy();
	edBgTexture.destroy();
	edDtTexture.destroy();
	edb1Texture.destroy();
	edb2Texture.destroy();
	edb3Texture.destroy();
	nndBgTexture.destroy();
	nndDtTexture.destroy();
	nndTbTexture.destroy();
	nndb1Texture.destroy();
	nndb1cTexture.destroy();
	nndb1hTexture.destroy();
	nndb2Texture.destroy();
	nndb2cTexture.destroy();
	nndb2hTexture.destroy();
}

void TitleScene::onDestroyRequest()
{
	destroy();
}

void TitleScene::setCommonWidgetsEnabled(bool enabled)
{
	createGameButtonWidget.setEnabled(enabled);
	joinGameButtonWidget.setEnabled(enabled);
	exitWidget.setEnabled(enabled);
}

void TitleScene::renderExitDialog(const Vec2i& area, float windowFactor)
{
	float diagSizeGrowthMul = 4.5f;
	float diagSizeMul = (windowFactor / diagSizeGrowthMul) + (1.0f - (1.0f / diagSizeGrowthMul));

	diagSizeMul *= Math::lerp(lastDialogAnimTimer, dialogAnimTimer, Game::getTickAlpha());

	i32 diagSize = std::lroundl(512.0f * diagSizeMul);
	i32 diagWidth = diagSize;
	i32 diagHeight = std::lroundl(1.0f / EXITDIAG_IMG_ARATIO * float(diagSize));

	float diagScaleDiffPix = (float(diagSize) / 302.0f);

	i32 diagX = (area.x - diagWidth) / 2;
	i32 diagY = (area.y - diagHeight) / 2;

	edBgWidget.setBounds({diagX, diagY, diagWidth, diagHeight});
	edDtWidget.setBounds(edBgWidget.getBounds());

	i32 btnSize = std::lroundl((53.0f * 512.0f) / 302.0f * diagSizeMul);
	i32 btnWidth = btnSize;
	i32 btnHeight = std::lroundl(1.0f / EXITDIAGBTN_IMG_ARATIO * float(btnSize));

	edbYWidget.setBounds({
		std::lroundl(158.0f * diagScaleDiffPix) + diagX,
		std::lroundl(78.0f * diagScaleDiffPix) + diagY,
		btnWidth,
		btnHeight
	});

	edbNWidget.setBounds({
		std::lroundl(226.0f * diagScaleDiffPix) + diagX,
		std::lroundl(78.0f * diagScaleDiffPix) + diagY,
		btnWidth,
		btnHeight
	});
}

void TitleScene::showExitDialog()
{
	if (exitDialogOpen)
		return;

	dialogAnimTimer = 0.0f;
	lastDialogAnimTimer = 0.0f;
	exitDialogOpen = true;

	auto area = Vec2i(Game::getFramebufferSize());
	float windowFactor = (float(area.x * area.y) / BASE_WND_AREA);
	renderExitDialog(area, windowFactor);

	setCommonWidgetsEnabled(false);

	exitDialogCanvas.setVisible(true);
}

void TitleScene::closeExitDialog()
{
	exitDialogCanvas.setVisible(false);

	setCommonWidgetsEnabled(true);

	exitDialogOpen = false;
}

void TitleScene::renderNicknameDialog(const Vec2i& area, float windowFactor)
{
	float diagSizeGrowthMul = 4.5f;
	float diagSizeMul = (windowFactor / diagSizeGrowthMul) + (1.0f - (1.0f / diagSizeGrowthMul));

	diagSizeMul *= Math::lerp(lastDialogAnimTimer, dialogAnimTimer, Game::getTickAlpha());

	i32 diagSize = std::lroundl(512.0f * diagSizeMul);
	i32 diagWidth = diagSize;
	i32 diagHeight = std::lroundl(1.0f / NICKNDIAG_IMG_ARATIO * float(diagSize));

	float diagScaleDiffPix = (float(diagSize) / float(NICKNDIAG_IMG_SIZE.x));

	i32 diagX = (area.x - diagWidth) / 2;
	i32 diagY = (area.y - diagHeight) / 2;

	nndBgWidget.setBounds({diagX, diagY, diagWidth, diagHeight});
	nndDtWidget.setBounds(nndBgWidget.getBounds());

	i32 textboxSize = std::lroundl((float(NICKNDIAGTB_IMG_SIZE.x) * 512.0f) / float(NICKNDIAG_IMG_SIZE.x) * diagSizeMul);
	i32 textboxWidth = textboxSize;
	i32 textboxHeight = std::lroundl(1.0f / NICKNDIAGTB_IMG_ARATIO * float(textboxSize));

	nndTbWidget.setBounds({
		std::lroundl(13.0f * diagScaleDiffPix) + diagX,
		std::lroundl(44.0f * diagScaleDiffPix) + diagY,
		textboxWidth,
		textboxHeight
	});

	i32 btn1Size = std::lroundl((float(NICKNDIAGBTN1_IMG_SIZE.x) * 512.0f) / float(NICKNDIAG_IMG_SIZE.x) * diagSizeMul);
	i32 btn1Width = btn1Size;
	i32 btn1Height = std::lroundl(1.0f / NICKNDIAGBTN1_IMG_ARATIO * float(btn1Size));

	nndbYWidget.setBounds({
		std::lroundl(165.0f * diagScaleDiffPix) + diagX,
		std::lroundl(86.0f * diagScaleDiffPix) + diagY,
		btn1Width,
		btn1Height
	});

	i32 btn2Size = std::lroundl((float(NICKNDIAGBTN2_IMG_SIZE.x) * 512.0f) / float(NICKNDIAG_IMG_SIZE.x) * diagSizeMul);
	i32 btn2Width = btn2Size;
	i32 btn2Height = std::lroundl(1.0f / NICKNDIAGBTN2_IMG_ARATIO * float(btn2Size));

	nndbNWidget.setBounds({
		std::lroundl(226.0f * diagScaleDiffPix) + diagX,
		std::lroundl(86.0f * diagScaleDiffPix) + diagY,
		btn2Width,
		btn2Height
	});
}

void TitleScene::showNicknameDialog()
{
	if (nicknameDialogOpen)
		return;

	dialogAnimTimer = 0.0f;
	lastDialogAnimTimer = 0.0f;
	nicknameDialogOpen = true;

	nndTbWidget.setText(u8"");

	auto area = Vec2i(Game::getFramebufferSize());
	float windowFactor = (float(area.x * area.y) / BASE_WND_AREA);
	renderNicknameDialog(area, windowFactor);

	setCommonWidgetsEnabled(false);

	nicknameDialogCanvas.setVisible(true);
}

void TitleScene::closeNicknameDialog()
{
	nicknameDialogCanvas.setVisible(false);

	setCommonWidgetsEnabled(true);

	nicknameDialogOpen = false;
}
