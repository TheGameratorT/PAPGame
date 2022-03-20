#include "Game.hpp"

#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <png.h>

#include "Utils.hpp"
#include "graphics/Image.hpp"
#include "object/Object.hpp"
#include "ObjectTable.hpp"
#include "object/Scene.hpp"
#include "scene/TitleScene.hpp"

constexpr int TICKS_PER_SECOND = 30;
constexpr float TICK_DURATION_SECONDS = 1.0f / TICKS_PER_SECOND;

constexpr int FRAMES_PER_SECOND = 60;
constexpr float FRAME_DURATION_SECONDS = 1.0f / FRAMES_PER_SECOND;

static void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	GLE::Framebuffer::setViewport(0, 0, width, height);
}

Game* Game::instance;

Game::Game() :
	frameDelta(0.0f),
	tickDelta(0.0f),
	fpsTimer(0.0f),
	currentFps(0),
	currentScene(nullptr),
	vSync(false),
	shouldQuitGame(false)
{}

Game::~Game() = default;

void Game::init()
{
	instance = this;

	if (!glfwInit())
		throw std::runtime_error("GLFW initialization failed");

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_SAMPLES, 4); // 4x antialiasing
#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	window = Window::create(640, 480, "PAP Game");
	window.makeContextCurrent();
	window.setFramebufferSizeCallback(framebuffer_size_callback);

	vSync = true;

	glfwSwapInterval(vSync); // V-SYNC

	if (!GLE::Core::init())
		throw std::runtime_error("GLE initialization failed");

	audio.init();
	audio.loadMusic("title", "assets/myheart.ogg", AudioContainer::OGG);

	switchScene<TitleScene>();
}

void Game::destroy()
{
	audio.destroy();
	window.destroy();
	glfwTerminate();
}

void Game::run()
{
	double lastTime = glfwGetTime();
	float tickTimer = 0.0f;

	while (!window.shouldClose() && !shouldQuitGame)
	{
		double timeNow = glfwGetTime();
		frameDelta = static_cast<float>(timeNow - lastTime);
		lastTime = timeNow;
		tickTimer += frameDelta;

		glfwPollEvents();

		while (tickTimer > TICK_DURATION_SECONDS)
		{
			update();
			if (tickTimer > (TICK_DURATION_SECONDS * 4)) // max of 4 updates in a run
			{
				std::cout << "[WARNING] The game ticker is under heavy load!\n"
							 "Discarding " << tickTimer << " seconds worth of updates." << std::endl;
				tickTimer = 0.0f;
				break;
			}
			else
			{
				tickTimer -= TICK_DURATION_SECONDS;
			}
		}

		tickDelta = tickTimer / TICK_DURATION_SECONDS;
		render();

		window.swapBuffers();

		if (!vSync)
		{
			double nextFrameTime = timeNow + FRAME_DURATION_SECONDS;
			while(glfwGetTime() < nextFrameTime);
		}
	}
}

void Game::update()
{
	fpsTimer += TICK_DURATION_SECONDS;
	if (fpsTimer >= (1.0f / 4.0f))
	{
		currentFps = std::lround(1.0f / frameDelta);
		window.setTitle((std::string("PAP Game | FPS: ") + std::to_string(currentFps)).c_str());
		fpsTimer = 0.0f;
	}

	for (Object* object : objects)
	{
		object->update();
	}
}

void Game::render()
{
	GLE::setClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	GLE::clear(GLE::ClearBuffer::Color);

	for (Object* object : objects)
	{
		object->render();
	}
}

Object* Game::spawnObject(const ObjectProfile* profile)
{
	Object* newObj = profile->ctor();
	newObj->create();
	objects.push_back(newObj);
	return newObj;
}

Scene* Game::switchScene(const ObjectProfile* profile)
{
	for (Object* object : objects)
	{
		if (object != currentScene)
			object->destroy();
	}
	if (currentScene != nullptr)
		currentScene->destroy();
	currentScene = reinterpret_cast<Scene*>(spawnObject(profile));
	return currentScene;
}
