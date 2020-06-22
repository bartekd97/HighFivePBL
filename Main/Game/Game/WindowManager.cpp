#include <glad/glad.h>

#include "WindowManager.h"
#include "Utility/Logger.h"
#include "Event/EventManager.h"
#include "Event/Events.h"

namespace WindowManager
{
	GLFWwindow* window = nullptr;
	int SCREEN_WIDTH;
	int SCREEN_HEIGHT;
	float DPI_FACTOR;

	void framebuffer_size_callback(GLFWwindow* window, int width, int height)
	{
		//glViewport(0, 0, width, height);
		if (SCREEN_WIDTH != width || SCREEN_HEIGHT != height)
		{
			SCREEN_WIDTH = width;
			SCREEN_HEIGHT = height;
			Event ev(Events::General::WINDOW_RESIZE);
			EventManager::FireEvent(ev);
		}
	}

	void Initialize(const int& screenWidth, const int& screenHeight, const std::string& windowTitle, bool fullscreen)
	{
		if (window != nullptr)
		{
			LogError("WindowManager already initialized");
			return;
		}

		SCREEN_WIDTH = screenWidth;
		SCREEN_HEIGHT = screenHeight;

		GLFWmonitor* monitor = nullptr;
		if (fullscreen)
		{
			// "windowed full screen" mode

			monitor = glfwGetPrimaryMonitor();
			const GLFWvidmode* mode = glfwGetVideoMode(monitor);

			glfwWindowHint(GLFW_RED_BITS, mode->redBits);
			glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
			glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
			glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);
			SCREEN_WIDTH = mode->width;
			SCREEN_HEIGHT = mode->height;
		}

		window = glfwCreateWindow(SCREEN_WIDTH, screenHeight, windowTitle.c_str(), monitor, nullptr);
		if (window != nullptr)
		{
			glfwMakeContextCurrent(window);
			glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
			glfwSetWindowAspectRatio(window, SCREEN_WIDTH, SCREEN_HEIGHT);

			float scalex, scaley;
			glfwGetWindowContentScale(window, &scalex, &scaley);
			DPI_FACTOR = (scalex + scaley) * 0.5f;

			LogInfo("Window size: {}x{}", SCREEN_WIDTH, SCREEN_HEIGHT);
			LogInfo("DPI Factor: {}", DPI_FACTOR);
		}
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
