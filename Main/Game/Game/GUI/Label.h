#pragma once

#include "Widget.h"

// TODO: possibility of setting font instead of default one?
class Label : public Widget
{
public:
	Label();
	void Draw() override;
	void Update(const glm::vec2& mousePosition) override;

	void SetText(std::string text);
	void SetFontSize(int size);
	int GetFontSize();

	glm::vec4 color;
private:
	void CalculateSize();

	std::string text;
	int fontSize;
};
