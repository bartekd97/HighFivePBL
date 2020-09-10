#include <glm/glm.hpp>
#include "SceneUtilities.h"
#include "WindowManager.h"

float SceneUtilities::CalculateGameplayCameraSizeFromWindowSize()
{
	float a2 = glm::pow(0.01f * WindowManager::SCREEN_WIDTH, 2.0f);
	float b2 = glm::pow(0.01f * WindowManager::SCREEN_HEIGHT, 2.0f);
	float c = glm::sqrt(a2 + b2);
	return (c / WindowManager::DPI_FACTOR) * 1.15f; // returns ~13.5f for 1280x720 at 125% dpi and ~16.9f for 100%
}
