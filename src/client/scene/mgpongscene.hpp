#pragma once

#include "scene.hpp"

#include "gui/container.hpp"
#include "gui/image.hpp"
#include "gui/textbox.hpp"
#include "render/texture.hpp"

class MGPongScene : public Scene
{
public:
	DECL_OBJECT(MGPongScene)

	void onCreate() override;
	void onUpdate() override;
	void onRender() override;
	void onDestroy() override;
	void onDestroyRequest() override;
	void onConnectionLost() override;

private:
	mGUI::Container canvas;

	bool switchingScene = false;


};
