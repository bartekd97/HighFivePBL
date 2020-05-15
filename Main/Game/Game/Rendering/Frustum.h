#pragma once

#include <glm/glm.hpp>
#include "Resourcing/Mesh.h"

class Frustum
{
private:
	glm::vec4 planes[6];

public:
	Frustum(glm::mat4 matrix);

	inline bool CheckIntersection(const AABBStruct& AABB)
	{
		glm::vec3 vmin, vmax;
		for (int k = 0; k < 6; ++k)
		{
			glm::vec3 n(planes[k].x, planes[k].y, planes[k].z);
			for (int i = 0; i < 3; ++i)
			{
				if (n[i] >= 0)
				{
					vmin[i] = AABB.min[i];
					vmax[i] = AABB.max[i];
				}
				else
				{
					vmin[i] = AABB.max[i];
					vmax[i] = AABB.min[i];
				}
			}

			if (glm::dot(n, vmin) + planes[k].w > 0.0f)
				return false; // not visible
		}

		return true; // visible fully or partial
	}
};

