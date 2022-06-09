#include "lobbyscene.hpp"

#include "game.hpp"

constexpr u32 TICK_RATE = 30;
constexpr float TICK_DURATION = 1.0f / TICK_RATE;

constexpr float BASE_WND_AREA = 640.0f * 480.0f;

IMPL_OBJECT(LobbyScene)

void LobbyScene::onCreate()
{
	Game::setTickRate(TICK_RATE);


	canvas.setZIndex(0);

	Game::getGUI().getContainer().addWidget(canvas);
}

void LobbyScene::onUpdate()
{

}

void LobbyScene::onRender()
{

}

void LobbyScene::onDestroy()
{
	canvas.clear();
	Game::getGUI().getContainer().removeWidget(canvas);


}

void LobbyScene::onDestroyRequest()
{
	destroy();
}
