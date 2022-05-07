#include "splashscene.hpp"

#include "game.hpp"
#include "titlescene.hpp"
#include "render/renderer.hpp"

#include "network/clientconnection.hpp"

IMPL_OBJECT(SplashScene)

void SplashScene::onCreate()
{
	splashTexture1.load("@/conn.png");
	splashTexture2.load("@/conn_fail.png");
	splashTexture3.load("@/conn_succ.png");
	splashTexture4.load("@/conn_lost.png");
}

void SplashScene::onUpdate()
{
}

void SplashScene::onRender()
{
	bool isConnected = Game::getConn()->isConnected();
	if (lastNetState == 0) // connecting
	{
		if (!isConnected)
		{
			Renderer::renderTexture(splashTexture1, {});
			if (Game::gotConnErr())
			{
				lastNetState = 2;
			}
		}
		else
		{
			lastNetState = 1;
		}
	}

	if (lastNetState == 1) // connected
	{
		if (isConnected)
		{
			Renderer::renderTexture(splashTexture3, {});
		}
		else
		{
			lastNetState = 3;
		}
	}

	if (lastNetState == 2) // connect fail
	{
		Renderer::renderTexture(splashTexture2, {});
		if (Game::gotConnErr() == 0)
		{
			lastNetState = 0;
		}
	}

	if (lastNetState == 3) // connection lost
	{
		Renderer::renderTexture(splashTexture4, {});
		if (Game::gotConnErr() == 0)
		{
			lastNetState = 0;
		}
	}
}

void SplashScene::onDestroy()
{
	splashTexture1.destroy();
}

void SplashScene::onDestroyRequest()
{
}
