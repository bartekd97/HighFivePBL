#pragma once

#include <glad/glad.h>
#include <vector>
#include <memory>
#include "Resourcing/Texture.h"
#include "WindowManager.h"

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
	std::vector<ColorAttachement> colorAttachementsConfig;
	std::shared_ptr<Texture> depthAttachementTexture;

	void rebindColorAttachements();

	FrameBuffer(int width, int height) : width(width), height(height) {}
public:
	static std::shared_ptr<FrameBuffer> Create(
		int width, int height,
		const std::vector<ColorAttachement>& colorAttachements,
		DepthAttachement depthAttachment);


	inline void bind() {
		glViewport(0,0,width,height);
		glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
	}
	inline void bindRead() {
		glBindFramebuffer(GL_READ_FRAMEBUFFER, frameBuffer);
	}
	inline void bindDraw() {
		glViewport(0, 0, width, height);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, frameBuffer);
	}

	static inline void BindDefaultScreen() {
		glViewport(0, 0, WindowManager::SCREEN_WIDTH, WindowManager::SCREEN_HEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	inline std::vector<std::shared_ptr<Texture>> getColorAttachements() {
		return colorAttachementTextures;
	}
	inline std::shared_ptr<Texture> getColorAttachement(int index) {
		return colorAttachementTextures[index];
	}
	inline std::shared_ptr<Texture> getDepthAttachement() {
		return depthAttachementTexture;
	}

	// return color attachement as independent texture, replacing it with new one
	std::shared_ptr<Texture> popColorAttachement(int index);

	static void BlitDepth(std::shared_ptr<FrameBuffer> from, std::shared_ptr<FrameBuffer> to = nullptr);
	static void BlitColor(std::shared_ptr<FrameBuffer> from, std::shared_ptr<FrameBuffer> to = nullptr, int readIndex = 0);

	~FrameBuffer()
	{
		glDeleteFramebuffers(1, &frameBuffer);
		if (depthRenderBuffer != 0)
			glDeleteRenderbuffers(1, &depthRenderBuffer);
	}
};
