#pragma once

#include <GLFW/glfw3.h>

namespace GLFW
{
	bool init();
	void terminate();

	inline void pollEvents()
	{ glfwPollEvents(); }
}
