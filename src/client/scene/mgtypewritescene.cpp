#include "mgtypewritescene.hpp"

#include "game.hpp"
#include "roundstats.hpp"
#include "render/fader.hpp"
#include "screen/loadingscreen.hpp"
#include "screen/connectionlostscreen.hpp"
#include "screen/instructionscreen.hpp"
#include "screen/waitingforplayersscreen.hpp"
#include "screen/placelistingscreen.hpp"
#include "network/gamenet.hpp"
#include "player.hpp"
#include "util.hpp"

#include "network/packet/pkt_c2s_playerready.hpp"
#include "network/packet/pkt_c2s_typestate.hpp"

constexpr u32 TICK_RATE = 30;
constexpr float TICK_DURATION = 1.0f / TICK_RATE;

constexpr float BASE_WND_AREA = 640.0f * 480.0f;

constexpr Vec2i MAINPANEL_IMG_SIZE = { 549, 261 };
constexpr float MAINPANEL_IMG_ARATIO = float(MAINPANEL_IMG_SIZE.x) / float(MAINPANEL_IMG_SIZE.y);

constexpr Vec2i CNTDWNNUM_IMG_SIZE = { 128, 190 };
constexpr float CNTDWNNUM_IMG_ARATIO = float(CNTDWNNUM_IMG_SIZE.x) / float(CNTDWNNUM_IMG_SIZE.y);

IMPL_OBJECT(MGTypeWriteScene)

void MGTypeWriteScene::onCreate()
{
	Game::setTickRate(TICK_RATE);

	bgTexture.load("@/lobby/background.png");
	mainPanelTexture.load("@/typewriter/textholder.png", GLE::TextureFilter::None);
	instructionTexture.load("@/typewriter/instruct.png", GLE::TextureFilter::None);
	for (SizeT i = 0; i < 3; i++)
		cntDwnTexture[i].load("@/cntdwn/" + std::to_string(i + 1) + ".png");

	InstructionScreen::setInstructionTexture(&instructionTexture);

	bgWidget.setTexture(&bgTexture);
	mainPanelWidget.setTexture(&mainPanelTexture);

	storyTitleWidget.setFont("smooth");
	storyTitleWidget.setReadOnly(true);
	storyTitleWidget.setTextAlign(mGUI::TextBox::AlignCenter);

	textboxWidget.setFont("smooth");
	textboxWidget.setReadOnly(true);
	textboxWidget.setFontColor({ 211, 211, 211, 255 });

	typeTextboxWidget.setFont("smooth");
	typeTextboxWidget.setCaretFixed(true);
	typeTextboxWidget.setCaretHidden(true);
	typeTextboxWidget.setBackspaceDisabled(true);
	typeTextboxWidget.setClipboardDisabled(true);
	typeTextboxWidget.setCharValidator([this](Unicode::Codepoint cp){ return onPlayerType(cp); });

	lettersTypedWidget.setFont("smooth");
	lettersTypedWidget.setReadOnly(true);
	playerLeadingWidget.setFont("smooth");
	playerLeadingWidget.setReadOnly(true);
	playerPlaceWidget.setFont("smooth");
	playerPlaceWidget.setReadOnly(true);
	youareBehindWidget.setFont("smooth");
	youareBehindWidget.setReadOnly(true);
	typefastWidget.setText(u8"Type as fast as you can!");
	typefastWidget.setFont("smooth");
	typefastWidget.setReadOnly(true);

	bgWidget.setZIndex(0);
	cntDwnWidget.setZIndex(1);
	storyTitleWidget.setZIndex(1005);
	textboxWidget.setZIndex(1005);
	typeTextboxWidget.setZIndex(1006);
	lettersTypedWidget.setZIndex(1008);
	playerLeadingWidget.setZIndex(1008);
	playerPlaceWidget.setZIndex(1008);
	youareBehindWidget.setZIndex(1008);
	typefastWidget.setZIndex(1008);

	typeCanvas.addWidget(mainPanelWidget);
	typeCanvas.addWidget(storyTitleWidget);
	typeCanvas.addWidget(textboxWidget);
	typeCanvas.addWidget(typeTextboxWidget);
	typeCanvas.addWidget(lettersTypedWidget);
	typeCanvas.addWidget(playerLeadingWidget);
	typeCanvas.addWidget(playerPlaceWidget);
	typeCanvas.addWidget(youareBehindWidget);
	typeCanvas.addWidget(typefastWidget);
	typeCanvas.setZIndex(1);
	typeCanvas.setVisible(false);
	canvas.addWidget(typeCanvas);

	canvas.addWidget(bgWidget);
	canvas.addWidget(cntDwnWidget);
	canvas.setZIndex(0);
	Game::getGUI().getContainer().addWidget(canvas);

	loadStories();

	//LoadingScreen::show();
}

void MGTypeWriteScene::onUpdate()
{
	if (isFadingInScene)
	{
		if (Fader::hasFadingFinished())
		{
			InstructionScreen::show();
			isFadingInScene = false;
		}
		return;
	}

	if (switchingScene)
	{
		if (Fader::hasFadingFinished())
		{
			LoadingScreen::close();
			destroy();
		}
		return;
	}

	if (playingStory)
	{
		Game::getGUI().setFocusedWidget(&typeTextboxWidget); // always in focus

		GameNet::getConnection()->sendPacket<PKT_C2S_TypeState>(lettersTyped);
	}
}

void MGTypeWriteScene::onRender()
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
		std::lroundl(118.0f * mainPanelSizeMul),
		mainPanelWidth,
		mainPanelHeight
	});

	// STORY TITLE

	storyTitleWidget.setBounds({
		panelWidgetX,
		std::lroundl(131.0f * mainPanelSizeMul),
		std::lroundl(594.0f * mainPanelSizeMul),
		std::lroundl(18.0f * mainPanelSizeMul)
	});

	RectI textboxBounds = {
		panelWidgetX + std::lroundl(14.0f * mainPanelSizeMul),
		std::lroundl(175.0f * mainPanelSizeMul),
		std::lroundl(260.0f * mainPanelSizeMul),
		std::lroundl(20.0f * mainPanelSizeMul)
	};

	// TEXBOX (BACK - GREY - FULL TEXT)

	textboxWidget.setBounds(textboxBounds);

	// TEXBOX (TOP - BLACK - USER TEXT)

	typeTextboxWidget.setBounds(textboxBounds);

	// LETTERS TYPED TEXT

	lettersTypedWidget.setBounds({
		panelWidgetX,
		std::lroundl(35.0f * mainPanelSizeMul),
		std::lroundl(260.0f * mainPanelSizeMul),
		std::lroundl(20.0f * mainPanelSizeMul)
	});

	// PLAYER LEADING TEXT

	playerLeadingWidget.setBounds({
		panelWidgetX,
		std::lroundl(67.0f * mainPanelSizeMul),
		std::lroundl(260.0f * mainPanelSizeMul),
		std::lroundl(20.0f * mainPanelSizeMul)
	});

	// YOUR POSTION TEXT

	playerPlaceWidget.setBounds({
		panelWidgetX + std::lroundl(304.0f * mainPanelSizeMul),
		std::lroundl(35.0f * mainPanelSizeMul),
		std::lroundl(260.0f * mainPanelSizeMul),
		std::lroundl(20.0f * mainPanelSizeMul)
	});

	// YOU ARE BEHIND TEXT

	youareBehindWidget.setBounds({
		panelWidgetX + std::lroundl(304.0f * mainPanelSizeMul),
		std::lroundl(67.0f * mainPanelSizeMul),
		std::lroundl(260.0f * mainPanelSizeMul),
		std::lroundl(20.0f * mainPanelSizeMul)
	});

	// TYPE AS FAST AS YOU CAN

	typefastWidget.setBounds({
		panelWidgetX + std::lroundl(74.0f * mainPanelSizeMul),
		std::lroundl(395.0f * mainPanelSizeMul),
		std::lroundl(417.0f * mainPanelSizeMul),
		std::lroundl(35.0f * mainPanelSizeMul)
	});

	// COUNTDOWN NUMBERS

	if (cntDwnWidget.getVisible())
	{
		i32 numWidth = std::lroundl(CNTDWNNUM_IMG_SIZE.x * mainPanelSizeMul);
		i32 numHeight = std::lroundl(1.0f / CNTDWNNUM_IMG_ARATIO * float(numWidth));

		cntDwnWidget.setBounds({
			(area.x - numWidth) / 2,
			(area.y - numHeight) / 2,
			std::lroundl(CNTDWNNUM_IMG_SIZE.x * mainPanelSizeMul),
			std::lroundl(CNTDWNNUM_IMG_SIZE.y * mainPanelSizeMul)
		});
	}

}

void MGTypeWriteScene::onDestroy()
{
	InstructionScreen::setInstructionTexture(nullptr);

	canvas.clear();
	Game::getGUI().getContainer().removeWidget(canvas);

	bgTexture.destroy();
	mainPanelTexture.destroy();
	instructionTexture.destroy();
	for (auto& i : cntDwnTexture)
		i.destroy();

	WaitingForPlayersScreen::close();
}

void MGTypeWriteScene::onDestroyRequest()
{
	switchingScene = true;
}

void MGTypeWriteScene::onConnectionLost()
{
	ConnectionLostScreen::show();
}

void MGTypeWriteScene::onInstructed()
{
	// Tell the server that we're ready to play
	GameNet::getConnection()->sendPacket<PKT_C2S_PlayerReady>(PKT_C2S_PlayerReady::GameReady);

	WaitingForPlayersScreen::show();
}

void MGTypeWriteScene::onPlaceListingClosed()
{
	// Tell the server that we're ready to play
	GameNet::getConnection()->sendPacket<PKT_C2S_PlayerReady>(PKT_C2S_PlayerReady::GameReady);

	WaitingForPlayersScreen::show();
}

void MGTypeWriteScene::onTypingStart(u32 storyID)
{
	curStory = storyID;
	lettersTyped = 0;
	charOffset = 0;

	TypeWriterStory& story = stories[curStory];

	storyTitleWidget.setText(story.getTitle());
	textboxWidget.setText(story.getText());
	typeTextboxWidget.clear();
	typeTextboxWidget.setReadOnly(false);
	lettersToType = story.getCharCount();

	WaitingForPlayersScreen::close();

	cntDwnWidget.setVisible(true);
	doCountDown();
}

void MGTypeWriteScene::onTypingStats(const TypeStateStats& stats)
{
	curStats = stats;
	updateTextboxes();
}

void MGTypeWriteScene::onRoundEnd(const RoundStats* stats, const RoundStats* finalStats)
{
	bool wasLastRound = finalStats != nullptr;
	WaitingForPlayersScreen::close();
	PlaceListingScreen::show(wasLastRound, stats, finalStats);
}

bool MGTypeWriteScene::onPlayerType(Unicode::Codepoint cp)
{
	if (!playingStory)
		return false;

	const U8String& text = stories[curStory].getText();

	u32 charOffsetTmp = charOffset;

	bool isNewLineChar = text[charOffsetTmp] == '\n';
	if (isNewLineChar)
		charOffsetTmp++;

	if (text[charOffsetTmp] == cp)
	{
		if (isNewLineChar)
			typeTextboxWidget.put('\n');
		lettersTyped++;
		charOffsetTmp++;
		charOffset = charOffsetTmp;
		if (lettersTyped == lettersToType)
			finishStoryType();
		return true;
	}

	return false;
}

void MGTypeWriteScene::finishStoryType()
{
	GameNet::getConnection()->sendPacket<PKT_C2S_TypeState>(lettersTyped);
	WaitingForPlayersScreen::show();
	typeTextboxWidget.setReadOnly(true);
	playingStory = false;
}

void MGTypeWriteScene::loadStories()
{
	stories = TypeWriterStory::loadStories("@/typewriter/textdata.txt");
	for (auto& story : stories)
		Log::info("MGTypeWriter",
				  std::string("Loaded story: ") + Util::utf8AsString(story.getTitle()) +
				  " (size: " + std::to_string(story.getText().size()) + ")");
}

void MGTypeWriteScene::updateTextboxes()
{
	U8String lettersTypedTxt = u8"Letters typed: ";
	lettersTypedTxt.append(Util::stringAsUtf8(std::to_string(lettersTyped)));
	lettersTypedTxt.append(u8"/");
	lettersTypedTxt.append(Util::stringAsUtf8(std::to_string(lettersToType)));
	lettersTypedWidget.setText(lettersTypedTxt);

	Player* leadingPlayer = Game::getPlayerByID(curStats.leadingPlayerID);
	if (leadingPlayer)
	{
		U8String playerLeadingTxt = u8"Player leading: ";
		playerLeadingTxt.append(leadingPlayer->getName());
		playerLeadingTxt.append(u8" (");
		playerLeadingTxt.append(Util::stringAsUtf8(std::to_string(curStats.leadingPlayerTL)));
		playerLeadingTxt.append(u8")");
		playerLeadingWidget.setText(playerLeadingTxt);
	}

	static const char* placeSuffix[] = { "st", "nd", "rd", "th" };
	u32 leadingPlace = curStats.leadingPlace;

	U8String playerPlaceTxt = u8"Your position: ";
	playerPlaceTxt.append(Util::stringAsUtf8(std::to_string(leadingPlace + 1)));
	playerPlaceTxt.append(Util::stringAsUtf8(placeSuffix[leadingPlace > 3 ? 3 : leadingPlace]));
	playerPlaceTxt.append(u8" place");
	playerPlaceWidget.setText(playerPlaceTxt);

	Player* playerInFront = Game::getPlayerByID(curStats.inFrontPlayerID);
	if (playerInFront)
	{
		if (playerInFront == Game::getPlayer())
		{
			youareBehindWidget.setText(u8"You are on the lead!");
		}
		else
		{
			U8String youareBehindTxt = u8"You are behind: ";
			youareBehindTxt.append(playerInFront->getName());
			youareBehindWidget.setText(youareBehindTxt);
		}
	}
}

void MGTypeWriteScene::doCountDown()
{
	if (curCntDwnTex == 0)
	{
		curCntDwnTex = 3;
		cntDwnWidget.setVisible(false);

		// start the game
		typeCanvas.setVisible(true);
		playingStory = true;
		return;
	}
	curCntDwnTex--;
	cntDwnWidget.setTexture(&cntDwnTexture[curCntDwnTex]);
	Game::schedule([this](){ doCountDown(); }, 1000);
}
