#include "mgtypewritescene.hpp"

#include "game.hpp"
#include "render/fader.hpp"
#include "loadingscreen.hpp"

constexpr u32 TICK_RATE = 10;
constexpr float TICK_DURATION = 1.0f / TICK_RATE;

IMPL_OBJECT(MGTypeWriteScene)

void MGTypeWriteScene::onCreate()
{
	Game::setTickRate(TICK_RATE);

	LoadingScreen::show();

	canvas.setZIndex(0);
	Game::getGUI().getContainer().addWidget(canvas);
}

void MGTypeWriteScene::onUpdate()
{
	if (switchingScene)
	{
		if (Fader::hasFadingFinished())
		{
			LoadingScreen::close();
			destroy();
		}
		return;
	}
}

void MGTypeWriteScene::onRender()
{
}

void MGTypeWriteScene::onDestroy()
{
	canvas.clear();
	Game::getGUI().getContainer().removeWidget(canvas);
}

void MGTypeWriteScene::onDestroyRequest()
{
	switchingScene = true;
}
