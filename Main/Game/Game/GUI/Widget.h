#pragma once

#include <glm/glm.hpp>
#include <functional>

#include "Anchor.h"

class Widget
{
public:
	virtual void Draw() = 0;
	virtual void Update(const glm::vec2& mousePosition)
	{
		for (auto& child : children)
		{
			child->Update(mousePosition);
		}
	}

	void SetPosition(glm::vec3 position)
	{
		this->position = position;
		CalculateAbsolutePosition();
		UpdateChildrenWorldPosition();
	}

	void SetAnchor(Anchor anchor) {}

	const glm::vec3& GetPosition()
	{
		return position;
	}

	const glm::vec3& GetAbsolutePosition()
	{
		return absolutePosition;
	}

	Anchor GetAnchor()
	{
		return anchor;
	}
	
	void AddChild(std::shared_ptr<Widget> child)
	{
		children.push_back(child);
		UpdateChildrenWorldPosition();
	}
	
	bool useWorldSpace; // w sensie 2d czy 3d
	glm::vec2 size; //width height

	std::shared_ptr<Widget> parent;
	std::vector<std::shared_ptr<Widget>> children;
protected:
	Widget()
	{
		anchor = Anchor::TOPLEFT;
		worldPosition = glm::vec3(0.0f);
		position = glm::vec3(0.0f);
		absolutePosition = glm::vec3(0.0f);
		size = glm::vec2(0.0f);
		useWorldSpace = false;
	}

	bool IsMouseOver(const glm::vec2& mousePosition)
	{
		if (mousePosition.x < absolutePosition.x) return false;
		if (mousePosition.x > (absolutePosition.x + size.x)) return false;
		if (mousePosition.y < absolutePosition.y) return false;
		if (mousePosition.y > (absolutePosition.y + size.y)) return false;
		return true;
	}

private:
	void UpdateChildrenWorldPosition()
	{
		for (auto& child : children)
		{
			child->worldPosition = absolutePosition;
			child->CalculateAbsolutePosition();
		}
	}

	void CalculateAbsolutePosition()
	{
		// TODO: anchor
		// TODO: maybe percentage position?
		absolutePosition = worldPosition + position;
	}

	Anchor anchor;

	glm::vec3 position; // relatywna pozycja anchor
	glm::vec3 absolutePosition;
	glm::vec3 worldPosition; // w sensie absolutna pozycja rodzica


};
