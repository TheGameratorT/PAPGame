#pragma once

#include "window/window.hpp"
#include "time/timer.hpp"

#include "scene.hpp"

#include "gui/interface.hpp"
#include "network/clientconnection.hpp"
#include "input/keyhandle.hpp"
#include "input/keydefs.hpp"

#include "font/truetype/truetype.hpp"
#include "locale/unicodestring.hpp"

namespace Game
{
	using KeyCallback = std::function<void(KeyState)>;
	using TaskCallback = std::function<void()>;

	extern mGUI::Interface gui;
	extern bool shouldQuitGame;
	extern float tickDuration;
	extern float frameDelta;
	extern float tickAlpha;
	extern Vec2ui framebufferSize;
	extern Vec2d cursorPosition;

	bool init();
	void run();
	void destroy();

	inline void quit() { shouldQuitGame = true; }

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

	std::string getClipboard();

	void switchScene(const ObjectProfile* profile);

	template<SceneType T>
	void switchScene() { switchScene(&T::profile); }

	Scene* createScene(int sceneID);

	template<SceneType T>
	T* createScene() { return reinterpret_cast<T*>(createScene(Objects::idOf<T>())); }

	void setFullscreen(bool newValue);

	KeyHandle bindKey(Key key, const KeyCallback& callback);

	void unbindKey(KeyHandle handle);

	void loadFont(const std::string& fontName, const Path& fontPath);
	const TrueType::Font& getFont(const std::string& fontName);

	void connect(const std::string& address, u16 port);

	void schedule(const TaskCallback& callback, u32 delayMs);

	void setPlayerName(const U8String& name);
}
