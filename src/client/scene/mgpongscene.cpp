#include "mgpongscene.hpp"

#include "game.hpp"
#include "render/fader.hpp"
#include "screen/loadingscreen.hpp"
#include "screen/connectionlostscreen.hpp"
#include "network/gamenet.hpp"
#include "player.hpp"
#include "util.hpp"

constexpr u32 TICK_RATE = 30;
constexpr float TICK_DURATION = 1.0f / TICK_RATE;

constexpr float BASE_WND_AREA = 640.0f * 480.0f;

IMPL_OBJECT(MGPongScene)

void MGPongScene::onCreate()
{
	Game::setTickRate(TICK_RATE);

	Game::getGUI().getContainer().addWidget(canvas);
}

void MGPongScene::onUpdate()
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

void MGPongScene::onRender()
{
	auto area = Vec2i(Game::getFramebufferSize());

	float windowFactor = (float(area.x * area.y) / BASE_WND_AREA);

}

void MGPongScene::onDestroy()
{
	canvas.clear();
	Game::getGUI().getContainer().removeWidget(canvas);
}

void MGPongScene::onDestroyRequest()
{
	switchingScene = true;
}

void MGPongScene::onConnectionLost()
{
	ConnectionLostScreen::show();
}
