#pragma once

#include <memory>

#include "graphics/Window.hpp"
#include "input/InputHolder.hpp"

class Game
{
public:
	Game();
	~Game();

	void run();
	void processInput();
	void update();
	void render(float delta);

	[[nodiscard]] constexpr bool getQuitGame() const { return shouldQuitGame; };
	constexpr void quit() { shouldQuitGame = true; };

private:
	void loadGame();

	std::unique_ptr<Window> window;
	InputHolder input;
	bool shouldQuitGame;
};
