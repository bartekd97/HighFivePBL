#include "HFEngine.h"
#include "SkinnedMeshRendererSystem.h"
#include "Resourcing/Material.h"
#include "Resourcing/Mesh.h"

#include "ECS/Components/Transform.h"
#include "ECS/Components/SkinnedMeshRenderer.h"

void SkinnedMeshRendererSystem::Init()
{
	toGBufferShader = ShaderManager::GetShader("ToGBufferSkinned");
	toGBufferShader->use();
	MaterialBindingPoint::AssignToShader(toGBufferShader);
}

void SkinnedMeshRendererSystem::RenderToGBuffer()
{
	toGBufferShader->use();
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	Frustum frustum = HFEngine::MainCamera.GetFrustum();
	ScheduleCulling(frustum);
	FinishCulling();

	HFEngine::MainCamera.Use(toGBufferShader);

	auto renderers = HFEngine::ECS.GetAllComponents<SkinnedMeshRenderer>();
	for (auto const& renderer : renderers)
	{
		if (!renderer.cullingData.visibleByMainCamera)
			continue;

		toGBufferShader->setMat4("gModel", renderer.cullingData.worldTransform);

		for (int i = 0; i < SkinnedMeshRenderer::MAX_BONES; i++)
		{
			auto mat = renderer.boneMatrices[i];
			toGBufferShader->setMat4(("gBones[" + std::to_string(i) + "]").c_str(), mat);
		}

		renderer.material->apply(toGBufferShader);
		renderer.mesh->bind();
		renderer.mesh->draw();
	}

	glDisable(GL_CULL_FACE);
}
