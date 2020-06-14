#pragma once
#include <glm/glm.hpp>

namespace PrimitiveRenderer {
	void DrawScreenQuad();

	void DrawLine(glm::vec3& a, glm::vec3& b);
	void DrawLines();
	void RejectLines();

	void DrawStickyPoint(glm::vec3 pos);
	void DrawStickyPoints();
}
