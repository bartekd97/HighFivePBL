#include "FrameBuffer.h"
#include "Logger.h"

std::shared_ptr<FrameBuffer> FrameBuffer::Create(
	int width, int height,
	std::vector<ColorAttachement> colorAttachements,
	FrameBuffer::DepthAttachement depthAttachment)
{
	// create framebuffer
	auto fb = std::shared_ptr<FrameBuffer>(new FrameBuffer(width, height));

	glGenFramebuffers(1, &fb->frameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, fb->frameBuffer);

	// assign color attachemnts
	if (colorAttachements.size() == 0)
	{
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
	}
	else
	{
		std::vector<unsigned int> attachementsIDs;
		for (auto ca : colorAttachements)
		{
			auto texture = TextureManager::CreateEmptyTexture(width, height, ca.dataFormat, ca.dataType, ca.internalFormat);

			unsigned int attachementId = GL_COLOR_ATTACHMENT0 + fb->colorAttachementTextures.size();
			glFramebufferTexture2D(
				GL_FRAMEBUFFER,
				attachementId,
				GL_TEXTURE_2D,
				texture->id(),
				0
			);
			attachementsIDs.push_back(attachementId);
			fb->colorAttachementTextures.push_back(texture);
		}
		glDrawBuffers(attachementsIDs.size(), &attachementsIDs[0]);
	}

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

void FrameBuffer::BlitDepth(std::shared_ptr<FrameBuffer> from, std::shared_ptr<FrameBuffer> to)
{
	glBindFramebuffer(GL_READ_FRAMEBUFFER, from->frameBuffer);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, to == nullptr ? 0 : to->frameBuffer);
	glBlitFramebuffer(
		0, 0, from->width, from->height, 0, 0, to->width, to->height, GL_DEPTH_BUFFER_BIT, GL_NEAREST
		);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
