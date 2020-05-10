#pragma once

#include "Widget.h"

class Label : public Widget
{
public:
	Label();
	void Draw() override;
	void Update(const glm::vec2& mousePosition) override;

	void SetText(std::string text);
	void SetFontSize(int size);
	int GetFontSize();
private:
	std::string text;
	int fontSize;
};
