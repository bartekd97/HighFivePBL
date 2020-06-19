#pragma once

#include <GLFW/glfw3.h>

namespace WindowManager
{
	void Initialize(const int& screenWidth, const int& screenHeight, const char* windowTitle);

	GLFWwindow* GetWindow();

	bool IsClosing();
	void Close();

	extern int SCREEN_WIDTH;
	extern int SCREEN_HEIGHT;
}
