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

struct VertexBoneData {
	unsigned int bones[4] = { 0,0,0,0 };
	float weights[4] = { 0.0f,0.0f,0.0f,0.0f };
};

struct AABBStruct {
	glm::vec3 min;
	glm::vec3 max;
};

class Mesh
{
	friend std::shared_ptr<Mesh> ModelManager::CreateMesh(const std::vector<Vertex>& vertices, const std::vector<unsigned>& indices, const AABBStruct AABB);
	friend std::shared_ptr<Mesh> ModelManager::CreateMesh(const std::vector<Vertex>& vertices, const std::vector<unsigned>& indices, const std::vector<VertexBoneData>& boneData, const AABBStruct AABB);

public:
	const AABBStruct AABB;

private:
	GLuint VAO;
	GLuint VBO;
	GLuint bVBO; // bone data
	GLuint EBO;
	int indicesSize;

	Mesh(GLuint VAO, GLuint VBO, GLuint bVBO, GLuint EBO, int indicesSize, const AABBStruct AABB) :
		VAO(VAO), VBO(VBO), bVBO(bVBO), EBO(EBO), indicesSize(indicesSize), AABB(AABB) {}

	inline static GLuint LastBoundVAO = 0;

public:
	inline void bind() {
		if (VAO == LastBoundVAO) return;
		glBindVertexArray(VAO);
		LastBoundVAO = VAO;
	}
	inline void forceBind() {
		glBindVertexArray(VAO);
		LastBoundVAO = VAO;
	}
	inline static void NoBind() {
		glBindVertexArray(0);
		LastBoundVAO = 0;
	}

	inline void draw() {
		glDrawElements(GL_TRIANGLES, indicesSize, GL_UNSIGNED_INT, 0);
	}
	inline void drawInstanced(int count) {
		glDrawElementsInstanced(GL_TRIANGLES, indicesSize, GL_UNSIGNED_INT, 0, count);
	}

	~Mesh() {
		glDeleteVertexArrays(1, &VAO);
		glDeleteBuffers(1, &VBO);
		if (bVBO != 0)
		{
			glDeleteBuffers(1, &bVBO);
		}
		glDeleteBuffers(1, &EBO);
	}
};

