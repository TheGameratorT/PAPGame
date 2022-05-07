#include "renderer.hpp"

#include "render/gle/gle.hpp"

static const char QuadVertexShader[] =
R"(#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aTexCoord;

uniform mat4 mvp_mat;

out vec3 ourColor;
out vec2 TexCoord;

void main()
{
	gl_Position = mvp_mat * vec4(aPos, 1.0);
    ourColor = aColor;
    TexCoord = aTexCoord;
}
)";

static const char QuadFragmentShader[] =
R"(#version 330 core

out vec4 FragColor;

in vec3 ourColor;
in vec2 TexCoord;

uniform sampler2D textureSampler;

void main()
{
	FragColor = texture(textureSampler, TexCoord);
}
)";

static float QuadVertices[] = {
	// Positions          // Colors            // Texture Coords
	 1.0f,  1.0f,  0.0f,   1.0f,  0.0f,  0.0f,  1.0f,  0.0f, // Top Right
	 1.0f, -1.0f,  0.0f,   0.0f,  1.0f,  0.0f,  1.0f,  1.0f, // Bottom Right
	-1.0f, -1.0f,  0.0f,   0.0f,  0.0f,  1.0f,  0.0f,  1.0f, // Bottom Left
	-1.0f,  1.0f,  0.0f,   1.0f,  1.0f,  0.0f,  0.0f,  0.0f  // Top Left
};

static unsigned int QuadIndices[] = {
	0, 1, 3, // First Triangle
	1, 2, 3  // Second Triangle
};

namespace Renderer
{
	static GLE::ShaderProgram quadShaderProgram;
	static GLE::Uniform quadUniform;
	static GLE::VertexArray quadVertexArray;
	static GLE::VertexBuffer quadVertexBuffer;
	static GLE::IndexBuffer quadIndexBuffer;

	void init()
	{
		quadShaderProgram.create();
		quadShaderProgram.addShader(QuadVertexShader, sizeof(QuadVertexShader), GLE::ShaderType::VertexShader);
		quadShaderProgram.addShader(QuadFragmentShader, sizeof(QuadFragmentShader), GLE::ShaderType::FragmentShader);
		quadShaderProgram.link();

		quadShaderProgram.getUniform("textureSampler").setInt(0);

		quadUniform = quadShaderProgram.getUniform("mvp_mat");

		quadVertexArray.create();
		quadVertexArray.bind();

		quadVertexBuffer.create();
		quadVertexBuffer.bind();
		quadVertexBuffer.allocate(sizeof(QuadVertices), QuadVertices, GLE::BufferAccess::StaticDraw);

		// Position Attribute
		quadVertexArray.setAttribute(0, 3, GLE::AttributeType::Float, 8 * sizeof(float), 0);
		quadVertexArray.enableAttribute(0);
		// Color Attribute
		quadVertexArray.setAttribute(1, 3, GLE::AttributeType::Float, 8 * sizeof(float), 3 * sizeof(float));
		quadVertexArray.enableAttribute(1);
		// Texture Coord Attribute
		quadVertexArray.setAttribute(2, 2, GLE::AttributeType::Float, 8 * sizeof(float), 6 * sizeof(float));
		quadVertexArray.enableAttribute(2);

		quadIndexBuffer.create();
		quadIndexBuffer.bind();
		quadIndexBuffer.allocate(sizeof(QuadIndices), QuadIndices, GLE::BufferAccess::StaticDraw);
	}

	void destroy()
	{
		quadVertexArray.destroy();
		quadVertexBuffer.destroy();
		quadIndexBuffer.destroy();
		quadShaderProgram.destroy();
	}

	void renderTexture(Texture& texture, const Mat4f& matrix)
	{
		quadShaderProgram.start();
		quadUniform.setMat4(matrix);
		quadVertexArray.bind();
		quadIndexBuffer.bind();
		texture.getGleTex2D().activate(0);
		GLE::renderIndexed(GLE::PrimType::Triangle, GLE::IndexType::Int, 6, 0);
	}
}
