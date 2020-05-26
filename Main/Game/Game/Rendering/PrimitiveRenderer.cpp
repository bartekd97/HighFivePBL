#include <glad/glad.h>
#include <vector>
#include <glm/glm.hpp>
#include <gl/GL.h>
#include "PrimitiveRenderer.h"
#include "../Resourcing/ShaderManager.h"
#include "../Resourcing/Shader.h"
#include "../HFEngine.h"

std::vector<glm::vec3> linePoints;

void PrimitiveRenderer::DrawScreenQuad()
{
	static GLuint quadVAO = 0;
	static GLuint quadVBO = 0;
	if (quadVAO == 0)
	{
		float quadVertices[] = {
			// positions        // texture Coords
			-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
			-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
			 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
			 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		};
		// setup plane VAO
		glGenVertexArrays(1, &quadVAO);
		glGenBuffers(1, &quadVBO);
		glBindVertexArray(quadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	}
	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
}

// xD
void PrimitiveRenderer::DrawLine(glm::vec3& a, glm::vec3& b)
{
	linePoints.push_back(a);
	linePoints.push_back(b);
}

void PrimitiveRenderer::DrawLines()
{
	if (linePoints.size() == 0) return;

	static glm::vec3 color(1.0f, 0.0f, 0.0f);
	auto shader = ShaderManager::GetShader("CubeShader");
	shader->use();

	HFEngine::MainCamera.Use(shader);
	shader->setMat4("gModel", glm::mat4(1.0f));
	shader->setVector3F("uColor", color);

	GLuint lineVAO;
	GLuint lineVBO;

	glGenVertexArrays(1, &lineVAO);
	glGenBuffers(1, &lineVBO);
	glBindVertexArray(lineVAO);
	glBindBuffer(GL_ARRAY_BUFFER, lineVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * linePoints.size(), linePoints.data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);

	glBindVertexArray(lineVAO);
	glDrawArrays(GL_LINES, 0, linePoints.size());
	glBindVertexArray(0);

	linePoints.clear();
	glDeleteVertexArrays(1, &lineVAO);
}
