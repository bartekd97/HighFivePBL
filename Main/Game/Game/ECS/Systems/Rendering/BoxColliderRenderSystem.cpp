#include <glm/glm.hpp>
#include <vector>

#include "HFEngine.h"
#include "BoxColliderRenderSystem.h"
#include "Resourcing/Shader.h"

#include "ECS/Components.h"

void BoxColliderRenderSystem::Render()
{
	shader->use();
	glBindVertexArray(vao);

	HFEngine::MainCamera.Use(shader);
	glm::vec3 dynamicColor(1.0f, 0.0f, 0.0f), staticColor(0.0f, 0.0f, 1.0f), triggerColor(1.0f, 1.0f, 0.0f);

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	for (auto const& gameObject : gameObjects)
	{
		auto& transform = HFEngine::ECS.GetComponent<Transform>(gameObject);
		auto& collider = HFEngine::ECS.GetComponent<Collider>(gameObject);
		auto& boxCollider = HFEngine::ECS.GetComponent<BoxCollider>(gameObject);

		if (collider.type == Collider::ColliderTypes::DYNAMIC) shader->setVector3F("uColor", dynamicColor);
		else if (collider.type == Collider::ColliderTypes::STATIC) shader->setVector3F("uColor", staticColor);
		else shader->setVector3F("uColor", triggerColor);

		glm::mat4 modelMat(1.0f);
		modelMat = glm::translate(modelMat, transform.GetWorldPosition() * glm::vec3(1.0f, 0.0f, 1.0f));
		modelMat *= glm::mat4_cast(transform.GetWorldRotation());
		modelMat = glm::scale(modelMat, glm::vec3(boxCollider.width, 1.0f, boxCollider.height));
		shader->setMat4("gModel", modelMat);

		glDrawArrays(GL_LINE_STRIP, 0, 5);
	}

	glBindVertexArray(0);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void BoxColliderRenderSystem::Init()
{
	shader = ShaderManager::GetShader("CubeShader");

	std::vector<glm::vec3> vertices =
	{
		glm::vec3(0.5f, 0.0f, -0.5f),
		glm::vec3(0.5f, 0.0f, 0.5f),
		glm::vec3(-0.5f, 0.0f, 0.5f),
		glm::vec3(-0.5f, 0.0f, -0.5f),
		glm::vec3(0.5f, 0.0f, -0.5f)
	};

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &vboVertices);
	glBindBuffer(GL_ARRAY_BUFFER, vboVertices);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * vertices.size(), vertices.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)nullptr);
	glEnableVertexAttribArray(0);

	glBindVertexArray(0);
}
