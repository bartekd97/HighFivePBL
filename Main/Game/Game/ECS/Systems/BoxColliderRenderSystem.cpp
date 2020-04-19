#include <glm/glm.hpp>
#include <vector>

#include "HFEngine.h"
#include "BoxColliderRenderSystem.h"
#include "../../Resourcing/Shader.h"

#include "../Components.h"

void BoxColliderRenderSystem::Render()
{
	shader->use();
	glBindVertexArray(vao);

	HFEngine::MainCamera.Use(shader);
	glm::vec3 color(1.0f, 0.0f, 0.0f);
	shader->setVector3F("uColor", color);

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	for (auto const& gameObject : gameObjects)
	{
		auto& transform = HFEngine::ECS.GetComponent<Transform>(gameObject);
		auto& boxCollider = HFEngine::ECS.GetComponent<BoxCollider>(gameObject);

		glm::mat4 modelMat = transform.GetWorldTransform();
		modelMat = glm::scale(modelMat, glm::vec3(boxCollider.width, 0.0f, boxCollider.height));
		shader->setMat4("gModel", modelMat);

		glDrawArrays(GL_TRIANGLES, 0, 6);
	}

	glBindVertexArray(0);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void BoxColliderRenderSystem::Init()
{
	shader = ShaderManager::GetShader("CubeShader");

	std::vector<glm::vec3> vertices =
	{
		glm::vec3(-1.0f, 5.0f, -1.0f),
		glm::vec3(1.0f, 5.0f, -1.0f),
		glm::vec3(1.0f, 5.0f, 1.0f),
		glm::vec3(1.0f, 5.0f, 1.0f),
		glm::vec3(-1.0f, 5.0f, 1.0f),
		glm::vec3(-1.0f, 5.0f, -1.0f)
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
