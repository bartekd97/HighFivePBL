#pragma once

#include <glm/glm.hpp>
#include <functional>

#include "Anchor.h"

//TODO: obcinanie? mo¿e zmienna, a potem sprawdzanie czy parent ma j¹ ustawion¹ i wtedy opengl scissors
class Widget
{
public:
	virtual void Draw() = 0;
	virtual void Update(const glm::vec2& mousePosition);

	void SetPosition(glm::vec3 position);
	void SetAnchor(Anchor anchor);
	void SetPositionAnchor(glm::vec3 position, Anchor anchor);
	void SetPivot(Anchor pivot);
	void SetSize(glm::vec2 size);
	const glm::vec3& GetPosition();
	const glm::vec3& GetAbsolutePosition();
	const glm::vec2& GetSize();
	Anchor GetAnchor();
	Anchor GetPivot();
	void AddChild(std::shared_ptr<Widget> child);
	
	bool useWorldSpace;

	std::shared_ptr<Widget> parent;
	std::vector<std::shared_ptr<Widget>> children;

protected:
	Widget();
	bool IsMouseOver(const glm::vec2& mousePosition);
	void CalculateAbsolutePosition();

private:
	void UpdateChildrenWorldPosition();

	Anchor anchor;
	Anchor pivot;

	glm::vec3 position;
	glm::vec3 absolutePosition;
	glm::vec3 worldPosition;
	glm::vec2 size;
};
