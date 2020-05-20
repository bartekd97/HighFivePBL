#include <vector>

#include "BoneAttacherSystem.h"
#include "HFEngine.h"

#include "../Components/SkinnedMeshRenderer.h"
#include "../Components/BoneAttacher.h"

void BoneAttacherSystem::Update(float dt)
{
	auto it = gameObjects.begin();
	while (it != gameObjects.end())
	{
		auto gameObject = *(it++);
		auto gameObjectParent = gameObjectHierarchy.GetParent(gameObject);
		assert(gameObjectParent.has_value() && "Missing parent");
		assert(HFEngine::ECS.SearchComponent<SkinnedMeshRenderer>(gameObjectParent.value()) && "Missing SkinnedMeshRenderer in parent");
		
		auto& transform = HFEngine::ECS.GetComponent<Transform>(gameObject);
		auto const& attacher = HFEngine::ECS.GetComponent<BoneAttacher>(gameObject);
		auto const& renderer = HFEngine::ECS.GetComponent<SkinnedMeshRenderer>(gameObjectParent.value());

		int boneIndex = renderer.skinningData->GetBoneIndex(attacher.boneName);
		assert(boneIndex >= 0 && "Invalid bone name");

		glm::mat4 localTransform =
			renderer.boneMatrices[boneIndex] *
			renderer.skinningData->GetInversedBoneOffsets()[boneIndex] *
			renderer.skinningData->InverseModelTransform;
		transform.SetLocalMatrix(localTransform);
	}
}
