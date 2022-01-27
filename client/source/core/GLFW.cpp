#include "GLFW.hpp"

#include <iostream>

namespace GLFW
{
	bool init()
	{
		if (!glfwInit())
		{
			std::cout << "ERROR: GLFW initialization failed." << std::endl;
			return false;
		}

		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		return true;
	}
}
