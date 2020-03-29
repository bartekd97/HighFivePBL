#include <glad/glad.h>

#include "WindowManager.h"
#include "Logger.h"

namespace WindowManager
{
	GLFWwindow* window = nullptr;

	void framebuffer_size_callback(GLFWwindow* window, int width, int height)
	{
		glViewport(0, 0, width, height);
	}

	void Initialize(const int& screenWidth, const int& screenHeight, const char* windowTitle)
	{
		if (window != nullptr)
		{
			LogError("WindowManager already initialized");
			return;
		}

		window = glfwCreateWindow(screenWidth, screenHeight, windowTitle, nullptr, nullptr);
		if (window != nullptr)
		{
			glfwMakeContextCurrent(window);
			glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
		}
	}

	GLFWwindow* GetWindow()
	{
		return window;
	}
}
