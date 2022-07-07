#pragma once

#include <vector>
#include <unordered_map>

#include "../minigame.hpp"
#include "math/vector.hpp"
#include "pongenums.hpp"

class Player;

class MGPong : public MiniGame
{
public:
	DECL_MINIGAME()

	void onCreate() override;
	void onUpdate() override;
	void onDestroy() override;
	void onClientsReady() override;

	void onPlayerMove(Player* player, PongMoveDirection direction);

private:
	struct PongPaddle
	{
		Vec2i pos;
		bool wasIntersectingBall = false;
	};

	struct PongBall
	{
		Vec2i pos;
		Vec2i vel;
		float rot = 0.0f;
	};

	bool pongPlaying = false;
	i32 curBallSpeed = 0;
	i32 lastServiceDirection = 1;
	u32 roundsPlayed = 0;
	u32 roundsToPlay = 0;

	PongPaddle paddles[2];
	PongBall ball;
	PongMoveDirection moveDirection[2];
	u32 playerForPaddle[2];
	u32 paddlePoints[2];

	std::unordered_map<u32, std::vector<u32>> playersFoughtByPlayer;

	void decideRoundPlayers();
	void resetMovables();
	void updatePaddleMovement(u32 paddleID, PongMoveDirection varMoveDirection);
	void updateBall();
	void increaseBallSpeed();
	void givePoint(u32 paddleID);
	void endRound(u32 winnerPaddleID);
	u32 getPlayerPaddle(u32 playerID);
	bool hasPlayerFoughtPlayer(u32 player1, u32 player2);
};
