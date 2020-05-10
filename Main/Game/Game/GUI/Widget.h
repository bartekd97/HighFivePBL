#pragma once

#include <glm/glm.hpp>
#include <functional>

#include "Anchor.h"

class Widget
{
public:
	virtual void Draw() = 0;
	virtual void Update(const glm::vec2& mousePosition);

	void SetPosition(glm::vec3 position);
	void SetAnchor(Anchor anchor);
	void SetPositionAnchor(glm::vec3 position, Anchor anchor);
	const glm::vec3& GetPosition();
	const glm::vec3& GetAbsolutePosition();
	Anchor GetAnchor();
	void AddChild(std::shared_ptr<Widget> child);
	
	bool useWorldSpace; // w sensie 2d czy 3d
	glm::vec2 size; //width height

	std::shared_ptr<Widget> parent;
	std::vector<std::shared_ptr<Widget>> children;

protected:
	Widget();
	bool IsMouseOver(const glm::vec2& mousePosition);

private:
	void UpdateChildrenWorldPosition();
	void CalculateAbsolutePosition();

	Anchor anchor;

	glm::vec3 position; // relatywna pozycja anchor
	glm::vec3 absolutePosition;
	glm::vec3 worldPosition; // w sensie absolutna pozycja rodzica


};
