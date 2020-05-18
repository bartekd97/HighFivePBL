#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <tsl/robin_map.h>
#include "ShaderManager.h"

class Shader {
	friend void ShaderManager::Initialize();

private:
	GLuint program;

	tsl::robin_map<std::string, GLint> unfiromLocations;

#ifdef _DEBUG
	GLint __CURRENT_SHADER = 0;
	#define BREAK_IF_SHADER_IS_NOT_ACTIVE() glGetIntegerv(GL_CURRENT_PROGRAM, &__CURRENT_SHADER); \
		if (program != __CURRENT_SHADER) {__debugbreak();}
#else
	#define BREAK_IF_SHADER_IS_NOT_ACTIVE()
#endif

	inline GLint getUniformLocation(const std::string& name) {
		auto locIt = unfiromLocations.find(name);
		if (locIt != unfiromLocations.end())
			return locIt->second;
		GLint loc = glGetUniformLocation(program, name.c_str());
		unfiromLocations.insert({ name, loc });
		return loc;
	}

	Shader(GLuint program) : program(program) { unfiromLocations.reserve(64); }

public:
	inline void use() { glUseProgram(program); }

	inline void setInt(std::string&& name, int val) {
		BREAK_IF_SHADER_IS_NOT_ACTIVE()
		glUniform1i(getUniformLocation(name), val );
	}
	inline void setFloat(std::string&& name, float val) {
		BREAK_IF_SHADER_IS_NOT_ACTIVE()
		glUniform1f(getUniformLocation(name), val);
	}

	inline void setVector2F(std::string&& name, float x, float y) {
		BREAK_IF_SHADER_IS_NOT_ACTIVE();
		glUniform2f(getUniformLocation(name), x, y);
	}
	inline void setVector3F(std::string&& name, float x, float y, float z) {
		BREAK_IF_SHADER_IS_NOT_ACTIVE();
		glUniform3f(getUniformLocation(name), x, y, z);
	}
	inline void setVector4F(std::string&& name, float x, float y, float z, float w) {
		BREAK_IF_SHADER_IS_NOT_ACTIVE();
		glUniform4f(getUniformLocation(name), x, y, z, w);
	}

	inline void setVector2F(std::string&& name, glm::vec2 vec) {
		BREAK_IF_SHADER_IS_NOT_ACTIVE();
		glUniform2f(getUniformLocation(name), vec.x, vec.y);
	}
	inline void setVector3F(std::string&& name, glm::vec3 vec) {
		BREAK_IF_SHADER_IS_NOT_ACTIVE();
		glUniform3f(getUniformLocation(name), vec.x, vec.y, vec.z);
	}
	inline void setVector4F(std::string&& name, glm::vec4 vec) {
		BREAK_IF_SHADER_IS_NOT_ACTIVE();
		glUniform4f(getUniformLocation(name), vec.x, vec.y, vec.z, vec.w);
	}

	inline void setMat4(std::string&& name, const glm::mat4& mat) {
		BREAK_IF_SHADER_IS_NOT_ACTIVE();
		glUniformMatrix4fv(getUniformLocation(name), 1, GL_FALSE, glm::value_ptr(mat));
	}

	// maybe for later usage
	/*
	void setTexture(std::string name, shared_ptr<Texture> texture, unsigned int slotId);
	void setCubemap(std::string name, shared_ptr<Cubemap> cubemap, unsigned int slotId);

	void bindUBO(std::string name, GLuint bindingPoint);
	*/

	~Shader()
	{ glDeleteProgram(program); }
};