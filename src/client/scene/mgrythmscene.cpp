#include "mgrythmscene.hpp"

#include "game.hpp"
#include "render/fader.hpp"
#include "screen/loadingscreen.hpp"
#include "screen/connectionlostscreen.hpp"
#include "player.hpp"
#include "util.hpp"

constexpr u32 TICK_RATE = 30;
constexpr float TICK_DURATION = 1.0f / TICK_RATE;

constexpr float BASE_WND_AREA = 640.0f * 480.0f;

IMPL_OBJECT(MGRythmScene)

void MGRythmScene::onCreate()
{
	Game::setTickRate(TICK_RATE);

	Game::getGUI().getContainer().addWidget(canvas);
}

void MGRythmScene::onUpdate()
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

void MGRythmScene::onRender()
{
}

void MGRythmScene::onDestroy()
{
	canvas.clear();
	Game::getGUI().getContainer().removeWidget(canvas);
}

void MGRythmScene::onDestroyRequest()
{
	switchingScene = true;
}

void MGRythmScene::onConnectionLost()
{
	ConnectionLostScreen::show();
}
