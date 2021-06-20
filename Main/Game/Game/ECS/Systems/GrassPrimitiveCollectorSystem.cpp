#include "GrassPrimitiveCollectorSystem.h"
#include "HFEngine.h"
#include "ECS/Components/MeshRenderer.h";
#include "ECS/Components/SkinnedMeshRenderer.h";


void GrassPrimitiveCollectorSystem::Init()
{
}

void GrassPrimitiveCollectorSystem::Update(float dt)
{
	auto simulators = HFEngine::ECS.GetAllComponents<GrassSimulator>();

	for (auto& s : simulators)
	{
		s.collectedPrimitives.clear();
	}

	auto it = gameObjects.begin();
	while (it != gameObjects.end())
	{
		auto gameObject = *(it++);
		auto& primitive = HFEngine::ECS.GetComponent<GrassCircleSimulationPrimitive>(gameObject);
		auto& transform = HFEngine::ECS.GetComponent<Transform>(gameObject);

		bool visible = primitive.radius > 0.0001f || primitive.targetHeight >= 0.98f;

		if (HFEngine::ECS.SearchComponent<MeshRenderer>(gameObject))
		{
			auto& renderer = HFEngine::ECS.GetComponent<MeshRenderer>(gameObject);
			if (HFEngine::CURRENT_FRAME_NUMBER - renderer.cullingData.lastUpdate > 5 || !renderer.cullingData.visibleByViewCamera)
				visible = false;
		}
		if (HFEngine::ECS.SearchComponent<SkinnedMeshRenderer>(gameObject))
		{
			auto& renderer = HFEngine::ECS.GetComponent<SkinnedMeshRenderer>(gameObject);
			if (HFEngine::CURRENT_FRAME_NUMBER - renderer.cullingData.lastUpdate > 5 || !renderer.cullingData.visibleByViewCamera)
				visible = false;
		}

		if (!visible)
			continue;

		for (auto& s : simulators)
		{
			if (IsInside(s, transform.GetWorldPosition(), primitive.radius))
			{
				s.collectedPrimitives.push_back(gameObject);
			}
		}
	}
}

bool GrassPrimitiveCollectorSystem::IsInside(const GrassSimulator& simulator, glm::vec3 position, float radius)
{
	glm::vec4 exBounds = glm::vec4(
		simulator.worldBounds.x - radius,
		simulator.worldBounds.y - radius,
		simulator.worldBounds.z + radius,
		simulator.worldBounds.w + radius
	);

	return position.x > simulator.worldBounds.x && position.x < simulator.worldBounds.z
		&& position.z > simulator.worldBounds.y && position.z < simulator.worldBounds.w;
}
