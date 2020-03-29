#pragma once

#include <memory>
#include <vector>
#include <glm/glm.hpp>
#include <glad/glad.h>
#include "ModelManager.h"

struct Vertex {
	glm::vec3 position;
	glm::vec2 uv;
	glm::vec3 normal;
	glm::vec3 tangent;
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
	static std::shared_ptr<Mesh> New(std::vector<Vertex>& vertices, std::vector<unsigned>& indices);

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

