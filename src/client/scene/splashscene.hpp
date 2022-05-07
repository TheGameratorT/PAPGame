#pragma once

#include "scene.hpp"

#include "render/gle/texture2d.hpp"

#include "render/texture.hpp"

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
	int render = 0;
	int lastNetState = 0;
	Texture splashTexture1;
	Texture splashTexture2;
	Texture splashTexture3;
	Texture splashTexture4;
};
