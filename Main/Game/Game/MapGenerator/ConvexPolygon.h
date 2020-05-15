#pragma once
#include <vector>
#include <glm/glm.hpp>

class ConvexPolygon
{
public:
	std::vector<glm::vec2> Points = std::vector<glm::vec2>(3);

	ConvexPolygon() {}
	ConvexPolygon(std::vector<glm::vec2>& points) : Points(points) {}

	ConvexPolygon CreateCircular(int segments, int precision = 4);

	bool IsPointInside(glm::vec2 point);
	float GetEdgeCenterRatio(glm::vec2 point, int precision = 5);

	ConvexPolygon ShellScaledBy(float mult);
	ConvexPolygon ScaledBy(float mult);
	ConvexPolygon BeveledVerticesBy(float mult);
};

