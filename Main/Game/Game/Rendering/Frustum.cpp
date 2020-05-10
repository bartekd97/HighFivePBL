#include <glm/gtc/matrix_access.hpp>
#include "Frustum.h"

Frustum::Frustum(glm::mat4 matrix)
{
	glm::vec4 rowX = glm::row(matrix, 0);
	glm::vec4 rowY = glm::row(matrix, 1);
	glm::vec4 rowZ = glm::row(matrix, 2);
	glm::vec4 rowW = glm::row(matrix, 3);

	auto normalize = [](glm::vec4 plane) {
		glm::vec3 normal(plane.x, plane.y, plane.z);
		float length = glm::length(normal);
		return -plane / length;
	};

	planes[0] = normalize(rowW + rowX);
	planes[1] = normalize(rowW - rowX);
	planes[2] = normalize(rowW + rowY);
	planes[3] = normalize(rowW - rowY);
	planes[4] = normalize(rowW + rowZ);
	planes[5] = normalize(rowW - rowZ);
}