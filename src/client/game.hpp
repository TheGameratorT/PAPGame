#pragma once

#include "window/window.hpp"
#include "time/timer.hpp"

#include "scene.hpp"

#include "gui/interface.hpp"
#include "network/clientconnection.hpp"
#include "input/keyhandle.hpp"
#include "input/keydefs.hpp"

namespace Game
{
	using KeyCallback = std::function<void(KeyState)>;

	extern mGUI::Interface gui;
	extern bool shouldQuitGame;
	extern float tickDuration;
	extern float frameDelta;
	extern float tickAlpha;
	extern Vec2ui framebufferSize;
	extern Vec2d cursorPosition;

	Network::ClientConnection* getConn();
	int gotConnErr();

	bool init();
	void run();
	void destroy();

	void reload();

	void update(double timeNow);
	void render();

	void setTickRate(u32 newTickRate);
	void setFrameRate(u32 newFrameRate);

	[[nodiscard]] inline float getTickDuration() { return tickDuration; }
	[[nodiscard]] inline float getFrameDelta() { return frameDelta; }
	[[nodiscard]] inline float getTickAlpha() { return tickAlpha; }
	[[nodiscard]] double getElapsedTime();
	[[nodiscard]] inline const Vec2ui& getFramebufferSize() { return framebufferSize; }
	[[nodiscard]] inline mGUI::Interface& getGUI() { return gui; }
	[[nodiscard]] inline Vec2d& getCursorPosition() { return cursorPosition; }

	void switchScene(const ObjectProfile* profile);

	template<SceneType T>
	void switchScene()
	{ switchScene(&T::profile); }

	Scene* createScene(int sceneID);

	template<SceneType T>
	T* createScene()
	{ return reinterpret_cast<T*>(createScene(Objects::idOf<T>())); }

	void setFullscreen(bool newValue);

	KeyHandle bindKey(Key key, const KeyCallback& callback);

	void unbindKey(KeyHandle handle);

	inline void quit()
	{ shouldQuitGame = true; }
}
