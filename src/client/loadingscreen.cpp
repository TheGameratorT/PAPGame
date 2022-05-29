#include "loadingscreen.hpp"

#include "render/texture.hpp"
#include "gui/image.hpp"
#include "gui/interface.hpp"

#include "game.hpp"

namespace LoadingScreen
{
	static Texture bgTexture;
	static Texture loadTextTexture;
	static Texture loadAnimTexture;
	static mGUI::Image bgWidget[2];
	static mGUI::Image loadTextWidget;
	static mGUI::Image loadAnimWidget;
	static i32 bgOffset;
	static i32 lastBgOffset;
	static float gearRotation;
	static float lastGearRotation;
	static bool isVisible = false;

	void init()
	{
		bgTexture.load("@/loading_bg.png");
		loadTextTexture.load("@/loading_text.png");
		loadAnimTexture.load("@/loading_anim.png");
	}

	void destroy()
	{
		bgTexture.destroy();
		loadTextTexture.destroy();
		loadAnimTexture.destroy();
	}

	void show()
	{
		if (isVisible)
			return;

		mGUI::Interface& gui = Game::getGUI();
		for (auto& i : bgWidget)
		{
			i.renderPriority = 1000;
			i.setTexture(bgTexture);
			gui.addWidget(i);
		}
		loadTextWidget.renderPriority = 1001;
		loadTextWidget.setTexture(loadTextTexture);
		gui.addWidget(loadTextWidget);
		loadAnimWidget.renderPriority = 1001;
		loadAnimWidget.setTexture(loadAnimTexture);
		gui.addWidget(loadAnimWidget);

		bgOffset = 0;
		lastBgOffset = 0;
		gearRotation = 0;
		lastGearRotation = 0;
		isVisible = true;
	}

	void close()
	{
		if (!isVisible)
			return;

		mGUI::Interface& gui = Game::getGUI();
		for (auto& i : bgWidget)
			gui.removeWidget(i);
		gui.removeWidget(loadTextWidget);
		gui.removeWidget(loadAnimWidget);
		isVisible = false;
	}

	void update()
	{
		if (!isVisible)
			return;

		Vec2ui area = Game::getFramebufferSize();
		float tickDuration = Game::getTickDuration();

		lastBgOffset = bgOffset;
		bgOffset -= i32(120 * tickDuration);
		if (bgOffset <= -i32(area.x))
		{
			bgOffset += i32(area.x);
			lastBgOffset += i32(area.x);
		}

		lastGearRotation = gearRotation;
		gearRotation += 1.2f * tickDuration;
	}

	void render()
	{
		if (!isVisible)
			return;

		Vec2ui area = Game::getFramebufferSize();

		i32 lerpedBgOffset = std::lroundl(Math::lerp(float(lastBgOffset), float(bgOffset), Game::getTickAlpha()));
		if (lerpedBgOffset > 0)
			lerpedBgOffset -= i32(area.x);

		bgWidget[0].bounds = RectI(lerpedBgOffset, 0, i32(area.x), i32(area.y));
		bgWidget[1].bounds = RectI(i32(area.x) + lerpedBgOffset, 0, i32(area.x), i32(area.y));

		float scale = float(area.x) + float(area.y);
		scale /= 560;

		u32 loadTextHeight = u32(32 * scale);
		loadTextWidget.bounds = RectI(16, i32(area.y - (loadTextHeight + 12)), i32(128 * scale), i32(loadTextHeight));

		float lerpedGearRotation = Math::lerp(lastGearRotation, gearRotation, Game::getTickAlpha());

		loadAnimWidget.bounds = RectI(i32(area.x - loadTextHeight - 32), i32(area.y - (loadTextHeight + 12)), i32(loadTextHeight), i32(loadTextHeight));
		loadAnimWidget.setRotation(Vec3f(0.0f, 0.0f, lerpedGearRotation));
	}
}