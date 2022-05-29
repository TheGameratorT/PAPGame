#include "titlescene.hpp"

#include "game.hpp"

constexpr u32 TICK_RATE = 30;
constexpr float TICK_DURATION = 1.0f / TICK_RATE;

IMPL_OBJECT(TitleScene)

void TitleScene::onCreate()
{
	Game::setTickRate(TICK_RATE);

	bgTexture.load("@/menu_bg.png");

	mGUI::Interface& gui = Game::getGUI();
	bgWidget.setTexture(bgTexture);
	gui.addWidget(bgWidget);
}

void TitleScene::onUpdate()
{
}

void TitleScene::onRender()
{
	Vec2ui area = Game::getFramebufferSize();

	bgWidget.bounds = RectI(0, 0, area.x, area.y);
}

void TitleScene::onDestroy()
{
	mGUI::Interface& gui = Game::getGUI();
	bgWidget.setTexture(bgTexture);
	gui.removeWidget(bgWidget);
}

void TitleScene::onDestroyRequest()
{
}
