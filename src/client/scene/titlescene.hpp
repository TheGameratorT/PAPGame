#pragma once

#include "scene.hpp"

class TitleScene : public Scene
{
public:
	DECL_OBJECT(TitleScene)

	void onCreate() override;
	void onUpdate() override;
	void onRender() override;
	void onDestroy() override;
	void onDestroyRequest() override;
};
