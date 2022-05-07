#pragma once

#include "window/window.hpp"
#include "time/timer.hpp"

#include "scene.hpp"

#include "network/clientconnection.hpp"

namespace Game
{
	Network::ClientConnection* getConn();
	int gotConnErr();

	bool init();
	void run();
	void destroy();

	void update(double timeNow);
	void render();

	void setTickRate(u32 newTickRate);
	void setFrameRate(u32 newFrameRate);

	double getElapsedTime();

	void switchScene(const ObjectProfile* profile);

	template<SceneType T>
	void switchScene()
	{ switchScene(&T::profile); }

	Scene* createScene(int sceneID);

	template<SceneType T>
	T* createScene()
	{ return reinterpret_cast<T*>(createScene(Objects::idOf<T>())); }

	void setFullscreen(bool newValue);
}
