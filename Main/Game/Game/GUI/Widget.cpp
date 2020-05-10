#include "Widget.h"
#include "../WindowManager.h"

void Widget::Update(const glm::vec2& mousePosition)
{
	for (auto& child : children)
	{
		child->Update(mousePosition);
	}
}

void Widget::SetPosition(glm::vec3 position)
{
	this->position = position;
	CalculateAbsolutePosition();
	UpdateChildrenWorldPosition();
}

void Widget::SetAnchor(Anchor anchor)
{
	this->anchor = anchor;
	CalculateAbsolutePosition();
}

void Widget::SetPositionAnchor(glm::vec3 position, Anchor anchor)
{
	this->position = position;
	this->anchor = anchor;
	CalculateAbsolutePosition();
}

const glm::vec3& Widget::GetPosition()
{
	return position;
}

const glm::vec3& Widget::GetAbsolutePosition()
{
	return absolutePosition;
}

Anchor Widget::GetAnchor()
{
	return anchor;
}

void Widget::AddChild(std::shared_ptr<Widget> child)
{
	children.push_back(child);
	UpdateChildrenWorldPosition();
}

Widget::Widget()
{
	anchor = Anchor::TOPLEFT;
	worldPosition = glm::vec3(0.0f);
	position = glm::vec3(0.0f);
	absolutePosition = glm::vec3(0.0f);
	size = glm::vec2(0.0f);
	useWorldSpace = false;
}

bool Widget::IsMouseOver(const glm::vec2& mousePosition)
{
	if (mousePosition.x < absolutePosition.x) return false;
	if (mousePosition.x > (absolutePosition.x + size.x)) return false;
	if (mousePosition.y < absolutePosition.y) return false;
	if (mousePosition.y > (absolutePosition.y + size.y)) return false;
	return true;
}

void Widget::UpdateChildrenWorldPosition()
{
	for (auto& child : children)
	{
		child->worldPosition = absolutePosition;
		child->CalculateAbsolutePosition();
	}
}

void Widget::CalculateAbsolutePosition()
{
	// TODO: anchor
	// TODO: maybe percentage position?
	glm::vec2 parentSize(WindowManager::SCREEN_WIDTH, WindowManager::SCREEN_HEIGHT);
	absolutePosition = worldPosition;
	if (parent != nullptr)
	{
		parentSize = parent->size;
	}

	if (anchor == Anchor::TOPLEFT)
	{
		absolutePosition.x += position.x;
		absolutePosition.y += position.y;
	} else if (anchor == Anchor::CENTER)
	{
		absolutePosition.x += parentSize.x / 2.0f + position.x;
		absolutePosition.y += parentSize.y / 2.0f + position.y;
	} else if (anchor == Anchor::TOPRIGHT)
	{
		absolutePosition.x += parentSize.x - size.x - position.x;
		absolutePosition.y += position.y;
	} else if (anchor == Anchor::BOTTOMLEFT)
	{
		absolutePosition.x += position.x;
		absolutePosition.y += parentSize.y - size.y - position.y;
	} else if (anchor == Anchor::BOTTOMRIGHT)
	{
		absolutePosition.x += parentSize.x - size.x - position.x;
		absolutePosition.y += parentSize.y - size.y - position.y;
	}

}
