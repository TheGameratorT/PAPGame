#pragma once

#include "scene.hpp"
#include "render/texture.hpp"
#include "gui/image.hpp"
#include "gui/button.hpp"
#include "gui/textbox.hpp"
#include "gui/container.hpp"

class LobbyScene : public Scene
{
public:
	DECL_OBJECT(LobbyScene)

	void onCreate() override;
	void onUpdate() override;
	void onRender() override;
	void onDestroy() override;
	void onDestroyRequest() override;

private:
	mGUI::Container canvas;

};
