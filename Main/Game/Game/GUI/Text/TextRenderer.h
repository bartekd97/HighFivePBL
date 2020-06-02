#pragma once

#include <string>
#include <memory>
#include <glm/glm.hpp>

#include "Font.h"

namespace TextRenderer
{
	void Initialize();
	void Terminate();
	void LoadFont(std::string name, std::string path);
	void SetFont(std::string fontName);
	std::shared_ptr<Font> GetCurrentFont();
	void RenderText(std::string text, float x, float y, float scale, glm::vec3 color);
	void UpdateProjection();
}
