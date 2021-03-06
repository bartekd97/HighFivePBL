#include <vector>

#include "Resourcing/AnimationClip.h"
#include "SkinAnimatorSystem.h"
#include "HFEngine.h"

#include "../Components/SkinnedMeshRenderer.h"
#include "../Components/SkinAnimator.h"

void SkinAnimatorSystem::WorkQueue(float dt)
{
	GameObject gameObject;
	while (workerQueue.try_pop(gameObject))
	{
		auto& renderer = HFEngine::ECS.GetComponent<SkinnedMeshRenderer>(gameObject);
		auto& animator = HFEngine::ECS.GetComponent<SkinAnimator>(gameObject);

		auto nodes = renderer.skinningData->GetNodes();
		auto boneOffsets = renderer.skinningData->GetBoneOffsets();
		auto inverseRootTransform = renderer.skinningData->InverseRootTransform;

		using IM = std::pair<int, glm::mat4>;

		std::vector<IM> rec;
		rec.reserve(nodes.size());
		rec.emplace_back(IM(0, glm::mat4(1.0f)));

		animator.animTime += animator.animatorSpeed * dt;
		float transitionFactor = 0.0f;
		if (animator.transitioning)
		{
			animator.nextAnimTime += animator.animatorSpeed * dt;
			transitionFactor = animator.nextAnimTime / animator.transitionDuration;
			if (transitionFactor >= 1.0f)
			{
				animator.currentClip = animator.nextClip;
				animator.animTime = animator.nextAnimTime;
				animator.currentClipMode = animator.nextClipMode;
				animator.transitioning = false;
			}
		}

		auto stateSignature = animator.CalculateStateSignature();
		if (animator.lastSignature.operator==(stateSignature))
		{
			renderer.business.MakeFree();
			continue;
		}
		else
		{
			animator.lastSignature = stateSignature;
		}

		IM i;
		SkinningData::Node* node;
		glm::mat4 globalTransform;
		glm::mat4 globalTransformNext;
		int ci;
		while (!rec.empty())
		{
			i = rec.back();
			rec.pop_back();
			node = &nodes[i.first];

			if (animator.transitioning)
			{
				globalTransform = animator.currentClip->EvaluateChannel
				(
					node->name, animator.animTime, node->nodeTransform, animator.currentClipMode
					);
				globalTransformNext = animator.nextClip->EvaluateChannel
				(
					node->name, animator.nextAnimTime, node->nodeTransform, animator.nextClipMode
					);
				// TODO: Make this interpolation in more proper way if bug occurs
				globalTransform = i.second * (
					(globalTransform * (1.0f - transitionFactor)) +
					(globalTransformNext * transitionFactor)
					);
			}
			else
			{
				globalTransform = i.second * animator.currentClip->EvaluateChannel
				(
					node->name, animator.animTime, node->nodeTransform, animator.currentClipMode
					);
			}

			if (node->boneIndex != -1)
				renderer.boneMatrices[node->boneIndex] = inverseRootTransform * globalTransform * boneOffsets[node->boneIndex];

			for (ci = 0; ci < node->childCount; ci++)
				rec.emplace_back(IM(node->childIndices[ci], globalTransform));
		}

		renderer.needMatricesBufferUpdate = true;
		renderer.business.MakeFree();
	}
}

void SkinAnimatorSystem::Update(float dt)
{
	skinAimatorWorker.WaitForAll(); // make sure we start frame with free worker

	auto it = gameObjects.begin();
	while (it != gameObjects.end())
	{
		auto gameObject = *(it++);

		auto& renderer = HFEngine::ECS.GetComponent<SkinnedMeshRenderer>(gameObject);
		auto& animator = HFEngine::ECS.GetComponent<SkinAnimator>(gameObject);

		if (animator.currentClip == nullptr || renderer.skinningData == nullptr)
			continue;

		// skip renderers that weren't seen on screen
		// there is 1 frame delay (cuz it checks state from previous render frame)
		// but extended AABB should do the trick
		if (renderer.cullingData.lastUpdate + 1 < HFEngine::CURRENT_FRAME_NUMBER)
			continue;
		if (!renderer.cullingData.visibleByViewCamera && !renderer.cullingData.visibleByLightCamera)
			continue;

		renderer.business.MakeBusy();
		workerQueue.push(gameObject);
	}

	skinAimatorWorker.FillWorkers(std::bind(&SkinAnimatorSystem::WorkQueue, this, dt));
	//skinAimatorWorker.FillWorkers([this, dt]() {this->WorkQueue(dt);});
}

void SkinAnimatorSystem::PostRender()
{
	skinAimatorWorker.WaitForAll();
}
