#include "Label.h"
#include "Text/TextRenderer.h"
#include "../WindowManager.h"

Label::Label()
{
	this->text = "";
	SetFontSize(48); //TODO: ??
	color = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
}

void Label::Draw()
{
	TextRenderer::RenderText(text, GetAbsolutePosition().x, WindowManager::SCREEN_HEIGHT - GetAbsolutePosition().y - GetLocalSize().y, ((float)fontSize / TextRenderer::GetCurrentFont()->GetSize()) * WindowManager::DPI_FACTOR, color);
}

void Label::Update(const glm::vec2& mousePosition)
{
	Widget::Update(mousePosition);
}

void Label::SetText(std::string text)
{
	this->text = text;
	CalculateSize();
}

void Label::SetFontSize(int size)
{
	fontSize = size;
	CalculateSize();
}

int Label::GetFontSize()
{
	return fontSize;
}

void Label::CalculateSize()
{
	float scale = (float)fontSize / TextRenderer::GetCurrentFont()->GetSize();
	auto currentFont = TextRenderer::GetCurrentFont();

	glm::vec2 tmpSize = glm::vec2(0.0f);

	float tmp;
	std::string::const_iterator c;
	for (c = text.begin(); c != text.end(); c++)
	{
		Character ch = currentFont->GetCharacter(*c);
		
		tmpSize.x += (ch.Advance >> 6)* scale;
		tmp = ch.Size.y * scale;
		if (tmp > tmpSize.y)
		{
			tmpSize.y = tmp;
		}
	}

	if (GetCoordinatesType() == Widget::CoordinatesType::RELATIVE)
	{
		glm::vec2 parentSize(WindowManager::SCREEN_WIDTH, WindowManager::SCREEN_HEIGHT);
		if (parent != nullptr)
		{
			parentSize = parent->GetLocalSize();
		}

		tmpSize /= parentSize;
	}

	SetSize(tmpSize);
}
