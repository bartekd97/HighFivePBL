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

void Widget::SetPivot(Anchor pivot)
{
	this->pivot = pivot;
	CalculateAbsolutePosition();
}

void Widget::SetSize(glm::vec2 size)
{
	this->size = size;
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

const glm::vec2& Widget::GetSize()
{
	return size;
}

Anchor Widget::GetAnchor()
{
	return anchor;
}

Anchor Widget::GetPivot()
{
	return pivot;
}

void Widget::AddChild(std::shared_ptr<Widget> child)
{
	children.push_back(child);
	UpdateChildrenWorldPosition();
}

Widget::Widget()
{
	anchor = Anchor::TOPLEFT;
	pivot = Anchor::TOPLEFT;
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
	// TODO: maybe percentage position?
	glm::vec2 parentSize(WindowManager::SCREEN_WIDTH, WindowManager::SCREEN_HEIGHT);
	absolutePosition = worldPosition + position;
	if (parent != nullptr)
	{
		parentSize = parent->size;
	}

	if (anchor == Anchor::CENTER)
	{
		absolutePosition.x += parentSize.x / 2.0f;
		absolutePosition.y += parentSize.y / 2.0f;
	}
	else
	{
		if (anchor == Anchor::TOPRIGHT || anchor == Anchor::BOTTOMRIGHT)
		{
			absolutePosition.x += parentSize.x;
		}
		if (anchor == Anchor::BOTTOMLEFT || anchor == Anchor::BOTTOMRIGHT)
		{
			absolutePosition.y += parentSize.y;
		}
	}

	if (pivot == Anchor::CENTER)
	{
		absolutePosition.x -= size.x / 2.0f;
		absolutePosition.y -= size.y / 2.0f;
	}
	else
	{
		if (pivot == Anchor::TOPRIGHT || pivot == Anchor::BOTTOMRIGHT)
		{
			absolutePosition.x -= size.x;
		}
		if (pivot == Anchor::BOTTOMLEFT || pivot == Anchor::BOTTOMRIGHT)
		{
			absolutePosition.y -= size.y;
		}
	}
}
