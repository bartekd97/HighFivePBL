#include <map>
#include "InputManager.h"
#include "WindowManager.h"
#include "Utility/Logger.h"

namespace InputManager
{
	GLFWwindow* window = nullptr;
	std::map<int, bool> currentlyPressed;
	std::map<int, bool> currentlyReleased;
	glm::vec2 mousePosition;

	void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
	{
		if (action == GLFW_PRESS)
			currentlyPressed[key] = true;
		else if (action == GLFW_RELEASE)
			currentlyReleased[key] = true;
	}

	void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
	{
		mousePosition.x = xpos;
		mousePosition.y = ypos;
	}

	void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
	{
		if (action == GLFW_PRESS)
			currentlyPressed[button] = true;
		else if (action == GLFW_RELEASE)
			currentlyReleased[button] = true;
	}

	void Initialize()
	{
		if (window != nullptr)
		{
			LogWarning("InputManager::Initialize(): Already initialized");
			return;
		}

		window = WindowManager::GetWindow();
		if (window == nullptr)
		{
			LogError("InputManager::Initialize(): WindowManager must be initialized first");
			return;
		}
		
		// register callbacks
		glfwSetKeyCallback(window, key_callback);
		glfwSetCursorPosCallback(window, cursor_position_callback);
		glfwSetMouseButtonCallback(window, mouse_button_callback);
	}

	void PollEvents()
	{
		currentlyPressed.clear();
		currentlyReleased.clear();
		glfwPollEvents();
	}


	bool GetKeyStatus(GLFW_KEY key)
	{
		return glfwGetKey(window, key) == GLFW_PRESS;
	}

	bool GetKeyUp(GLFW_KEY key)
	{
		return currentlyReleased.find(key) != currentlyReleased.end();
	}
	bool GetKeyDown(GLFW_KEY key)
	{
		return currentlyPressed.find(key) != currentlyPressed.end();
	}

	bool GetMouseButtonUp(GLFW_MOUSE_BUTTON key)
	{
		return currentlyReleased.find(key) != currentlyReleased.end();
	}
	bool GetMouseButtonDown(GLFW_MOUSE_BUTTON key)
	{
		return currentlyPressed.find(key) != currentlyPressed.end();
	}

	const glm::vec2& GetMousePosition()
	{
		return mousePosition;
	}

	int GetMouseButtonState(int button)
	{
		// TODO: assert?
		//return mouseButtonStates[button];
		return glfwGetMouseButton(window, button);
	}
}
