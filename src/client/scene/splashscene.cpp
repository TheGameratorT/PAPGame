#include "splashscene.hpp"

#include "game.hpp"
#include "titlescene.hpp"
#include "loadingscreen.hpp"

#include "render/gle/render.hpp"
#include "render/fader.hpp"

constexpr u32 TICK_RATE = 10;
constexpr float TICK_DURATION = 1.0f / TICK_RATE;

constexpr float TITLE_IMG_ARATIO = 512.0f / 256.0f;
constexpr float GAMEBY_IMG_ARATIO = 140.0f / 24.0f;

IMPL_OBJECT(SplashScene)

void SplashScene::onCreate()
{
	Game::setTickRate(TICK_RATE);

	LoadingScreen::show();

	Game::getGUI().getContainer().addWidget(canvas);
	canvas.setZIndex(3000);

	gameByTexture.load("@/game_by.png", GLE::TextureFilter::None);
	logoTexture.load("@/title.png", GLE::TextureFilter::None);

	gameByWidget.setZIndex(1);
	gameByWidget.setTexture(&gameByTexture);
	canvas.addWidget(gameByWidget);

	logoWidget.setZIndex(0);
	logoWidget.setTexture(&logoTexture);
	canvas.addWidget(logoWidget);
}

void SplashScene::onUpdate()
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

	if (timer >= 0.0f)
	{
		if (timer >= 4.0f)
		{
			Game::switchScene<TitleScene>();
			timer = -1.0f;
		}
		else
		{
			timer += TICK_DURATION;
		}
	}
}

void SplashScene::onRender()
{
	Vec2ui area = Game::getFramebufferSize();

	auto areaSum = float(area.x + area.y);

	auto logoSize = areaSum / 1.8f;
	i32 targetWidth = i32(logoSize);
	i32 targetHeight = i32(1.0f / TITLE_IMG_ARATIO * logoSize);
	logoWidget.setBounds({(i32(area.x) - targetWidth) / 2, 0, targetWidth, targetHeight});

	auto gameBySize = areaSum / 3.5f;
	i32 targetWidth2 = i32(gameBySize);
	i32 targetHeight2 = i32(1.0f / GAMEBY_IMG_ARATIO * gameBySize);
	gameByWidget.setBounds({
		(i32(area.x) - targetWidth2) / 2,
		std::lroundl(float(targetHeight) - 48 * (areaSum / 1100.0f)),
		targetWidth2,
		targetHeight2
	});
}

void SplashScene::onDestroy()
{
	canvas.clear();
	Game::getGUI().getContainer().removeWidget(canvas);
	gameByTexture.destroy();
	logoTexture.destroy();
}

void SplashScene::onDestroyRequest()
{
	switchingScene = true;
}
