#pragma once

#include <glm/glm.hpp>
#include "Object.hpp"

class Actor : public Object
{
public:
	Actor();
	~Actor() override;

	void create() override;
	void update() override;
	void render() override;
	void destroy() override;

	glm::vec3 position;
	glm::vec3 rotation;
	glm::vec3 scale;
	glm::vec3 prevPosition;
	glm::vec3 prevRotation;
	glm::vec3 prevScale;
};
