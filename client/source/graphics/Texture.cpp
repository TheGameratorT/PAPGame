#include "Texture.hpp"

#include <glm/gtc/matrix_transform.hpp>

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
	// positions         // colors           // texture coords
	1.0f,  1.0f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // top right
	1.0f, -1.0f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // bottom right
	-1.0f, -1.0f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,  // bottom left
	-1.0f,  1.0f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f   // top left
};

static unsigned int QuadIndices[] = {
	0, 1, 3, // first triangle
	1, 2, 3  // second triangle
};

static GLE::TextureSourceFormat GLEColorFormatForImageColorFormat[] = {
	GLE::TextureSourceFormat::RGB,
	GLE::TextureSourceFormat::RGBA,
	GLE::TextureSourceFormat::BGR
};

static GLE::ShaderProgram QuadShaderProgram;
static GLE::Uniform QuadUniform;

void Texture::initProgram()
{
	QuadShaderProgram.create();
	QuadShaderProgram.addShader(QuadVertexShader, sizeof(QuadVertexShader), GLE::ShaderType::VertexShader);
	QuadShaderProgram.addShader(QuadFragmentShader, sizeof(QuadFragmentShader), GLE::ShaderType::FragmentShader);
	QuadShaderProgram.link();

	QuadShaderProgram.getUniform("textureSampler").setInt(0);

	QuadUniform = QuadShaderProgram.getUniform("mvp_mat");
}

void Texture::destroyProgram()
{
	QuadShaderProgram.destroy();
}

Texture::Texture() :
	position(0.0f, 0.0f, 0.0f),
	rotation(0.0f, 0.0f, 0.0f),
	scale(1.0f, 1.0f)
{}

void Texture::loadImage(const Image& image)
{
	vertexArray.create();
	vertexArray.bind();

	vertexBuffer.create();
	vertexBuffer.bind();
	vertexBuffer.allocate(sizeof(QuadVertices), QuadVertices, GLE::BufferAccess::StaticDraw);

	// position attribute
	vertexArray.setAttribute(0, 3, GLE::AttributeType::Float, 8 * sizeof(float), 0);
	vertexArray.enableAttribute(0);
	// color attribute
	vertexArray.setAttribute(1, 3, GLE::AttributeType::Float, 8 * sizeof(float), 3 * sizeof(float));
	vertexArray.enableAttribute(1);
	// texture coord attribute
	vertexArray.setAttribute(2, 2, GLE::AttributeType::Float, 8 * sizeof(float), 6 * sizeof(float));
	vertexArray.enableAttribute(2);

	indexBuffer.create();
	indexBuffer.bind();
	indexBuffer.allocate(sizeof(QuadIndices), QuadIndices, GLE::BufferAccess::StaticDraw);

	texture2D.create();
	texture2D.bind();

	texture2D.setData(
		image.getWidth(),
		image.getHeight(),
		GLE::ImageFormat::RGBA8,
		GLEColorFormatForImageColorFormat[static_cast<int>(image.getColorFormat())],
		GLE::TextureSourceType::UByte,
		image.getData()
	);

	texture2D.setWrapU(GLE::TextureWrap::Repeat);
	texture2D.setWrapV(GLE::TextureWrap::Repeat);
	texture2D.setMinFilter(GLE::TextureFilter::Trilinear, true);
	texture2D.setMagFilter(GLE::TextureFilter::Bilinear);

	texture2D.generateMipmaps();
}

void Texture::render()
{
	glm::mat4 matrix(1.0f);
	matrix = glm::scale(matrix, glm::vec3(scale.x, scale.y, 1.0f));
	matrix = glm::rotate(matrix, rotation.x, glm::normalize(glm::vec3(1.0f, 0.0f, 0.0f)));
	matrix = glm::rotate(matrix, rotation.y, glm::normalize(glm::vec3(0.0f, 1.0f, 0.0f)));
	matrix = glm::rotate(matrix, rotation.z, glm::normalize(glm::vec3(0.0f, 0.0f, 1.0f)));
	matrix = glm::translate(matrix, glm::vec3(position.x, position.y, position.z));

	QuadShaderProgram.start();
	QuadUniform.setMat4(matrix);
	vertexArray.bind();
	indexBuffer.bind();
	texture2D.activate(0);
	GLE::renderIndexed(GLE::PrimType::Triangle, GLE::IndexType::Int, 6, 0);
}

void Texture::destroy()
{
	vertexArray.destroy();
	vertexBuffer.destroy();
	indexBuffer.destroy();
	texture2D.destroy();
}
