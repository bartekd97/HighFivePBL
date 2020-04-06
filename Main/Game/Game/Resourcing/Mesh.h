#pragma once

#include <memory>
#include <vector>
#include <glm/glm.hpp>
#include <glad/glad.h>
#include "ModelManager.h"

struct Vertex {
	glm::vec3 position = { 0,0,0 };
	glm::vec2 uv = { 0,0 };
	glm::vec3 normal = { 0,0,0 };
	glm::vec3 tangent = { 0,0,0 };
	glm::vec3 bitangent = { 0,0,0 };
};

class Mesh
{
	friend std::shared_ptr<Mesh> ModelManager::CreateMesh(std::vector<Vertex>& vertices, std::vector<unsigned>& indices);

private:
	GLuint VAO;
	GLuint VBO;
	GLuint EBO;
	int indicesSize;

	Mesh(GLuint VAO, GLuint VBO, GLuint EBO, int indicesSize) :
		VAO(VAO), VBO(VBO), EBO(EBO), indicesSize(indicesSize) {}

public:
	inline void bind() { glBindVertexArray(VAO); }

	inline void draw() {
		glDrawElements(GL_TRIANGLES, indicesSize, GL_UNSIGNED_INT, 0);
	}
	inline void drawInstanced(int count) {
		glDrawElementsInstanced(GL_TRIANGLES, indicesSize, GL_UNSIGNED_INT, 0, count);
	}

	~Mesh() {
		glDeleteVertexArrays(1, &VAO);
		glDeleteBuffers(1, &VBO);
		glDeleteBuffers(1, &EBO);
	}
};

