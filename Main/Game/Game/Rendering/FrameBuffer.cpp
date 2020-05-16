#include "FrameBuffer.h"
#include "../Utility/Logger.h"

void FrameBuffer::rebindColorAttachements()
{
	if (colorAttachementTextures.size() == 0)
	{
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
	}
	else
	{
		std::vector<unsigned int> attachementsIDs;
		int tid = 0;
		for (auto ct : colorAttachementTextures)
		{
			unsigned int attachementId = GL_COLOR_ATTACHMENT0 + tid;
			glFramebufferTexture2D(
				GL_FRAMEBUFFER,
				attachementId,
				GL_TEXTURE_2D,
				ct->id(),
				0
				);
			attachementsIDs.push_back(attachementId);
			tid++;
		}
		glDrawBuffers(attachementsIDs.size(), &attachementsIDs[0]);
	}
}

std::shared_ptr<FrameBuffer> FrameBuffer::Create(
	int width, int height,
	const std::vector<ColorAttachement>& colorAttachements,
	FrameBuffer::DepthAttachement depthAttachment)
{
	// create framebuffer
	auto fb = std::shared_ptr<FrameBuffer>(new FrameBuffer(width, height));
	fb->colorAttachementsConfig = colorAttachements;

	glGenFramebuffers(1, &fb->frameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, fb->frameBuffer);

	// assign color attachemnts
	for (auto ca : colorAttachements)
	{
		auto texture = TextureManager::CreateEmptyTexture(width, height, ca.dataFormat, ca.dataType, ca.internalFormat, GL_LINEAR);
		fb->colorAttachementTextures.push_back(texture);
	}
	fb->rebindColorAttachements();

	// create depth buffer
	switch (depthAttachment)
	{
	case FrameBuffer::DepthAttachement::CREATE_TEXTURE:
	{
		fb->depthAttachementTexture =
			TextureManager::CreateEmptyTexture(width, height, GL_DEPTH_COMPONENT, GL_FLOAT, GL_DEPTH_COMPONENT);
		glFramebufferTexture2D(
			GL_FRAMEBUFFER,
			GL_DEPTH_ATTACHMENT,
			GL_TEXTURE_2D,
			fb->depthAttachementTexture->id(),
			0
			);
		fb->depthRenderBuffer = 0;
		break;
	}
	case FrameBuffer::DepthAttachement::DEFAULT:
	default:
	{
		glGenRenderbuffers(1, &fb->depthRenderBuffer);
		glBindRenderbuffer(GL_RENDERBUFFER, fb->depthRenderBuffer);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);

		glFramebufferRenderbuffer(
			GL_FRAMEBUFFER,
			GL_DEPTH_ATTACHMENT,
			GL_RENDERBUFFER,
			fb->depthRenderBuffer
			);
		fb->depthAttachementTexture = nullptr;
		break;
	}
	}

	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE)
	{
		LogError("FrameBuffer::Create(): Failed creating framebuffer with #'{}' color attachements and '{}' depth attachement. Reason: {}", colorAttachements.size(), (int)depthAttachment, (int)status);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	return fb;
}

std::shared_ptr<Texture> FrameBuffer::popColorAttachement(int index)
{
	assert(index < colorAttachementTextures.size() && "out of range");

	auto current = colorAttachementTextures[index];
	auto ca = colorAttachementsConfig[index];
	auto replacement = TextureManager::CreateEmptyTexture(width, height, ca.dataFormat, ca.dataType, ca.internalFormat, GL_LINEAR);
	colorAttachementTextures[index] = replacement;
	rebindColorAttachements();
	return current;
}

void FrameBuffer::BlitDepth(std::shared_ptr<FrameBuffer> from, std::shared_ptr<FrameBuffer> to)
{
	glBindFramebuffer(GL_READ_FRAMEBUFFER, from->frameBuffer);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, to == nullptr ? 0 : to->frameBuffer);
	glBlitFramebuffer(
		0, 0, from->width, from->height,
		0, 0, to == nullptr ? WindowManager::SCREEN_WIDTH : to->width, to == nullptr ? WindowManager::SCREEN_HEIGHT : to->height,
		GL_DEPTH_BUFFER_BIT, GL_NEAREST
		);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FrameBuffer::BlitColor(std::shared_ptr<FrameBuffer> from, std::shared_ptr<FrameBuffer> to, int readIndex)
{
	glBindFramebuffer(GL_READ_FRAMEBUFFER, from->frameBuffer);
	glReadBuffer(GL_COLOR_ATTACHMENT0 + readIndex);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, to == nullptr ? 0 : to->frameBuffer);
	glBlitFramebuffer(
		0, 0, from->width, from->height,
		0, 0, to == nullptr ? WindowManager::SCREEN_WIDTH : to->width, to == nullptr ? WindowManager::SCREEN_HEIGHT :to->height,
		GL_COLOR_BUFFER_BIT, GL_NEAREST
		);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}