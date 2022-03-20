#include <iostream>

#include "Game.hpp"

int main()
{
	std::ios_base::sync_with_stdio(false);

	auto* game = new Game();
	game->init();
	game->run();
	game->destroy();
	delete game;

	return 0;
}
