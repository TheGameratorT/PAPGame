#pragma once

#include "scene.hpp"

#include "render/gle/texture2d.hpp"

#include "render/texture.hpp"
#include "input/keyhandle.hpp"
#include "gui/image.hpp"

class SplashScene : public Scene
{
public:
	DECL_OBJECT(SplashScene)

	void onCreate() override;
	void onUpdate() override;
	void onRender() override;
	void onDestroy() override;
	void onDestroyRequest() override;

private:
	Texture gameByTexture;
	Texture logoTexture;
	mGUI::Image gameByWidget;
	mGUI::Image logoWidget;
	bool usingBiLinearLogo = false;
	bool switchingScene = false;
	float timer = 0;
};
