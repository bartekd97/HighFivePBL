#include <glad/glad.h>

#include "WindowManager.h"
#include "Utility/Logger.h"

namespace WindowManager
{
	GLFWwindow* window = nullptr;
	int SCREEN_WIDTH;
	int SCREEN_HEIGHT;

	void framebuffer_size_callback(GLFWwindow* window, int width, int height)
	{
		//glViewport(0, 0, width, height);
		SCREEN_WIDTH = width;
		SCREEN_HEIGHT = height;
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

		SCREEN_WIDTH = screenWidth;
		SCREEN_HEIGHT = screenHeight;
	}

	GLFWwindow* GetWindow()
	{
		return window;
	}

	bool IsClosing()
	{
		return glfwWindowShouldClose(window);
	}
	void Close()
	{
		glfwSetWindowShouldClose(window, GLFW_TRUE);
	}
}
