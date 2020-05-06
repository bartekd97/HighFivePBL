#pragma once

#include <thread>
#include <concurrent_queue.h>

#include "HFEngine.h"
#include "ECS/System.h"
#include "ECS/Components/Transform.h"
#include "Rendering/Frustum.h"

template<typename RenderedComponent>
class CulledRendererSystem : public System
{
public:
	concurrency::concurrent_queue<GameObject> gameObjectsQueue;
	std::array<std::thread, 4> workers;
	std::uint32_t business;

	bool exitWorker = false;

	Frustum* workerViewFrustum;
	inline void CullWorker(uint32_t bitflag)
	{
		GameObject gameObject;
		while (!exitWorker)
		{
			if (gameObjectsQueue.empty()) {
				business ^= bitflag;
				std::this_thread::yield();
			}
			else {
				business |= bitflag;
				if (!gameObjectsQueue.try_pop(gameObject))
					continue;

				Frustum viewFrustum = *workerViewFrustum;

				Transform& transform = HFEngine::ECS.GetComponent<Transform>(gameObject);
				RenderedComponent& renderer = HFEngine::ECS.GetComponent<RenderedComponent>(gameObject);

				if (!renderer.enabled)
					continue;

				if (transform.LastFrameUpdate() > renderer.lastAABBUpdate)
				{
					renderer.worldAABB = RecalculateWorldAABB(renderer.mesh->AABB, transform);
					renderer.worldTransform = transform.GetWorldTransform();
				}
				renderer.lastAABBUpdate = HFEngine::CURRENT_FRAME_NUMBER;

				renderer.visibleByMainCamera = viewFrustum.CheckIntersection(renderer.worldAABB);
			}
		}
	}

public:

	virtual ~CulledRendererSystem() {
		exitWorker = true;
		for (uint32_t i = 0; i < workers.size(); i++)
		{
			workers[i].join();
		}
	};

	inline void InitWorkers()
	{
		for (uint32_t i = 0; i < workers.size(); i++)
		{
			workers[i] = std::thread(&CulledRendererSystem<RenderedComponent>::CullWorker, this, 1 << i);
		}
		business = 0;
	}

	inline void Cull(Frustum& viewFrustum)
	{
		workerViewFrustum = &viewFrustum;

		for (auto const& gameObject : gameObjects)
			gameObjectsQueue.push(gameObject);

		do {
			std::this_thread::yield();
		} while (business != 0);

		/*
		for (auto const& gameObject : gameObjects)
		{
			Transform& transform = HFEngine::ECS.GetComponent<Transform>(gameObject);
			RenderedComponent& renderer = HFEngine::ECS.GetComponent<RenderedComponent>(gameObject);
			
			if (!renderer.enabled)
				continue;

			if (transform.LastFrameUpdate() > renderer.lastAABBUpdate)
			{
				renderer.worldAABB = RecalculateWorldAABB(renderer.mesh->AABB, transform);
				
				renderer.worldTransform = transform.GetWorldTransform();
			}
			renderer.lastAABBUpdate = HFEngine::CURRENT_FRAME_NUMBER;

			renderer.visibleByMainCamera = viewFrustum.CheckIntersection(renderer.worldAABB);
		}*/
	}

	inline AABBStruct RecalculateWorldAABB(AABBStruct localAABB, Transform& transform)
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


	// keep virtuality
	virtual void Init() {};
};