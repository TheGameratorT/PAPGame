#pragma once

#include "graphics/gle/gle.hpp"
#include <GLFW/glfw3.h>
#include <memory>
#include <vector>

#include "graphics/Window.hpp"
#include "input/InputHolder.hpp"
#include "audio/GameAudio.hpp"
#include "graphics/Renderer.hpp"
#include "graphics/Texture.hpp"
#include "object/Object.hpp"

class Scene;

class Game
{
public:
	inline static Game& getInstance()
	{ return *instance; }

	Game();
	~Game();

	void init();
	void destroy();

	void run();
	void update();
	void render();

	template<class T> requires std::is_base_of_v<Object, T>
	T* spawnObject()
	{ return reinterpret_cast<T*>(spawnObject(&T::profile)); }

	Object* spawnObject(const ObjectProfile* profile);

	template<class T> requires std::is_base_of_v<Scene, T>
	T* switchScene()
	{ return reinterpret_cast<T*>(switchScene(&T::profile)); }

	Scene* switchScene(const ObjectProfile* profile);

	[[nodiscard]] constexpr bool getQuitGame() const { return shouldQuitGame; };
	constexpr void quit() { shouldQuitGame = true; };

	[[nodiscard]] constexpr GameAudio& getAudio()
	{ return audio; }

private:
	static Game* instance;

	Window window;
	GameAudio audio;
	Renderer renderer;
	bool vSync;
	bool shouldQuitGame;

	float frameDelta;
	float tickDelta;
	float fpsTimer;
	int currentFps;

	Scene* currentScene;
	std::vector<Object*> objects;
};
