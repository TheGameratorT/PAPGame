#pragma once

#include "scene.hpp"

#include "gui/container.hpp"
#include "gui/image.hpp"
#include "gui/textbox.hpp"
#include "render/texture.hpp"
#include "input/keyhandle.hpp"

#include "pongenums.hpp"
#include "roundstats.hpp"

class MGPongScene : public Scene
{
public:
	DECL_OBJECT(MGPongScene)

	void onCreate() override;
	void onUpdate() override;
	void onRender() override;
	void onDestroy() override;
	void onDestroyRequest() override;
	void onConnectionLost() override;
	void onInstructed() override;
	void onPlaceListingClosed() override;

	void onRoundEnd(const RoundStats* stats, const RoundStats* finalStats);
	void onGameStart(u32 player1, u32 player2);
	void onMoveObject(PongMovedObject movedObject, const Vec2i& position, PongMoveDirection direction);
	void onGamePoint(bool isFinalPoint, u32 paddleID);

private:
	struct PongPaddle
	{
		mGUI::Image img;
		Vec2i pos;
		Vec2i lastPos;
		Vec2i lastRenderPos;
		bool wasIntersectingBall = false;
	};

	struct PongBall
	{
		mGUI::Image img;
		Vec2i pos;
		Vec2i lastPos;
		Vec2i lastRenderPos;
		Vec2i vel;
		float rot = 0.0f;
	};

	bool isFadingInScene = true;
	bool switchingScene = false;

	mGUI::Container canvas;
	mGUI::Container gameCanvas;
	mGUI::TextBox points1Widget;
	mGUI::TextBox points2Widget;
	mGUI::TextBox spectTxtWidget;

	Texture bgTexture;
	Texture paddleTexture;
	Texture ballTexture;
	Texture instructionTexture;

	mGUI::Image bgWidget;

	PongPaddle paddles[2];
	PongBall ball;

	KeyHandle upKey;
	KeyHandle downKey;

	bool movingUp = false;
	bool movingDown = false;
	PongMoveDirection newMoveDirection = PongMoveDirection::None;
	PongMoveDirection moveDirection[2];

	i32 curBallSpeed = 0;
	i32 lastServiceDirection = 1;
	u32 myPaddleID;
	u32 paddlePoints[2];
	u32 playerForPaddle[2];

	bool pongPlaying = false;
	bool isSpectating = false;

	void resetMovables();
	void updateRenderLerpPositions();
	void updatePaddleMovement(u32 paddleID, PongMoveDirection varMoveDirection);
	void updateBall();
	void increaseBallSpeed();

	void onUpKey(KeyState state);
	void onDownKey(KeyState state);
	void updateInput();

	void updateTextboxes();
};
