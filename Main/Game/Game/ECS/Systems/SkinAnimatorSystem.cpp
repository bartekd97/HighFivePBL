#include <vector>

#include "Resourcing/AnimationClip.h"
#include "SkinAnimatorSystem.h"
#include "HFEngine.h"

#include "../Components/SkinnedMeshRenderer.h"
#include "../Components/SkinAnimator.h"

void SkinAnimatorSystem::Update(float dt)
{
	auto it = gameObjects.begin();
	while (it != gameObjects.end())
	{
		auto gameObject = *(it++);

		auto& renderer = HFEngine::ECS.GetComponent<SkinnedMeshRenderer>(gameObject);
		auto& animator = HFEngine::ECS.GetComponent<SkinAnimator>(gameObject);

		if (animator.currentClip == nullptr || renderer.skinningData == nullptr)
			continue;

		auto nodes = renderer.skinningData->GetNodes();
		auto boneOffsets = renderer.skinningData->GetBoneOffsets();
		auto inverseRootTransform = renderer.skinningData->InverseRootTransform;

		using IM = std::pair<int, glm::mat4>;

		std::vector<IM> rec;
		rec.reserve(nodes.size());
		rec.emplace_back(IM(0, glm::mat4(1.0f)));

		float animTime = animator.animTime;
		animator.animTime += dt;

		IM i;
		SkinningData::Node* node;
		glm::mat4 globalTransform;
		int ci;
		while (!rec.empty())
		{
			i = rec.back();
			rec.pop_back();
			node = &nodes[i.first];

			globalTransform = i.second * animator.currentClip->EvaluateChannel
			(
				node->name, animTime, node->nodeTransform
			);
			
			if (node->boneIndex != -1)
				renderer.boneMatrices[node->boneIndex] = inverseRootTransform * globalTransform * boneOffsets[node->boneIndex];

			for (ci=0; ci< node->childCount; ci++)
				rec.emplace_back(IM(node->childIndices[ci], globalTransform));
		}
	}
}
