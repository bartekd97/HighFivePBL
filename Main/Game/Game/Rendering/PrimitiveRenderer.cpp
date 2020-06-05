#include <glad/glad.h>
#include <vector>
#include <glm/glm.hpp>
#include <gl/GL.h>
#include "PrimitiveRenderer.h"
#include "../Resourcing/ShaderManager.h"
#include "../Resourcing/Shader.h"
#include "../HFEngine.h"

#define MAX_STICKY_POINTS 64000

std::vector<glm::vec3> linePoints;
std::vector<glm::vec3> stickyPoints;

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

	static glm::vec3 color(0.4f, 1.0f, 0.0f);
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
	glLineWidth(2.0f);
	glDrawArrays(GL_LINES, 0, linePoints.size());
	glBindVertexArray(0);

	linePoints.clear();
	glDeleteVertexArrays(1, &lineVAO);
}

void PrimitiveRenderer::RejectLines()
{
	linePoints.clear();
}



void PrimitiveRenderer::DrawStickyPoint(glm::vec3 pos)
{
	assert(stickyPoints.size() < MAX_STICKY_POINTS && "Too many Sticky points");
	stickyPoints.push_back(pos);
}

void PrimitiveRenderer::DrawStickyPoints()
{
	if (stickyPoints.size() == 0) return;

	static glm::vec3 color(5.0f, 1.0f, 1.0f);
	auto shader = ShaderManager::GetShader("CubeShader");
	shader->use();

	HFEngine::MainCamera.Use(shader);
	shader->setMat4("gModel", glm::mat4(1.0f));
	shader->setVector3F("uColor", color);

	static GLuint pointsVAO = 0;
	static GLuint pointsVBO = 0;

	if (pointsVAO == 0)
	{
		glGenVertexArrays(1, &pointsVAO);
		glGenBuffers(1, &pointsVBO);
		glBindVertexArray(pointsVAO);
		glBindBuffer(GL_ARRAY_BUFFER, pointsVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * MAX_STICKY_POINTS, NULL, GL_DYNAMIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
	}

	glBindVertexArray(pointsVAO);
	glBindBuffer(GL_ARRAY_BUFFER, pointsVBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::vec3) * stickyPoints.size(), stickyPoints.data());
	glPointSize(4.0f);
	glDrawArrays(GL_POINTS, 0, stickyPoints.size());
	glBindVertexArray(0);
}
