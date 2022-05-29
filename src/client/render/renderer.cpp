#include "renderer.hpp"

#include "render/gle/gle.hpp"
#include "game.hpp"

static const char QuadVertexShader[] =
R"(#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aTexCoord;

uniform mat4 u_MVP;

out vec3 ourColor;
out vec2 TexCoord;

void main()
{
	gl_Position = u_MVP * vec4(aPos, 1.0);
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
uniform float opacity;

void main()
{
	FragColor = texture(textureSampler, TexCoord) * vec4(1.0, 1.0, 1.0, opacity);
}
)";

static float QuadVertices[] = {
	// Positions          // Colors            // Texture Coords
	 1.0f,  1.0f,  0.0f,   1.0f,  0.0f,  0.0f,  1.0f,  1.0f, // Top Right
	 1.0f, -1.0f,  0.0f,   0.0f,  1.0f,  0.0f,  1.0f,  0.0f, // Bottom Right
	-1.0f, -1.0f,  0.0f,   0.0f,  0.0f,  1.0f,  0.0f,  0.0f, // Bottom Left
	-1.0f,  1.0f,  0.0f,   1.0f,  1.0f,  0.0f,  0.0f,  1.0f  // Top Left
};

static unsigned int QuadIndices[] = {
	0, 1, 3, // First Triangle
	1, 2, 3  // Second Triangle
};

namespace Renderer
{
	static GLE::ShaderProgram quadShaderProgram;
	static GLE::Uniform quadUniformMVP;
	static GLE::Uniform quadUniformOpacity;
	static GLE::VertexArray quadVertexArray;
	static GLE::VertexBuffer quadVertexBuffer;
	static GLE::IndexBuffer quadIndexBuffer;
	static Mat4f projectionMatrix;

	void init()
	{
		quadShaderProgram.create();
		quadShaderProgram.addShader(QuadVertexShader, sizeof(QuadVertexShader), GLE::ShaderType::VertexShader);
		quadShaderProgram.addShader(QuadFragmentShader, sizeof(QuadFragmentShader), GLE::ShaderType::FragmentShader);
		quadShaderProgram.link();

		quadShaderProgram.getUniform("textureSampler").setInt(0);

		quadUniformMVP = quadShaderProgram.getUniform("u_MVP");
		quadUniformOpacity = quadShaderProgram.getUniform("opacity");

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

		GLE::enableBlending();
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		updateProjection();
	}

	void destroy()
	{
		quadVertexArray.destroy();
		quadVertexBuffer.destroy();
		quadIndexBuffer.destroy();
		quadShaderProgram.destroy();
	}

	void updateProjection()
	{
		Vec2ui fbs = Game::getFramebufferSize();
		projectionMatrix = Mat4f::ortho(0.0f, float(fbs.x) * 2, float(fbs.y) * 2, 0.0f, -1.0f, 1.0f);
	}

	void renderTexture(Texture& texture, const Vec3i& position, const Vec3f& rotation, const Vec3f& scale, float opacity)
	{
		Mat4f mtx = projectionMatrix;

		auto imgWidth = float(texture.getWidth());
		auto imgHeight = float(texture.getHeight());

		mtx.translate(
			((imgWidth / 2 * scale.x) + float(position.x)) * 2,
			((imgHeight / 2 * scale.y) + float(position.y)) * 2,
			position.z
		);
		mtx.rotate(Vec3f(1.0f, 0.0f, 0.0f), rotation.x);
		mtx.rotate(Vec3f(0.0f, 1.0f, 0.0f), rotation.y);
		mtx.rotate(Vec3f(0.0f, 0.0f, 1.0f), rotation.z);
		mtx *= Mat4f::fromScale(imgWidth * scale.x, imgHeight * scale.y, scale.z);

		quadShaderProgram.start();
		quadUniformMVP.setMat4(mtx);
		quadUniformOpacity.setFloat(opacity);
		quadVertexArray.bind();
		quadIndexBuffer.bind();
		texture.getGleTex2D().activate(0);
		GLE::renderIndexed(GLE::PrimType::Triangle, GLE::IndexType::Int, 6, 0);
	}

	void renderTexture(Texture& texture, const RectI& rectangle, const Vec3f& rotation, float opacity)
	{
		auto imgWidth = float(texture.getWidth());
		auto imgHeight = float(texture.getHeight());
		auto rectWidth = float(rectangle.getWidth());
		auto rectHeight = float(rectangle.getHeight());
		//Log::info("render img", "width: " + std::to_string(imgWidth) + ", height: " + std::to_string(imgHeight) + ", rectWidth: " + std::to_string(rectWidth) + ", rectHeight: " + std::to_string(rectHeight));
		Vec3f scale(rectWidth / imgWidth, rectHeight / imgHeight, 1.0f);
		renderTexture(texture, Vec3i(rectangle.getX(), rectangle.getY(), 0), rotation, scale, opacity);
	}
}
