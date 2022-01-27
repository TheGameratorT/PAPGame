#pragma once

#include <GLFW/glfw3.h>

namespace GLFW
{
	bool init();

	inline void terminate()
	{ glfwTerminate(); }

	inline void pollEvents()
	{ glfwPollEvents(); }
}
