#pragma once

#include <GLFW/glfw3.h>

typedef int GLFW_KEY;

namespace InputManager
{
	void Initialize();
	// call it once at the begin of every frame
	void PollEvents();

	bool GetKeyStatus(GLFW_KEY key);
	bool GetKeyUp(GLFW_KEY key);
	bool GetKeyDown(GLFW_KEY key);
}
