#include <algorithm>
#include <glad/glad.h>

#include "Widget.h"
#include "../WindowManager.h"

void Widget::Update(const glm::vec2& mousePosition)
{
	if (clippingWidgets.size() > 0)
	{
		float maxLeft = -1, maxTop = -1, minRight = std::numeric_limits<float>::max(), minBottom = std::numeric_limits<float>::max();

		for (auto widget : clippingWidgets)
		{
			maxLeft = std::max(maxLeft, widget->GetAbsolutePosition().x);
			maxTop = std::max(maxTop, widget->GetAbsolutePosition().y);
			minRight = std::min(minRight, widget->GetAbsolutePosition().x + widget->GetSize().x);
			minBottom = std::min(minBottom, widget->GetAbsolutePosition().y + widget->GetSize().y);
		}

		if (maxLeft <= minRight && maxTop <= minBottom)
		{
			isClipped = true;
			clippingArea = glm::vec4(maxLeft, maxTop, minRight - maxLeft, minBottom - maxTop);
		}
		else
		{
			isClipped = false;
		}
	}
	else
	{
		isClipped = false;
	}

	for (auto& child : children)
	{
		if (child->GetEnabled())
		{
			child->Update(mousePosition);
		}
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
	child->SetLevel(GetLevel() + 1);
	child->clippingWidgets = clippingWidgets;
	if (isClipping)
	{
		child->UpdateClipping(true, shared_from_this());
	}
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
	isClipping = false;
	enabled = true;
	level = 0;
}

bool Widget::IsMouseOver(const glm::vec2& mousePosition)
{
	if (mousePosition.x < absolutePosition.x) return false;
	if (mousePosition.x > (absolutePosition.x + size.x)) return false;
	if (mousePosition.y < absolutePosition.y) return false;
	if (mousePosition.y > (absolutePosition.y + size.y)) return false;
	if (isClipped)
	{
		if (mousePosition.x < clippingArea.x
			|| mousePosition.x > (clippingArea.x + clippingArea.z)
			|| mousePosition.y < clippingArea.y
			|| mousePosition.y > (clippingArea.y + clippingArea.w)) return false;
	}
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

void Widget::SetClipping(bool clipping)
{
	bool changed = (isClipping != clipping);
	if (changed)
	{
		isClipping = !isClipping;
		for (auto& child : children)
		{
			child->UpdateClipping(clipping, shared_from_this());
		}
	}
}

void Widget::UpdateClipping(bool clipping, std::shared_ptr<Widget> clippingWidget)
{
	auto it = clippingWidgets.find(clippingWidget);
	if (clipping)
	{
		if (it == clippingWidgets.end())
		{
			clippingWidgets.insert(clippingWidget);
		}
	}
	else
	{
		if (it != clippingWidgets.end())
		{
			clippingWidgets.erase(it);
		}
	}
	for (auto& child : children)
	{
		child->UpdateClipping(clipping, clippingWidget);
	}
}

int Widget::GetLevel()
{
	return level;
}

void Widget::SetLevel(int level)
{
	this->level = level;
}

void Widget::PreDraw()
{
	if (isClipped)
	{
		glScissor(clippingArea.x, WindowManager::SCREEN_HEIGHT - clippingArea.y - clippingArea.w, clippingArea.z, clippingArea.w);
		glEnable(GL_SCISSOR_TEST);
	}
}

void Widget::PostDraw()
{
	glDisable(GL_SCISSOR_TEST);
}

void Widget::SetEnabled(bool enabled)
{
	if (this->enabled != enabled)
	{
		this->enabled = enabled;
		for (auto& child : children)
		{
			child->SetEnabled(enabled);
		}
	}
}

bool Widget::GetEnabled()
{
	return enabled;
}
