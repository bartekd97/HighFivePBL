#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "ShaderManager.h"

class Shader
{
	friend void ShaderManager::Initialize();

private:
	GLuint program;

	inline Shader(GLuint program) : program(program) {}

public:
	inline void use() { glUseProgram(program); }

	inline void setInt(const char* name, int val) {
		glUniform1i( glGetUniformLocation(program, name), val );
	}
	inline void setFloat(const char* name, float val) {
		glUniform1i(glGetUniformLocation(program, name), val);
	}

	inline void setVector2F(const char* name, float x, float y) {
		glUniform2f(glGetUniformLocation(program, name), x, y);
	}
	inline void setVector3F(const char* name, float x, float y, float z) {
		glUniform3f(glGetUniformLocation(program, name), x, y, z);
	}
	inline void setVector4F(const char* name, float x, float y, float z, float w) {
		glUniform4f(glGetUniformLocation(program, name), x, y, z, w);
	}

	inline void setVector2F(const char* name, glm::vec2 &vec) {
		glUniform2f(glGetUniformLocation(program, name), vec.x, vec.y);
	}
	inline void setVector3F(const char* name, glm::vec3 &vec) {
		glUniform3f(glGetUniformLocation(program, name), vec.x, vec.y, vec.z);
	}
	inline void setVector4F(const char* name, glm::vec4 &vec) {
		glUniform4f(glGetUniformLocation(program, name), vec.x, vec.y, vec.z, vec.w);
	}

	inline void setMat4(const char* name, glm::mat4 &mat) {
		glUniformMatrix4fv(glGetUniformLocation(program, name), 1, GL_FALSE, glm::value_ptr(mat));
	}

	// maybe for later usage
	/*
	void setTexture(const char* name, shared_ptr<Texture> texture, unsigned int slotId);
	void setCubemap(const char* name, shared_ptr<Cubemap> cubemap, unsigned int slotId);

	void bindUBO(const char* name, GLuint bindingPoint);
	*/

	inline ~Shader()
	{ glDeleteProgram(program); }
};