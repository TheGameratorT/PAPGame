#pragma once

#include <string>
#include <GLFW/glfw3.h>

class Window
{
public:
	constexpr Window() :
		m_window(nullptr)
	{}

	explicit constexpr Window(GLFWwindow* window) :
		m_window(window)
	{}

	inline static Window create(int width, int height, const char* title)
	{
		GLFWwindow* window = glfwCreateWindow(width, height, title, nullptr, nullptr);
		return Window(window);
	}

	inline void swapBuffers()
	{ glfwSwapBuffers(m_window); }

	inline GLFWframebuffersizefun setFramebufferSizeCallback(GLFWframebuffersizefun callback)
	{ return glfwSetFramebufferSizeCallback(m_window, callback); }

	inline void setTitle(const char* title)
	{ glfwSetWindowTitle(m_window, title); }

	inline void makeContextCurrent()
	{ glfwMakeContextCurrent(m_window); }

	[[nodiscard]] inline bool shouldClose()
	{ return glfwWindowShouldClose(m_window); }

	inline void destroy()
	{ glfwDestroyWindow(m_window); }

private:
	GLFWwindow* m_window;
};
