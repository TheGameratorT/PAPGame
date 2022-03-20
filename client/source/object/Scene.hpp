#pragma once

#include "Object.hpp"

class Scene : public Object
{
public:
	Scene();
	~Scene() override;

	void create() override;
	void update() override;
	void render() override;
	void destroy() override;
};
