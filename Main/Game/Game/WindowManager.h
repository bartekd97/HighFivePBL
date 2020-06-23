#pragma once

#include <string>
#include <GLFW/glfw3.h>

namespace WindowManager
{
	void Initialize(const int& screenWidth, const int& screenHeight, const std::string& windowTitle, bool fullscreen = false);

	GLFWwindow* GetWindow();

	bool IsClosing();
	void Close();

	extern int SCREEN_WIDTH;
	extern int SCREEN_HEIGHT;
	extern float DPI_FACTOR; // if dpi is set to 150%, then this value is 1.5f
}
