#pragma once

#include <glad/glad.h>
#include <vector>
#include <memory>
#include "../Resourcing/Texture.h"
#include "../WindowManager.h"

class FrameBuffer
{
public:
	struct ColorAttachement {
		GLint internalFormat;
		GLenum dataFormat;
		GLenum dataType;
	};
	enum class DepthAttachement {
		DEFAULT,
		CREATE_TEXTURE
	};

public:
	const int width;
	const int height;

private:
	GLuint frameBuffer = 0;
	GLuint depthRenderBuffer = 0;

	std::vector<std::shared_ptr<Texture>> colorAttachementTextures;
	std::shared_ptr<Texture> depthAttachementTexture;

	FrameBuffer(int width, int height) : width(width), height(height) {}
public:
	static std::shared_ptr<FrameBuffer> Create(
		int width, int height,
		std::vector<ColorAttachement> colorAttachements,
		DepthAttachement depthAttachment);


	inline void bind() {
		glViewport(0,0,width,height);
		glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
	}

	static inline void BindDefaultScreen() {
		glViewport(0, 0, WindowManager::SCREEN_WIDTH, WindowManager::SCREEN_HEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	inline std::vector<std::shared_ptr<Texture>> getColorAttachements() {
		return colorAttachementTextures;
	}
	inline std::shared_ptr<Texture> getDepthAttachement() {
		return depthAttachementTexture;
	}

	static void BlitDepth(std::shared_ptr<FrameBuffer> from, std::shared_ptr<FrameBuffer> to = nullptr);

	~FrameBuffer()
	{
		glDeleteFramebuffers(1, &frameBuffer);
		if (depthRenderBuffer != 0)
			glDeleteRenderbuffers(1, &depthRenderBuffer);
	}
};
