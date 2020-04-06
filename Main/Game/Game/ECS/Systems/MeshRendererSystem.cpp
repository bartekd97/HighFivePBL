#include "HFEngine.h"
#include "MeshRendererSystem.h"
#include "../../Resourcing/Material.h"
#include "../../Resourcing/Mesh.h"

#include "../Components.h"

void MeshRendererSystem::Init()
{
	toGBufferShader = ShaderManager::GetShader("ToGBuffer");
	toGBufferShader->use();
	MaterialBindingPoint::AssignToShader(toGBufferShader);
}

void MeshRendererSystem::RenderToGBuffer()
{
	toGBufferShader->use();
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	HFEngine::MainCamera.Use(toGBufferShader);

	for (auto const& gameObject : gameObjects)
	{
		auto& transform = HFEngine::ECS.GetComponent<Transform>(gameObject);
		auto const& renderer = HFEngine::ECS.GetComponent<MeshRenderer>(gameObject);

		if (!renderer.enabled)
			continue;

		glm::mat4 modelMat = transform.GetWorldTransform();
		toGBufferShader->setMat4("gModel", modelMat);

		renderer.material->apply(toGBufferShader);
		renderer.mesh->bind();
		renderer.mesh->draw();
	}

	glDisable(GL_CULL_FACE);
}
