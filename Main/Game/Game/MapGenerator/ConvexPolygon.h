#pragma once
#include <vector>
#include <glm/glm.hpp>

class ConvexPolygon
{
private:
	struct PointAngle
	{
		glm::vec2 point;
		float angle;
	};
	std::vector<PointAngle> pointAngles;

	void CalculatePointAngles();

public:
	std::vector<glm::vec2> Points = std::vector<glm::vec2>(3);

	ConvexPolygon() { CalculatePointAngles(); }
	ConvexPolygon(std::vector<glm::vec2>& points) : Points(points) { CalculatePointAngles(); }

	ConvexPolygon CreateCircular(int segments, int precision = 4);

	bool IsPointInside(glm::vec2 point);
	float GetEdgeCenterRatio(glm::vec2 point, int precision = 5);
	glm::vec4 GetBounds();

	ConvexPolygon ShellScaledBy(float mult);
	ConvexPolygon ScaledBy(float mult);
	ConvexPolygon BeveledVerticesBy(float mult);
};

