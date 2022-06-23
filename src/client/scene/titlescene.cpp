#include "titlescene.hpp"

#include "game.hpp"
#include "render/fader.hpp"

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

constexpr float CONNDIAG_IMG_ARATIO = 302.0f / 121.0f;

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
	exitTexture.load("@/exit.png", GLE::TextureFilter::None);
	aboutTexture.load("@/title/about.png", GLE::TextureFilter::None);
	settingsTexture.load("@/settings.png", GLE::TextureFilter::None);
	edBgTexture.load("@/title/exit_dialog_bg.png", GLE::TextureFilter::None);
	edDtTexture.load("@/title/exit_dialog_text.png");
	edb1Texture.load("@/title/edb1.png", GLE::TextureFilter::None);
	edb2Texture.load("@/title/edb2.png", GLE::TextureFilter::None);
	edb3Texture.load("@/title/edb3.png", GLE::TextureFilter::None);
	txtBgTexture.load("@/title/txt_diag_bg.png", GLE::TextureFilter::None);
	nndDtTexture.load("@/title/name_diag_dt.png", GLE::TextureFilter::None);
	ipdDtTexture.load("@/title/ip_diag_dt.png", GLE::TextureFilter::None);
	txtTbTexture.load("@/title/txttb.png", GLE::TextureFilter::None);
	txtb1Texture.load("@/title/txtb1.png", GLE::TextureFilter::None);
	txtb1cTexture.load("@/title/txtb1_c.png", GLE::TextureFilter::None);
	txtb1hTexture.load("@/title/txtb1_h.png", GLE::TextureFilter::None);
	txtb2Texture.load("@/title/txtb2.png", GLE::TextureFilter::None);
	txtb2cTexture.load("@/title/txtb2_c.png", GLE::TextureFilter::None);
	txtb2hTexture.load("@/title/txtb2_h.png", GLE::TextureFilter::None);
	connBgTexture.load("@/title/conn_diag.png", GLE::TextureFilter::None);
	connDt1Texture.load("@/title/conn_diag_1.png", GLE::TextureFilter::None);
	connDt2Texture.load("@/title/conn_diag_2.png", GLE::TextureFilter::None);

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
	txtBgWidget.setTexture(&txtBgTexture);
	txtTbWidget.setTexture(&txtTbTexture);
	txtbYWidget.setTexture(&txtb1Texture);
	txtbYWidget.setHoverTexture(&txtb1hTexture);
	txtbYWidget.setHeldTexture(&txtb1cTexture);
	txtbNWidget.setTexture(&txtb2Texture);
	txtbNWidget.setHoverTexture(&txtb2hTexture);
	txtbNWidget.setHeldTexture(&txtb2cTexture);
	connBgWidget.setTexture(&connBgTexture);

	bgWidget.setZIndex(0);
	createGameTextWidget.setZIndex(1010);
	joinGameTextWidget.setZIndex(1010);

	edBgWidget.setZIndex(1020);
	edDtWidget.setZIndex(1022);
	edbYWidget.setZIndex(1021);
	edbNWidget.setZIndex(1021);

	txtBgWidget.setZIndex(1020);
	txtDtWidget.setZIndex(1021);
	txtTbWidget.setZIndex(1022);
	txtbYWidget.setZIndex(1022);
	txtbNWidget.setZIndex(1022);

	connBgWidget.setZIndex(1020);
	connDtWidget.setZIndex(1021);

	exitWidget.setOnClick([this](){ showExitDialog(); });
	edbYWidget.setOnClick([](){ Game::quit(); });
	edbNWidget.setOnClick([this](){ closeExitDialog(); });

	createGameButtonWidget.setOnClick([this](){
		isCreating = true;
		showNicknameDialog();
	});
	joinGameButtonWidget.setOnClick([this](){
		isCreating = false;
		showNicknameDialog();
	});

	txtbNWidget.setOnClick([this](){ closeTextDialog(); });

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

	textDialogCanvas.addWidget(txtBgWidget);
	textDialogCanvas.addWidget(txtDtWidget);
	textDialogCanvas.addWidget(txtTbWidget);
	textDialogCanvas.addWidget(txtbYWidget);
	textDialogCanvas.addWidget(txtbNWidget);
	textDialogCanvas.setZIndex(1051);
	textDialogCanvas.setVisible(false);
	canvas.addWidget(textDialogCanvas);

	connDialogCanvas.addWidget(connBgWidget);
	connDialogCanvas.addWidget(connDtWidget);
	connDialogCanvas.setZIndex(1051);
	connDialogCanvas.setVisible(false);
	canvas.addWidget(connDialogCanvas);

	Game::getGUI().getContainer().addWidget(canvas);
}

void TitleScene::onUpdate()
{
	if (switchingScene)
	{
		if (Fader::hasFadingFinished())
		{
			destroy();
			return;
		}
	}

	if (isDialogOpen())
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
	if (textDialogOpen)
		renderTextDialog(area, windowFactor);
	if (connDialogOpen)
		renderConnDialog(area, windowFactor);
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
	txtBgTexture.destroy();
	nndDtTexture.destroy();
	ipdDtTexture.destroy();
	txtTbTexture.destroy();
	txtb1Texture.destroy();
	txtb1cTexture.destroy();
	txtb1hTexture.destroy();
	txtb2Texture.destroy();
	txtb2cTexture.destroy();
	txtb2hTexture.destroy();
	connBgTexture.destroy();
	connDt1Texture.destroy();
	connDt2Texture.destroy();
}

void TitleScene::onDestroyRequest()
{
	switchingScene = true;
}

void TitleScene::onConnectionLost()
{
	connDtWidget.setTexture(&connDt2Texture);
	Game::schedule([this](){ closeConnDialog(); }, 2000);
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

	resetDialogAnim();
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

void TitleScene::renderTextDialog(const Vec2i& area, float windowFactor)
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

	txtBgWidget.setBounds({diagX, diagY, diagWidth, diagHeight});
	txtDtWidget.setBounds(txtBgWidget.getBounds());

	i32 textboxSize = std::lroundl((float(NICKNDIAGTB_IMG_SIZE.x) * 512.0f) / float(NICKNDIAG_IMG_SIZE.x) * diagSizeMul);
	i32 textboxWidth = textboxSize;
	i32 textboxHeight = std::lroundl(1.0f / NICKNDIAGTB_IMG_ARATIO * float(textboxSize));

	txtTbWidget.setBounds({
		std::lroundl(13.0f * diagScaleDiffPix) + diagX,
		std::lroundl(44.0f * diagScaleDiffPix) + diagY,
		textboxWidth,
		textboxHeight
	});

	i32 btn1Size = std::lroundl((float(NICKNDIAGBTN1_IMG_SIZE.x) * 512.0f) / float(NICKNDIAG_IMG_SIZE.x) * diagSizeMul);
	i32 btn1Width = btn1Size;
	i32 btn1Height = std::lroundl(1.0f / NICKNDIAGBTN1_IMG_ARATIO * float(btn1Size));

	txtbYWidget.setBounds({
		std::lroundl(165.0f * diagScaleDiffPix) + diagX,
		std::lroundl(86.0f * diagScaleDiffPix) + diagY,
		btn1Width,
		btn1Height
	});

	i32 btn2Size = std::lroundl((float(NICKNDIAGBTN2_IMG_SIZE.x) * 512.0f) / float(NICKNDIAG_IMG_SIZE.x) * diagSizeMul);
	i32 btn2Width = btn2Size;
	i32 btn2Height = std::lroundl(1.0f / NICKNDIAGBTN2_IMG_ARATIO * float(btn2Size));

	txtbNWidget.setBounds({
		std::lroundl(226.0f * diagScaleDiffPix) + diagX,
		std::lroundl(86.0f * diagScaleDiffPix) + diagY,
		btn2Width,
		btn2Height
	});
}

void TitleScene::showTextDialog()
{
	if (textDialogOpen)
		return;
	textDialogOpen = true;

	txtTbWidget.clear();
	Game::getGUI().setFocusedWidget(&txtTbWidget);

	auto area = Vec2i(Game::getFramebufferSize());
	float windowFactor = (float(area.x * area.y) / BASE_WND_AREA);
	renderTextDialog(area, windowFactor);

	setCommonWidgetsEnabled(false);

	textDialogCanvas.setVisible(true);
}

void TitleScene::closeTextDialog()
{
	textDialogCanvas.setVisible(false);

	setCommonWidgetsEnabled(true);

	textDialogOpen = false;
}

void TitleScene::showNicknameDialog()
{
	txtDtWidget.setTexture(&nndDtTexture);

	txtTbWidget.setFont("smooth");
	txtTbWidget.setFontScale(0.70f);
	txtTbWidget.setTextSidePadding(768.0f);
	txtTbWidget.setMaxCharacters(16);
	txtTbWidget.setCharValidator([](Unicode::Codepoint chr){ return true; });

	auto onClick = [this](){
		const U8String& txt = txtTbWidget.getText();
		if (!txt.empty())
		{
			Game::setPlayerName(txt);
			closeTextDialog();
			if (isCreating)
				createServerGame();
			else
				showIPDialog();
		}
	};

	txtbYWidget.setOnClick(onClick);
	txtTbWidget.setOnEnter(onClick);

	resetDialogAnim();
	showTextDialog();
}

void TitleScene::showIPDialog()
{
	txtDtWidget.setTexture(&ipdDtTexture);

	txtTbWidget.setFont("arial");
	txtTbWidget.setFontScale(0.70f);
	txtTbWidget.setTextSidePadding(768.0f);
	txtTbWidget.setMaxCharacters(21);
	txtTbWidget.setCharValidator([](Unicode::Codepoint chr){
		return (chr >= 48 && chr <= 58) || chr == 46;
	});

	auto onClick = [this](){
		const U8String& txt = txtTbWidget.getText();
		if (!txt.empty())
		{
			closeTextDialog();
			showConnDialog();
			connectToServer();
		}
	};

	txtbYWidget.setOnClick(onClick);
	txtTbWidget.setOnEnter(onClick);

	showTextDialog();
}

void TitleScene::connectToServer()
{
	const U8String& txt = txtTbWidget.getText();
	const char* txtc = reinterpret_cast<const char*>(txt.data());

	const char* sepPos = std::strchr(txtc, ':');

	std::string ip;
	u16 port;

	if (sepPos == nullptr)
	{
		ip = txtc;
		port = 25565;
	}
	else
	{
		ip = std::string(txtc, sepPos);
		port = std::atoi(sepPos + 1);
	}

	Game::connect(ip, port);
}

void TitleScene::renderConnDialog(const Vec2i& area, float windowFactor)
{
	float diagSizeGrowthMul = 4.5f;
	float diagSizeMul = (windowFactor / diagSizeGrowthMul) + (1.0f - (1.0f / diagSizeGrowthMul));

	diagSizeMul *= Math::lerp(lastDialogAnimTimer, dialogAnimTimer, Game::getTickAlpha());

	i32 diagSize = std::lroundl(512.0f * diagSizeMul);
	i32 diagWidth = diagSize;
	i32 diagHeight = std::lroundl(1.0f / CONNDIAG_IMG_ARATIO * float(diagSize));

	i32 diagX = (area.x - diagWidth) / 2;
	i32 diagY = (area.y - diagHeight) / 2;

	connBgWidget.setBounds({diagX, diagY, diagWidth, diagHeight});
	connDtWidget.setBounds(connBgWidget.getBounds());
}

void TitleScene::showConnDialog()
{
	if (connDialogOpen)
		return;
	connDialogOpen = true;

	connDtWidget.setTexture(&connDt1Texture);

	auto area = Vec2i(Game::getFramebufferSize());
	float windowFactor = (float(area.x * area.y) / BASE_WND_AREA);
	renderConnDialog(area, windowFactor);

	setCommonWidgetsEnabled(false);

	connDialogCanvas.setVisible(true);
}

void TitleScene::closeConnDialog()
{
	connDialogCanvas.setVisible(false);

	setCommonWidgetsEnabled(true);

	connDialogOpen = false;
}

void TitleScene::resetDialogAnim()
{
	dialogAnimTimer = 0.0f;
	lastDialogAnimTimer = 0.0f;
}

bool TitleScene::isDialogOpen() const
{
	return exitDialogOpen || textDialogOpen || connDialogOpen;
}

void TitleScene::createServerGame()
{
	std::system("start papgame_server");
	Game::connect("127.0.0.1", 25565);
}
