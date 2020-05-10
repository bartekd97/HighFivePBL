#include "Label.h"
#include "Text/TextRenderer.h"
#include "../WindowManager.h"

Label::Label()
{
	this->text = "";
	SetFontSize(48); //TODO: ??
}

void Label::Draw()
{
	glm::vec3 color(0.0f);
	TextRenderer::RenderText(text, GetAbsolutePosition().x, WindowManager::SCREEN_HEIGHT - GetAbsolutePosition().y, 1.0f, color); // TODO: scale
}

void Label::Update(const glm::vec2& mousePosition)
{
	Widget::Update(mousePosition);
}


void Label::SetText(std::string text)
{
	this->text = text;
	// calc
}

void Label::SetFontSize(int size)
{
	fontSize = size;
	// calc
}

int Label::GetFontSize()
{
	return fontSize;
}
