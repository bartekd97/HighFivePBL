#include <glm/glm.hpp>
#include <vector>

#include "HFEngine.h"
#include "GravityColliderRenderSystem.h"
#include "Resourcing/Shader.h"

#include "ECS/Components.h"

void GravityColliderRenderSystem::Render()
{
	/*shader->use();
	glBindVertexArray(vao);

	HFEngine::MainCamera.Use(shader);
	glm::vec3 dynamicColor(1.0f, 0.0f, 0.0f), staticColor(0.0f, 0.0f, 1.0f), triggerColor(1.0f, 1.0f, 0.0f);

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	for (auto const& gameObject : gameObjects)
	{
		auto& transform = HFEngine::ECS.GetComponent<Transform>(gameObject);
		auto& collider = HFEngine::ECS.GetComponent<Collider>(gameObject);
		auto& gravityCollider = HFEngine::ECS.GetComponent<GravityCollider>(gameObject);

		if (collider.type == Collider::ColliderTypes::DYNAMIC) shader->setVector3F("uColor", dynamicColor);
		else if (collider.type == Collider::ColliderTypes::STATIC) shader->setVector3F("uColor", staticColor);
		else shader->setVector3F("uColor", triggerColor);

		glm::mat4 modelMat(1.0f);
		modelMat = glm::translate(modelMat, transform.GetWorldPosition() * glm::vec3(1.0f, 0.0f, 1.0f));
		modelMat *= glm::mat4_cast(transform.GetWorldRotation());
		modelMat = glm::scale(modelMat, glm::vec3(gravityCollider.width, 1.0f, gravityCollider.height));
		shader->setMat4("gModel", modelMat);

		glDrawArrays(GL_LINE_STRIP, 0, 5);
	}

	glBindVertexArray(0);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);*/
}

void GravityColliderRenderSystem::Init()
{
	shader = ShaderManager::GetShader("CubeShader");
}
