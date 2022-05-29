#pragma once

#include "scene.hpp"
#include "render/texture.hpp"
#include "gui/image.hpp"

class TitleScene : public Scene
{
public:
	DECL_OBJECT(TitleScene)

	void onCreate() override;
	void onUpdate() override;
	void onRender() override;
	void onDestroy() override;
	void onDestroyRequest() override;

private:
	Texture bgTexture;
	mGUI::Image bgWidget;
};
