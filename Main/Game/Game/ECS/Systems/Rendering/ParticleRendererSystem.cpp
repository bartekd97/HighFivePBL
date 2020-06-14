#include "ECS/Components/ParticleContainer.h"
#include "ECS/Components/ParticleRenderer.h"
#include "Resourcing/UniformBuffer.h"
#include "HFEngine.h"
#include "ParticleRendererSystem.h"

static struct ParticlesBinding
{
	static const GLuint BUFFER = 2;

	static const GLuint TEXTURE = 1;
	static const GLuint COLOR = 2;
	static const GLuint OPACITY = 3;
};

inline static void CheckParticlesBuffer(ParticleContainer& container, ParticleRenderer& renderer)
{
	container.business.Wait();
	if (container.lastUpdate > renderer.lastUpdate || renderer.particlesBuffer == nullptr)
	{
		if (renderer.particlesBuffer == nullptr)
		{
			renderer.particlesBuffer = UniformBuffer::Create(sizeof(Particle) * ParticleContainer::MAX_ALLOWED_PARTICLES);
		}

		renderer.particlesBuffer->uploadData(
			(char*)container.particles.data(),
			sizeof(Particle) * container.particles.size()
		);
		renderer.lastUpdate = HFEngine::CURRENT_FRAME_NUMBER;
	}
}

void ParticleRendererSystem::Init()
{
	// create particle quad
	static const float quadVertices[] = {
		// positions        // texture Coords
		-0.5f,  0.5f, 0.0f, 0.0f, 1.0f,
		-0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
		 0.5f,  0.5f, 0.0f, 1.0f, 1.0f,
		 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
	};
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glBindVertexArray(0);

	// get shader
	particleShader = ShaderManager::GetShader("ParticleRender");
	particleShader->use();
	particleShader->bindUniformBlockPoint("gParticlesBuffer", ParticlesBinding::BUFFER);
	particleShader->setInt("gSpriteSheet", ParticlesBinding::TEXTURE);
	particleShader->setInt("gColor", ParticlesBinding::COLOR);
	particleShader->setInt("gOpacity", ParticlesBinding::OPACITY);
}


unsigned int ParticleRendererSystem::Render(Camera& viewCamera)
{
	unsigned int rendered = 0;
	particleShader->use();

	glm::mat4 viewMatrix = viewCamera.GetViewMatrix();
	glm::mat4 projectionMatrix = viewCamera.GetProjectionMatrix();
	// http://www.opengl-tutorial.org/intermediate-tutorials/billboards-particles/billboards/#solution-2--the-3d-way
	glm::vec3 cameraRightWorld = { viewMatrix[0][0], viewMatrix[1][0], viewMatrix[2][0] };
	glm::vec3 cameraUpWorld = { viewMatrix[0][1], viewMatrix[1][1], viewMatrix[2][1] };

	particleShader->setMat4("gView", viewMatrix);
	particleShader->setMat4("gProjection", projectionMatrix);
	particleShader->setVector3F("gCameraRight", cameraRightWorld);
	particleShader->setVector3F("gCameraUp", cameraUpWorld);

	ParticleRenderer::BlendingMode lastBlendingMode = ParticleRenderer::BlendingMode::ADD;

	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	glBindVertexArray(VAO);
	auto currentFrame = HFEngine::CURRENT_FRAME_NUMBER;
	auto it = gameObjects.begin();
	while (it != gameObjects.end())
	{
		auto gameObject = *(it++);

		auto& container = HFEngine::ECS.GetComponent<ParticleContainer>(gameObject);
		auto& renderer = HFEngine::ECS.GetComponent<ParticleRenderer>(gameObject);

		if (renderer.cullingData.lastUpdate != currentFrame)
			continue;
		if (!renderer.cullingData.visibleByViewCamera)
			continue;
		if (container._activeParticles == 0)
			continue;

		if (lastBlendingMode != renderer.blendingMode)
		{
			lastBlendingMode = renderer.blendingMode;
			switch (lastBlendingMode)
			{
			case ParticleRenderer::BlendingMode::BLEND:
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
				break;
			case ParticleRenderer::BlendingMode::ADD:
			default:
				glBlendFunc(GL_SRC_ALPHA, GL_ONE);
				break;
			}
		}

		CheckParticlesBuffer(container, renderer);
		renderer.particlesBuffer->bind(ParticlesBinding::BUFFER);
		renderer.spriteSheet->bind(ParticlesBinding::TEXTURE);
		renderer.colorOverTime->bind(ParticlesBinding::COLOR);
		renderer.opacityOverTime->bind(ParticlesBinding::OPACITY);
		particleShader->setInt("gSpriteSheetCount", renderer.spriteSheetCount);

		glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, container.particles.size());
		rendered++;
	}
	glBindVertexArray(0);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	return rendered;
}
