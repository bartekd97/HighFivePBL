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
	friend std::shared_ptr<Mesh> ModelManager::CreateMesh(std::vector<Vertex>& vertices, std::vector<unsigned>& indices, AABBStruct AABB);
	friend std::shared_ptr<Mesh> ModelManager::CreateMesh(std::vector<Vertex>& vertices, std::vector<unsigned>& indices, std::vector<VertexBoneData>& boneData, AABBStruct AABB);

public:
	const AABBStruct AABB;

private:
	GLuint VAO;
	GLuint VBO;
	GLuint bVBO; // bone data
	GLuint EBO;
	int indicesSize;

	Mesh(GLuint VAO, GLuint VBO, GLuint bVBO, GLuint EBO, int indicesSize, AABBStruct AABB) :
		VAO(VAO), VBO(VBO), bVBO(bVBO), EBO(EBO), indicesSize(indicesSize), AABB(AABB) {}

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
		if (bVBO != 0)
		{
			glDeleteBuffers(1, &bVBO);
		}
		glDeleteBuffers(1, &EBO);
	}
};

