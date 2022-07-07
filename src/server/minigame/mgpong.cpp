#include "mgpong.hpp"

#include "../server.hpp"

#include "math/vector.hpp"
#include "math/rectangle.hpp"

#include "network/packet/pkt_s2c_pongstart.hpp"
#include "network/packet/pkt_s2c_pongmove.hpp"
#include "network/packet/pkt_s2c_pongpoint.hpp"
#include "network/packet/pkt_s2c_pongend.hpp"

constexpr u32 TICK_RATE = 60;
constexpr float TICK_DURATION = 1.0f / TICK_RATE;

constexpr u32 GAME_WIDTH = 640;
constexpr u32 GAME_HEIGHT = 480;

constexpr i32 GAME_SPEED = 30;
constexpr i32 PADDLE_SPEED = 8;
constexpr i32 INITIAL_BALL_SPEED = 4;
constexpr i32 MAX_BALL_SPEED = 14;
constexpr u32 POINTS_TO_WIN = 5;

constexpr Vec2i PADDLE_SIZE = { 28, 139 };
constexpr float PADDLE_ARATIO = float(PADDLE_SIZE.x) / float(PADDLE_SIZE.y);
constexpr Vec2i BALL_SIZE = { 32, 32 };
constexpr float BALL_ARATIO = float(BALL_SIZE.x) / float(BALL_SIZE.y);

//constexpr float FAKE_NET_DELAY_SECONDS = 0.0f;

IMPL_MINIGAME(MGPong, "Pong")

void MGPong::onCreate()
{
	Server::setTickRate(TICK_RATE);

	// Calculate rounds to play
	u32 playerCount = Server::getPlayers().size();
	roundsToPlay = (playerCount - 1) * (playerCount) / 2; // Use Gauss's formula

	playersFoughtByPlayer.clear();
}

void MGPong::onUpdate()
{
	if (pongPlaying)
	{
		for (SizeT i = 0; i < 2; i++)
			updatePaddleMovement(i, moveDirection[i]);
		updateBall();

		/*if (FAKE_NET_DELAY_SECONDS != 0.0f)
		{
			static float delayTicksTimer = 0.0f;
			if (delayTicksTimer < 1.0f)
			{
				delayTicksTimer += TICK_DURATION;
				return;
			}
			else
			{
				delayTicksTimer = 0;*/
		Log::info("Pong", "Sending move packets...");
		PKT_S2C_PongMove ballMovePkt(PongMovedObject::Ball, ball.pos, PongMoveDirection::None);
		PKT_S2C_PongMove pad1MovePkt(PongMovedObject::Paddle1, paddles[0].pos, moveDirection[0]);
		PKT_S2C_PongMove pad2MovePkt(PongMovedObject::Paddle2, paddles[1].pos, moveDirection[1]);
		for (auto& p : Server::getPlayers())
		{
			auto client = p->getClient();
			client->sendPacket(ballMovePkt);
			client->sendPacket(pad1MovePkt);
			client->sendPacket(pad2MovePkt);
		}
		/*	}
		}*/
	}
}

void MGPong::onDestroy()
{

}

void MGPong::onClientsReady()
{
	if (roundsPlayed == roundsToPlay)
	{
		// end game and start next minigame
		Server::startRandomMinigame();
		return;
	}

	Log::info("Pong", "Everyone is ready!");

	for (auto& points : paddlePoints)
		points = 0;

	decideRoundPlayers();

	PKT_S2C_PongStart pongStartPkt(playerForPaddle[0], playerForPaddle[1]);
	for (auto& p : Server::getPlayers())
		p->getClient()->sendPacket(pongStartPkt);

	resetMovables();
	pongPlaying = true;
}

void MGPong::decideRoundPlayers()
{
	auto& players = Server::getPlayers();
	for (auto& player1 : players)
	{
		for (auto& player2 : players)
		{
			if (player1 != player2 && !hasPlayerFoughtPlayer(player1->getID(), player2->getID()))
			{
				// Non-fought opponent found!

				playersFoughtByPlayer[player1->getID()].push_back(player2->getID());
				playersFoughtByPlayer[player2->getID()].push_back(player1->getID());
				playerForPaddle[0] = player1->getID();
				playerForPaddle[1] = player2->getID();
				break;
			}
		}
	}
}

void MGPong::resetMovables()
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
}

void MGPong::updatePaddleMovement(u32 paddleID, PongMoveDirection varMoveDirection)
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

void MGPong::updateBall()
{
	// Screen Y boundary collision
	if (ball.pos.y <= 0 || ball.pos.y >= (GAME_HEIGHT - BALL_SIZE.y))
		ball.vel.y *= -1;

	// Screen X boundary collision
	if (ball.pos.x <= -BALL_SIZE.x)
	{
		givePoint(1);
		return;
	}
	else if (ball.pos.x >= i32(GAME_WIDTH))
	{
		givePoint(0);
		return;
	}

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

void MGPong::increaseBallSpeed()
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

void MGPong::givePoint(u32 paddleID)
{
	resetMovables();

	paddlePoints[paddleID]++;
	bool isFinalPoint = paddlePoints[paddleID] == POINTS_TO_WIN;
	if (isFinalPoint) // WIN
		endRound(paddleID);
	for (auto& p : Server::getPlayers())
		p->getClient()->sendPacket<PKT_S2C_PongPoint>(isFinalPoint, paddleID);
}

void MGPong::endRound(u32 winnerPaddleID)
{
	pongPlaying = false;
	roundsPlayed++;

	u32 winnerPlayerID = playerForPaddle[winnerPaddleID];

	auto& players = Server::getPlayers();

	bool wasLastRound = roundsPlayed == roundsToPlay;

	auto* stats = new RoundStats();
	RoundStats* finalStats = wasLastRound ? new RoundStats() : nullptr;

	std::vector<Player*> pointSortedPlayers(players.size());
	for (SizeT i = 0; i < players.size(); i++)
		pointSortedPlayers[i] = players[i].get();
	std::sort(pointSortedPlayers.begin(), pointSortedPlayers.end(), [](Player* a, Player* b){
		return a->getPoints() > b->getPoints();
	});

	auto& playerInfo = stats->playerInfo;
	playerInfo.resize(pointSortedPlayers.size());
	for (SizeT i = 0; i < playerInfo.size(); i++)
	{
		u32 playerID = pointSortedPlayers[i]->getID();
		u32 earnedPoints = playerID == winnerPlayerID ? 5 : 0;
		pointSortedPlayers[i]->setPoints(pointSortedPlayers[i]->getPoints() + earnedPoints);
		playerInfo[i].playerID = playerID;
		playerInfo[i].place = i;
		playerInfo[i].points = earnedPoints;
	}

	if (wasLastRound)
	{
		std::vector<Player*> finalPointSortedPlayers(players.size());
		for (SizeT i = 0; i < players.size(); i++)
			finalPointSortedPlayers[i] = players[i].get();
		std::sort(finalPointSortedPlayers.begin(), finalPointSortedPlayers.end(), [](Player* a, Player* b){
			return a->getPoints() > b->getPoints();
		});

		auto& finalPlayerInfo = finalStats->playerInfo;
		finalPlayerInfo.resize(players.size());
		for (SizeT i = 0; i < finalPlayerInfo.size(); i++)
		{
			finalPlayerInfo[i].playerID = finalPointSortedPlayers[i]->getID();
			finalPlayerInfo[i].place = i;
			finalPlayerInfo[i].points = finalPointSortedPlayers[i]->getPoints();
		}
	}

	PKT_S2C_PongEnd pktEnd(roundsPlayed == roundsToPlay, stats, finalStats);
	for (auto& p : Server::getPlayers())
		p->getClient()->sendPacket(pktEnd);
}

u32 MGPong::getPlayerPaddle(u32 playerID)
{
	for (SizeT i = 0; i < 2; i++)
	{
		if (playerForPaddle[i] == playerID)
			return i;
	}
	return 0xFFFFFFFF;
}

void MGPong::onPlayerMove(Player* player, PongMoveDirection direction)
{
	u32 paddleID = getPlayerPaddle(player->getID());
	if (paddleID == 0xFFFFFFFF)
	{
		Log::info("Pong", "Tried to get paddle of invalid or spectating player");
		return;
	}
	moveDirection[paddleID] = direction;
}

bool MGPong::hasPlayerFoughtPlayer(u32 player1, u32 player2)
{
	if (playersFoughtByPlayer.contains(player1))
	{
		auto& foughtPlayers = playersFoughtByPlayer.at(player1);
		for (u32 foughtPlayer : foughtPlayers)
		{
			if (foughtPlayer == player2)
				return true;
		}
	}
	return false;
}
