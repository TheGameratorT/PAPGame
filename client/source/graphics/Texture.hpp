#pragma once

#include "gle/gle.hpp"

#include "Image.hpp"

class Texture
{
public:
	Texture();

	static void initProgram();
	static void destroyProgram();

	void loadImage(const Image& image);
	void render();
	void destroy();

	glm::vec3 position;
	glm::vec3 rotation;
	glm::vec2 scale;

private:
	GLE::VertexArray vertexArray;
	GLE::VertexBuffer vertexBuffer;
	GLE::IndexBuffer indexBuffer;
	GLE::Texture2D texture2D;
};
