#pragma once

#include <string>
#include <GLFW/glfw3.h>

class Window
{
public:
	inline ~Window()
	{ glfwDestroyWindow(m_window); }

	inline static Window create(int width, int height, const char* title)
	{ return {width, height, title}; }

	inline void swapBuffers()
	{ glfwSwapBuffers(m_window); }

	inline void setTitle(const char* title)
	{ glfwSetWindowTitle(m_window, title); }

	inline void makeContextCurrent()
	{ glfwMakeContextCurrent(m_window); }

	[[nodiscard]] inline bool shouldClose()
	{ return glfwWindowShouldClose(m_window); }

private:
	inline Window(int width, int height, const char* title)
	{ m_window = glfwCreateWindow(width, height, title, nullptr, nullptr); }

	GLFWwindow* m_window;
};

#ifdef UNUSED___

#pragma once

#include <string>
#include <GLFW/glfw3.h>

class Window
{
public:
	Window() = default;

	inline ~Window()
	{
		if (m_window != nullptr)
			glfwDestroyWindow(m_window);
	}

	inline void init(int width, int height, const char* title)
	{ m_window = glfwCreateWindow(width, height, title, nullptr, nullptr); }

	inline void swapBuffers()
	{ glfwSwapBuffers(m_window); }

	inline void setTitle(const char* title)
	{ glfwSetWindowTitle(m_window, title); }

	inline void makeContextCurrent()
	{ glfwMakeContextCurrent(m_window); }

	[[nodiscard]] inline bool shouldClose()
	{ return glfwWindowShouldClose(m_window); }

private:
	GLFWwindow* m_window = nullptr;
};

#endif
