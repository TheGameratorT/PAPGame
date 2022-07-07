#include "mgpongscene.hpp"

#include "game.hpp"
#include "render/fader.hpp"
#include "player.hpp"
#include "util.hpp"

#include "screen/loadingscreen.hpp"
#include "screen/connectionlostscreen.hpp"
#include "screen/instructionscreen.hpp"
#include "screen/waitingforplayersscreen.hpp"

#include "network/gamenet.hpp"
#include "network/packet/pkt_c2s_playerready.hpp"
#include "network/packet/pkt_c2s_pongmove.hpp"
#include "screen/placelistingscreen.hpp"

constexpr u32 TICK_RATE = 60;
constexpr float TICK_DURATION = 1.0f / TICK_RATE;

constexpr float BASE_WND_AREA = 640.0f * 480.0f;

constexpr u32 GAME_WIDTH = 640;
constexpr u32 GAME_HEIGHT = 480;

constexpr i32 GAME_SPEED = 30;
constexpr i32 PADDLE_SPEED = 8;
constexpr i32 INITIAL_BALL_SPEED = 4;
constexpr i32 MAX_BALL_SPEED = 14;

constexpr Vec2i PADDLE_SIZE = { 28, 139 };
constexpr float PADDLE_ARATIO = float(PADDLE_SIZE.x) / float(PADDLE_SIZE.y);
constexpr Vec2i BALL_SIZE = { 32, 32 };
constexpr float BALL_ARATIO = float(BALL_SIZE.x) / float(BALL_SIZE.y);

/**
 * The square seen in the game that bounces
 * is referred in the code as "ball", this is
 * because referring to it as "square" or "object"
 * would be confusing. "square" could be misinterpreted
 * as just a rectangle object, and object could
 * be misinterpreted as literally anything that
 * is an object in the code.
 * */

IMPL_OBJECT(MGPongScene)

void MGPongScene::onCreate()
{
	Game::setTickRate(TICK_RATE);

	bgTexture.load("@/pong/bg.png");
	paddleTexture.load("@/pong/paddle.png", GLE::TextureFilter::None);
	ballTexture.load("@/pong/ball.png", GLE::TextureFilter::None);
	instructionTexture.load("@/pong/instruct.png");

	InstructionScreen::setInstructionTexture(&instructionTexture);

	bgWidget.setTexture(&bgTexture);

	for (auto& paddle : paddles)
	{
		paddle.img.setTexture(&paddleTexture);
		paddle.img.setZIndex(1);
		gameCanvas.addWidget(paddle.img);
	}
	ball.img.setTexture(&ballTexture);
	ball.img.setZIndex(0);
	gameCanvas.addWidget(ball.img);

	points1Widget.setFont("smooth");
	points1Widget.setTextAlign(mGUI::TextBox::AlignLeft);
	points2Widget.setFont("smooth");
	points2Widget.setTextAlign(mGUI::TextBox::AlignRight);
	spectTxtWidget.setFont("smooth");
	spectTxtWidget.setTextAlign(mGUI::TextBox::AlignCenter);
	spectTxtWidget.setText(u8"You are spectating, please wait for your turn.");
	spectTxtWidget.setVisible(false);

	bgWidget.setZIndex(0);
	gameCanvas.setZIndex(1);
	points1Widget.setZIndex(2);
	points2Widget.setZIndex(2);
	spectTxtWidget.setZIndex(2);

	canvas.addWidget(bgWidget);
	canvas.addWidget(gameCanvas);
	canvas.addWidget(points1Widget);
	canvas.addWidget(points2Widget);
	canvas.addWidget(spectTxtWidget);
	canvas.setZIndex(0);
	Game::getGUI().getContainer().addWidget(canvas);

	resetMovables();

	upKey = Game::bindKey(KeyCode::Up, [this](KeyState state){ onUpKey(state); });
	downKey = Game::bindKey(KeyCode::Down, [this](KeyState state){ onDownKey(state); });
}

void MGPongScene::onUpdate()
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

	if (pongPlaying)
	{
		updateRenderLerpPositions();
		for (SizeT i = 0; i < 2; i++)
			updatePaddleMovement(i, moveDirection[i]);
		updateBall();
	}
}

void MGPongScene::onRender()
{
	auto screenSize = Vec2i(Game::getFramebufferSize());

	float windowFactor = (float(screenSize.x * screenSize.y) / BASE_WND_AREA);

	bgWidget.setBounds({0, 0, screenSize.x, screenSize.y});

	// ------ PONG GAME AREA ------

	float gameScale = Math::min(float(screenSize.x) / GAME_WIDTH, float(screenSize.y) / GAME_HEIGHT);

	float gameOffsetX = (float(screenSize.x) - (GAME_WIDTH * gameScale)) / 2.0f;
	float gameOffsetY = (float(screenSize.y) - (GAME_HEIGHT * gameScale)) / 2.0f;

	float tickAlpha = Game::getTickAlpha();

	for (auto& paddle : paddles)
	{
		float lerpedPaddleX = std::lerp(float(paddle.lastRenderPos.x), float(paddle.pos.x), tickAlpha);
		float lerpedPaddleY = std::lerp(float(paddle.lastRenderPos.y), float(paddle.pos.y), tickAlpha);
		paddle.img.setBounds({
			std::lroundl(gameOffsetX + (lerpedPaddleX * gameScale)),
			std::lroundl(gameOffsetY + (lerpedPaddleY * gameScale)),
			std::lroundl(gameScale * float(PADDLE_SIZE.x)),
			std::lroundl(gameScale * float(PADDLE_SIZE.y)),
		});
	}
	ball.img.setBounds({
		std::lroundl(gameOffsetX + (std::lerp(float(ball.lastRenderPos.x), float(ball.pos.x), tickAlpha) * gameScale)),
		std::lroundl(gameOffsetY + (std::lerp(float(ball.lastRenderPos.y), float(ball.pos.y), tickAlpha) * gameScale)),
		std::lroundl(gameScale * float(BALL_SIZE.x)),
		std::lroundl(gameScale * float(BALL_SIZE.y)),
	});

	// ------ TEXTBOXES ------

	float textSizeGrowthMul = 4.5f;
	float textSizeMul = (windowFactor / textSizeGrowthMul) + (1.0f - (1.0f / textSizeGrowthMul));

	i32 textSidePadding = std::lroundl(31.0f * textSizeMul);

	points1Widget.setBounds({
		textSidePadding,
		std::lroundl(27.0f * textSizeMul),
		screenSize.x - textSidePadding,
		std::lroundl(27.0f * textSizeMul)
	});

	points2Widget.setBounds({
		0,
		std::lroundl(27.0f * textSizeMul),
		screenSize.x - textSidePadding,
		std::lroundl(27.0f * textSizeMul)
	});

	spectTxtWidget.setBounds({
		textSidePadding,
		screenSize.y - std::lroundl(46.0f * textSizeMul),
		screenSize.x - textSidePadding,
		std::lroundl(27.0f * textSizeMul)
	});
}

void MGPongScene::onDestroy()
{
	InstructionScreen::setInstructionTexture(nullptr);

	canvas.clear();
	Game::getGUI().getContainer().removeWidget(canvas);

	bgTexture.destroy();
	paddleTexture.destroy();
	ballTexture.destroy();

	Game::unbindKey(upKey);
	Game::unbindKey(downKey);

	WaitingForPlayersScreen::close();
}

void MGPongScene::onDestroyRequest()
{
	switchingScene = true;
}

void MGPongScene::onConnectionLost()
{
	ConnectionLostScreen::show();
	pongPlaying = false;
}

void MGPongScene::onInstructed()
{
	// Tell the server that we're ready to play
	GameNet::getConnection()->sendPacket<PKT_C2S_PlayerReady>(PKT_C2S_PlayerReady::GameReady);

	WaitingForPlayersScreen::show();
}

void MGPongScene::onPlaceListingClosed()
{
	// Tell the server that we're ready to play
	GameNet::getConnection()->sendPacket<PKT_C2S_PlayerReady>(PKT_C2S_PlayerReady::GameReady);

	WaitingForPlayersScreen::show();
}

void MGPongScene::resetMovables()
{
	i32 paddlePosY = (GAME_HEIGHT - PADDLE_SIZE.y) / 2;

	paddles[0].pos.x = 32;
	paddles[1].pos.x = 580;
	for (auto& paddle : paddles)
	{
		paddle.pos.y = paddlePosY;
		paddle.wasIntersectingBall = false;
	}

	curBallSpeed = INITIAL_BALL_SPEED;
	lastServiceDirection *= -1;

	ball.pos.x = (GAME_WIDTH - BALL_SIZE.x) / 2;
	ball.pos.y = (GAME_HEIGHT - BALL_SIZE.y) / 2;
	ball.vel.x = curBallSpeed * lastServiceDirection;
	ball.vel.y = -curBallSpeed;

	updateRenderLerpPositions(); // force teleport
}

void MGPongScene::updateRenderLerpPositions()
{
	for (auto& paddle : paddles)
		paddle.lastRenderPos = paddle.pos;
	ball.lastRenderPos = ball.pos;
}

void MGPongScene::updatePaddleMovement(u32 paddleID, PongMoveDirection varMoveDirection)
{
	switch (varMoveDirection)
	{
	case PongMoveDirection::Up:
		paddles[paddleID].pos.y -= PADDLE_SPEED * GAME_SPEED / TICK_RATE;
		if (paddles[paddleID].pos.y < 0)
			paddles[paddleID].pos.y = 0;
		break;
	case PongMoveDirection::Down:
		paddles[paddleID].pos.y += PADDLE_SPEED * GAME_SPEED / TICK_RATE;
		if (paddles[paddleID].pos.y + PADDLE_SIZE.y > GAME_HEIGHT)
			paddles[paddleID].pos.y = GAME_HEIGHT - PADDLE_SIZE.y;
		break;
	default:
		break;
	}
}

void MGPongScene::updateBall()
{
	// Screen Y boundary collision
	if (ball.pos.y <= 0 || ball.pos.y >= (GAME_HEIGHT - BALL_SIZE.y))
		ball.vel.y *= -1;

	// Screen X boundary collision
	// --- HANDLED BY SERVER! ---

	// Paddle collision
	for (auto& paddle : paddles)
	{
		RectI paddleBounds = RectI(paddle.pos, PADDLE_SIZE);
		RectI ballBounds = RectI(ball.pos, BALL_SIZE);

		bool isIntersectingBall = paddleBounds.intersects(ballBounds);
		bool wasIntersectingBall = paddle.wasIntersectingBall;
		paddle.wasIntersectingBall = isIntersectingBall;

		if (isIntersectingBall && !wasIntersectingBall)
		{
			increaseBallSpeed();
			ball.vel.x *= -1;
			if (ball.pos.y + BALL_SIZE.y <= paddle.pos.y)
				ball.vel.y = -(curBallSpeed * 2);
			else if (ball.pos.y >= paddle.pos.y + PADDLE_SIZE.y)
				ball.vel.y = curBallSpeed * 2;
			else
				ball.vel.y = curBallSpeed * Math::sign(ball.vel.y);
			break;
		}
	}

	ball.pos.x += ball.vel.x * i32(GAME_SPEED) / i32(TICK_RATE);
	ball.pos.y += ball.vel.y * i32(GAME_SPEED) / i32(TICK_RATE);
}

void MGPongScene::increaseBallSpeed()
{
	if (curBallSpeed < MAX_BALL_SPEED)
	{
		// increase ball speed
		curBallSpeed++;
		ball.vel.x = curBallSpeed * Math::sign(ball.vel.x);
		ball.vel.y = curBallSpeed * Math::sign(ball.vel.y);
		Log::info("Pong", "Ball Speed: " + std::to_string(curBallSpeed));
	}
}

void MGPongScene::onUpKey(KeyState state)
{
	if (state == KeyState::Pressed)
		movingUp = true;
	else if (state == KeyState::Released)
		movingUp = false;
	newMoveDirection = PongMoveDirection::Up;
	updateInput();
}

void MGPongScene::onDownKey(KeyState state)
{
	if (state == KeyState::Pressed)
		movingDown = true;
	else if (state == KeyState::Released)
		movingDown = false;
	newMoveDirection = PongMoveDirection::Down;
	updateInput();
}

void MGPongScene::updateInput()
{
	if (isSpectating)
		return;

	if (movingUp && movingDown)
		moveDirection[myPaddleID] = newMoveDirection;
	else if (movingUp)
		moveDirection[myPaddleID] = PongMoveDirection::Up;
	else if (movingDown)
		moveDirection[myPaddleID] = PongMoveDirection::Down;
	else
		moveDirection[myPaddleID] = PongMoveDirection::None;

	GameNet::getConnection()->sendPacket<PKT_C2S_PongMove>(moveDirection[myPaddleID]);
}

void MGPongScene::onGameStart(u32 player1, u32 player2)
{
	u32 playerID = Game::getPlayerID();
	if (player1 == playerID)
	{
		isSpectating = false;
		myPaddleID = 0;
	}
	else if (player2 == playerID)
	{
		isSpectating = false;
		myPaddleID = 1;
	}
	else
	{
		isSpectating = true;
		myPaddleID = 2; // spectator
	}

	playerForPaddle[0] = player1;
	playerForPaddle[1] = player2;

	spectTxtWidget.setVisible(isSpectating);

	for (auto& points : paddlePoints)
		points = 0;

	for (auto& direction : moveDirection)
		direction = PongMoveDirection::None;

	WaitingForPlayersScreen::close();

	resetMovables();
	updateTextboxes();
	pongPlaying = true;
}

void MGPongScene::onMoveObject(PongMovedObject movedObject, const Vec2i& position, PongMoveDirection direction)
{
	if (!pongPlaying) // async safety check
		return;

	Log::info("Pong", "Got move packets " +
	std::to_string(u32(movedObject)) + " [" + std::to_string(position.x) + ", " + std::to_string(position.y) + "] " +
	std::to_string(u32(direction)));

	if (movedObject == PongMovedObject::Ball)
	{
		// if ball is too far from server simulation, then readjust
		const u32 distanceThreshold = 2;
		double distance = ball.pos.distance(position);
		Log::info("Pong", "Ball distance: " + std::to_string(distance));
		if (distance > curBallSpeed * distanceThreshold)
			ball.pos = position;
		return;
	}

	if (movedObject == PongMovedObject::Paddle1 || movedObject == PongMovedObject::Paddle2)
	{
		u32 paddleID = movedObject == PongMovedObject::Paddle1 ? 0 : 1;
		if (paddleID != myPaddleID)
		{
			moveDirection[paddleID] = direction;
			Vec2i& paddlePos = paddles[paddleID].pos;

			const u32 distanceThreshold = 8;
			double distance = paddlePos.distance(position);
			Log::info("Pong", "Paddle " + std::to_string(paddleID) + " distance: " + std::to_string(distance));
			if (distance < distanceThreshold)
				paddlePos = position;
		}
	}
}

void MGPongScene::onGamePoint(bool isFinalPoint, u32 paddleID)
{
	paddlePoints[paddleID]++;
	if (isFinalPoint)
	{
		pongPlaying = false;
		updateRenderLerpPositions(); // force teleport
	}
	else
	{
		resetMovables();
	}
	updateTextboxes();
}

void MGPongScene::onRoundEnd(const RoundStats* stats, const RoundStats* finalStats)
{
	bool wasLastRound = finalStats != nullptr;
	WaitingForPlayersScreen::close();
	PlaceListingScreen::show(wasLastRound, stats, finalStats);
}

void MGPongScene::updateTextboxes()
{
	Player* p1 = Game::getPlayerByID(playerForPaddle[0]);
	if (p1)
	{
		U8String pointsText = p1->getName();
		pointsText.append(Util::stringAsUtf8(" - " + std::to_string(paddlePoints[0])));
		points1Widget.setText(pointsText);
	}

	Player* p2 = Game::getPlayerByID(playerForPaddle[1]);
	if (p2)
	{
		U8String pointsText = p2->getName();
		pointsText.append(Util::stringAsUtf8(" - " + std::to_string(paddlePoints[1])));
		points2Widget.setText(pointsText);
	}
}
