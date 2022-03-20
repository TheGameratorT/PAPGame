#pragma once

#include "../object/Scene.hpp"

class TitleScene : public Scene
{
public:
	DECL_OBJECT_PROFILE();

	TitleScene();
	~TitleScene() override;

	void create() override;
	void update() override;
	void render() override;
	void destroy() override;
};
