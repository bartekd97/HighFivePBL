#pragma once

#include <glm/glm.hpp>
#include <functional>

#include "Anchor.h"

class Widget
{
public:
	virtual void Draw() = 0;
	virtual void Update(const glm::vec2& mousePosition) = 0;

	void SetPosition(glm::vec3 position)
	{
		this->position = position;
	}

	void SetAnchor(Anchor anchor) {}

	const glm::vec3& GetPosition()
	{
		return position;
	}

	Anchor GetAnchor()
	{
		return anchor;
	}
	
	bool useWorldSpace; // w sensie 2d czy 3d
	glm::vec2 size; //width height

	std::shared_ptr<Widget> parent;
	std::vector<std::shared_ptr<Widget>> children;
protected:
	Widget()
	{
		anchor = Anchor::TOPLEFT;
	}

	bool IsMouseOver(const glm::vec2& mousePosition)
	{
		if (mousePosition.x < position.x) return false;
		if (mousePosition.x > (position.x + size.x)) return false;
		if (mousePosition.y < position.y) return false;
		if (mousePosition.y > (position.y + size.y)) return false;
		return true;
	}

private:
	glm::vec3 position; // relatywna z anchorem
	Anchor anchor;

	glm::vec3 localPosition; // relatywna pozycja dziecka
	glm::vec3 worldPosition; // w sensie absolutna pozycja rodzica
};
