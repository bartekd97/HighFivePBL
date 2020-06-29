#pragma once
#include <glm/glm.hpp>

namespace PrimitiveRenderer {

	void Init();
	void DrawScreenQuad();

	void DrawLine(const glm::vec3& a, const glm::vec3& b);
	void DrawLines();
	void RejectLines();

	void DrawStickyPoint(glm::vec3 pos);
	void DrawStickyPoints();
}
