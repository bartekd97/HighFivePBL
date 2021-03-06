#pragma once

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

typedef int GLFW_KEY;
typedef int GLFW_MOUSE_BUTTON;

namespace InputManager
{
	void Initialize();
	// call it once at the begin of every frame
	void PollEvents();

	bool GetKeyStatus(GLFW_KEY key);
	bool GetKeyUp(GLFW_KEY key);
	bool GetKeyDown(GLFW_KEY key);

	bool GetMouseButtonUp(GLFW_MOUSE_BUTTON key);
	bool GetMouseButtonDown(GLFW_MOUSE_BUTTON key);

	const glm::vec2& GetMousePosition();
	int GetMouseButtonState(int button);
}
