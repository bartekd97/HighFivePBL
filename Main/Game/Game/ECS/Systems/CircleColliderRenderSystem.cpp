#include <glm/glm.hpp>
#include <vector>

#include "HFEngine.h"
#include "CircleColliderRenderSystem.h"
#include "../../Resourcing/Shader.h"

#include "../Components.h"

void CircleColliderRenderSystem::Render()
{
	shader->use();
	glBindVertexArray(vao);

	HFEngine::MainCamera.Use(shader);
	glm::vec3 color(1.0f, 0.0f, 0.0f);
	shader->setVector3F("uColor", color);

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	for (auto const& gameObject : gameObjects)
	{
		auto& transform = HFEngine::ECS.GetComponent<Transform>(gameObject);
		auto& circleCollider = HFEngine::ECS.GetComponent<CircleCollider>(gameObject);

		glm::mat4 modelMat(1.0f);
		modelMat = glm::translate(modelMat, transform.GetPosition());
		modelMat *= glm::mat4_cast(transform.GetRotation());
		modelMat = glm::scale(modelMat, glm::vec3(circleCollider.radius, 1.0f, circleCollider.radius));
		shader->setMat4("gModel", modelMat);

		glDrawArrays(GL_LINE_LOOP, 0, size);
	}

	glBindVertexArray(0);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

std::vector<glm::vec3> CreateCircleArray(float radius, int fragments)
{
	const float PI = 3.1415926f;

	std::vector<glm::vec3> result;

	float increment = 2.0f * PI / fragments;

	for (float currAngle = 0.0f; currAngle <= 2.0f * PI; currAngle += increment)
	{
		result.push_back(glm::vec3(radius * cos(currAngle), 0.0f, radius * sin(currAngle)));
	}

	return result;
}

void CircleColliderRenderSystem::Init()
{
	shader = ShaderManager::GetShader("CubeShader");

	std::vector<glm::vec3> vertices = CreateCircleArray(1.0f, 36);
	size = vertices.size();

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &vboVertices);
	glBindBuffer(GL_ARRAY_BUFFER, vboVertices);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * vertices.size(), vertices.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)nullptr);
	glEnableVertexAttribArray(0);

	glBindVertexArray(0);
}
