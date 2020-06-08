#pragma once

#include <glm/glm.hpp>
#include <functional>
#include <tsl/robin_set.h>
#include <tsl/robin_set.h>

#include "Anchor.h"

class Widget : public std::enable_shared_from_this<Widget>
{
public:
	virtual void Draw() = 0;
	virtual void Update(const glm::vec2& mousePosition);

	enum class CoordinatesType
	{
		PIXEL,
		RELATIVE,
		WORLD
	};

	void PreDraw();
	void PostDraw();
	void SetPosition(glm::vec3 position);
	void SetAnchor(Anchor anchor);
	void SetPositionAnchor(glm::vec3 position, Anchor anchor);
	void SetPivot(Anchor pivot);
	void SetSize(glm::vec2 size);
	const glm::vec3& GetPosition();
	const glm::vec3& GetAbsolutePosition();
	const glm::vec2& GetSize();
	const glm::vec2& GetLocalSize();
	Anchor GetAnchor();
	Anchor GetPivot();
	void SetZIndex(int zIndex);
	int GetZIndex();
	void AddChild(std::shared_ptr<Widget> child);
	void SetClipping(bool clipping);
	void SetEnabled(bool enabled);
	bool GetEnabled();
	void SetCoordinatesType(CoordinatesType type);
	void Recalculate();

	std::shared_ptr<Widget> parent;
	std::vector<std::shared_ptr<Widget>> children;
	tsl::robin_set<std::shared_ptr<Widget>> clippingWidgets;

protected:
	Widget();
	bool IsMouseOver(const glm::vec2& mousePosition);
	void CalculateAbsolutePosition();
	void UpdateClipping(bool clipping, std::shared_ptr<Widget> clippingWidget);
	int GetLevel(); // TODO: potrzebne?
	void SetLevel(int level);

private:
	void UpdateChildrenWorldPosition();
	glm::vec3 ToAbsolute(glm::vec3 vec, bool size = false);
	glm::vec2 ToAbsolute(glm::vec2 vec);

	Anchor anchor;
	Anchor pivot;

	glm::vec4 clippingArea;
	glm::vec3 position; //typed coordinates
	glm::vec2 size; //typed coordinates

	glm::vec3 absolutePosition; //world + local
	glm::vec3 worldPosition; //pixel
	glm::vec3 localPosition; //pixel
	glm::vec2 localSize; //final
	bool isClipping;
	bool isClipped;
	bool enabled;
	int level;
	CoordinatesType coordinatesType;
	int zIndex;
};
