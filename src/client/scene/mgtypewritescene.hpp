#pragma once

#include "scene.hpp"

#include "gui/container.hpp"

class MGTypeWriteScene : public Scene
{
public:
	DECL_OBJECT(MGTypeWriteScene)

	void onCreate() override;
	void onUpdate() override;
	void onRender() override;
	void onDestroy() override;
	void onDestroyRequest() override;

private:
	mGUI::Container canvas;

	bool switchingScene = false;
};
