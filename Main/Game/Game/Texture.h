#pragma once

#include "TextureManager.h"

class Texture {
	friend void TextureManager::Initialize();
	friend std::shared_ptr<Texture> TextureManager::CreateTextureFromRawData(unsigned char* data, int width, int height, GLint dataFormat, TextureConfig& config);
	friend std::shared_ptr<Texture> TextureManager::CreateTextureFromFile(std::string filename, TextureConfig& config);
	
private:
	GLuint texture;

public:
	const int width;
	const int height;
	const GLint format;

private:
	Texture(GLuint texture, int width, int height, GLint format) :
		texture(texture), width(width), height(height), format(format) {}

public:
	inline void bind(unsigned int slotId) {
		glActiveTexture(GL_TEXTURE0 + slotId);
		glBindTexture(GL_TEXTURE_2D, texture);
	}

	inline GLuint id() { return texture; }

	inline void setFiltering(GLint min, GLint mag) {
		glActiveTexture(GL_TEXTURE31); // use last slot to prevent overwriting bound texture on current slot
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, min);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mag);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	inline void setCompare(GLint mode, GLint func) {
		glActiveTexture(GL_TEXTURE31);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, mode);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, func);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	inline void setEdges(GLint vertical, GLint horizontal) {
		glActiveTexture(GL_TEXTURE31);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, vertical);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, horizontal);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	inline void setBorderColor(float r, float g, float b, float a) {
		glActiveTexture(GL_TEXTURE31);
		float borderColor[] = { r, g, b, a };
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
		glBindTexture(GL_TEXTURE_2D, 0);
	}


	~Texture()
	{ glDeleteTextures(1, &texture); }
};