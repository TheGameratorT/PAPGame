#include "Game.hpp"

#include <iostream>

#include "Utils.hpp"

#define MS_PER_UPDATE 16

Game::Game() :
	shouldQuitGame(false)
{
	loadGame();
}

Game::~Game() = default;

void Game::run()
{
	float previous = Utils::getCurrentTimeMS();
	float lag = 0.0;

	while (!shouldQuitGame)
	{
		float current = Utils::getCurrentTimeMS();
		float elapsed = current - previous;
		previous = current;
		lag += elapsed;

		processInput();

		while (lag >= MS_PER_UPDATE)
		{
			update();
			lag -= MS_PER_UPDATE;
		}

		render(lag / MS_PER_UPDATE);
	}
}

void Game::loadGame()
{

}

void Game::processInput()
{

}

void Game::update()
{

}

void Game::render(float delta)
{

}
