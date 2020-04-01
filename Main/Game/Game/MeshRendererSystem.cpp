#include "HFEngine.h"
#include "MeshRendererSystem.h"
#include "Material.h"
#include "Mesh.h"

#include "Transform.h"
#include "MeshRenderer.h"

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

	// tmp
	// TODO: use camera matrixes
	glm::mat4 view = glm::lookAt(glm::vec3(0.0f, 5.0f, 10.0f),
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)(1280) / (float)(720), 0.1f, 100.0f);
	toGBufferShader->setMat4("gView", view);
	toGBufferShader->setMat4("gProjection", projection);

	for (auto const& gameObject : gameObjects)
	{
		auto const& transform = HFEngine::ECS.GetComponent<Transform>(gameObject);
		auto const& renderer = HFEngine::ECS.GetComponent<MeshRenderer>(gameObject);

		if (!renderer.enabled)
			continue;

		// TODO: use better transform component with already calcualted matrix
		glm::mat4 matrix = glm::mat4(1.0f);
		matrix = glm::translate(matrix, transform.position);
		matrix = glm::rotate(matrix, transform.rotation.y, glm::vec3(0.0, 1.0, 0.0));
		matrix = glm::rotate(matrix, transform.rotation.x, glm::vec3(1.0, 0.0, 0.0));
		matrix = glm::rotate(matrix, transform.rotation.z, glm::vec3(0.0, 0.0, 1.0));
		matrix = glm::scale(matrix, transform.scale);

		toGBufferShader->setMat4("gModel", matrix);

		renderer.material->apply(toGBufferShader);
		renderer.mesh->bind();
		renderer.mesh->draw();
	}

	glDisable(GL_CULL_FACE);
}
