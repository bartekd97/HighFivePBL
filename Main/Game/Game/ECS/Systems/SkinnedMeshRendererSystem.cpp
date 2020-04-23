#include "HFEngine.h"
#include "SkinnedMeshRendererSystem.h"
#include "../../Resourcing/Material.h"
#include "../../Resourcing/Mesh.h"

#include "../Components.h"

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

	HFEngine::MainCamera.Use(toGBufferShader);

	for (auto const& gameObject : gameObjects)
	{
		auto& transform = HFEngine::ECS.GetComponent<Transform>(gameObject);
		auto const& renderer = HFEngine::ECS.GetComponent<SkinnedMeshRenderer>(gameObject);

		if (!renderer.enabled)
			continue;

		glm::mat4 modelMat = transform.GetWorldTransform();
		toGBufferShader->setMat4("gModel", modelMat);

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
