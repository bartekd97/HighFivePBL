#pragma once
#include <memory>
#include <assert.h>
#include <glad/glad.h>

class UniformBuffer {
private:
	GLuint UBO;
	unsigned int bufferSize;

	UniformBuffer(GLuint UBO, unsigned int size) : UBO(UBO), bufferSize(size) {}

public:
	inline static std::shared_ptr<UniformBuffer> Create(unsigned int size)
	{
		GLuint ubo;
		glGenBuffers(1, &ubo);
		glBindBuffer(GL_UNIFORM_BUFFER, ubo);
		glBufferData(GL_UNIFORM_BUFFER, size, NULL, GL_DYNAMIC_DRAW);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
		return std::shared_ptr<UniformBuffer>(new UniformBuffer(ubo, size));
	}

	inline void bind(GLuint bindingPoint) {
		glBindBufferBase(GL_UNIFORM_BUFFER, bindingPoint, UBO);
	}

	inline void uploadData(char* data) {
		glBindBuffer(GL_UNIFORM_BUFFER, UBO);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, bufferSize, data);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}
	inline void uploadData(char* data, unsigned int size) {
		assert(size <= bufferSize && "Possible buffer overflow");
		glBindBuffer(GL_UNIFORM_BUFFER, UBO);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, size, data);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}
	inline void uploadData(char* data, unsigned int offset, unsigned int size) {
		assert((offset+size) <= bufferSize && "Possible buffer overflow");
		glBindBuffer(GL_UNIFORM_BUFFER, UBO);
		glBufferSubData(GL_UNIFORM_BUFFER, offset, size, data);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}


	~UniformBuffer()
	{
		glDeleteBuffers(1, &UBO);
	}
};