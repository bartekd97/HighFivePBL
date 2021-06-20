#pragma once

#include "HFEngine.h"
#include "ECS/System.h"
#include "ECS/Components/Transform.h"
#include "ECS/Components/CulledRenderer.h"
#include "Rendering/Frustum.h"
#include "Utility/TaskPool.h"

template<typename RenderedComponent, size_t _WorksersCount>
class CulledRendererSystem : public System
{
private:
	TaskPool<_WorksersCount> cullingWorkers;
	concurrency::concurrent_queue<GameObject> gameObjectsQueue;

private:
	inline void CullingTask(Frustum viewFrustum, Frustum lightFrustum)
	{
		GameObject gameObject;
		while (gameObjectsQueue.try_pop(gameObject))
		{
			Transform& transform = HFEngine::ECS.GetComponent<Transform>(gameObject);
			RenderedComponent& renderer = HFEngine::ECS.GetComponent<RenderedComponent>(gameObject);
			CulledRenderer& cullingData = renderer.cullingData;

			if (transform.LastFrameUpdate() > cullingData.lastUpdate)
			{
				cullingData.worldAABB = RecalculateWorldAABB(GetLocalAABB(renderer), transform);
				cullingData.worldTransform = transform.GetWorldTransform();
				cullingData.targetGameObject = gameObject;
			}
			cullingData.lastUpdate = HFEngine::CURRENT_FRAME_NUMBER;

			cullingData.visibleByViewCamera = viewFrustum.CheckIntersection(cullingData.worldAABB);
			cullingData.visibleByLightCamera = lightFrustum.CheckIntersection(cullingData.worldAABB);
		}
	}

public:

	inline void ScheduleCulling(Frustum& viewFrustum, Frustum& lightFrustum)
	{
		for (auto const& gameObject : gameObjects)
			gameObjectsQueue.push(gameObject);

		cullingWorkers.FillWorkers([this, &viewFrustum, &lightFrustum]() {this->CullingTask(viewFrustum, lightFrustum);});
	}
	inline void FinishCulling()
	{
		cullingWorkers.WaitForAll();
	}

	inline virtual const AABBStruct& GetLocalAABB(RenderedComponent& component) = 0;
	inline virtual AABBStruct RecalculateWorldAABB(const AABBStruct& localAABB, Transform& transform)
	{
		glm::mat4 model = transform.GetWorldTransform();
		glm::vec3 points[] =
		{
			{ localAABB.min.x, localAABB.min.y, localAABB.min.z },
			{ localAABB.max.x, localAABB.min.y, localAABB.min.z },
			{ localAABB.max.x, localAABB.max.y, localAABB.min.z },
			{ localAABB.min.x, localAABB.max.y, localAABB.min.z },

			{ localAABB.min.x, localAABB.min.y, localAABB.max.z },
			{ localAABB.max.x, localAABB.min.y, localAABB.max.z },
			{ localAABB.max.x, localAABB.max.y, localAABB.max.z },
			{ localAABB.min.x, localAABB.max.y, localAABB.max.z }
		};

		// transform points to world space
		for (int i = 0; i < 8; ++i)
		{
			points[i] = glm::vec3(model * glm::vec4(points[i], 1.0f));
		}

		glm::vec3 wMin = points[0];
		glm::vec3 wMax = points[0];
		for (int i = 1; i < 8; ++i)
		{
			wMin.x = glm::min(wMin.x, points[i].x);
			wMin.y = glm::min(wMin.y, points[i].y);
			wMin.z = glm::min(wMin.z, points[i].z);
			wMax.x = glm::max(wMax.x, points[i].x);
			wMax.y = glm::max(wMax.y, points[i].y);
			wMax.z = glm::max(wMax.z, points[i].z);
		}
		return { wMin, wMax };
	}
};