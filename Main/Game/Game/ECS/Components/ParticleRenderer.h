#pragma once

#include <memory>
#include <vector>
#include <glm/glm.hpp>
#include "Resourcing/Texture.h"
#include "CulledRenderer.h"

class UniformBuffer;
struct ParticleRenderer
{
	enum class BlendingMode {
		ADD,
		BLEND
	};
	BlendingMode blendingMode = BlendingMode::ADD;

	std::shared_ptr<UniformBuffer> particlesBuffer;
	FrameCounter lastUpdate = 0;

	std::shared_ptr<Texture> spriteSheet;
	int spriteSheetCount = 1;

	std::shared_ptr<Texture> colorOverTime;
	std::shared_ptr<Texture> opacityOverTime;

	float visibilityRadius = 5.0f;

	ParticleRenderer()
	{
		spriteSheet = TextureManager::BLANK_TEXTURE;
		colorOverTime = TextureManager::BLANK_TEXTURE;
		opacityOverTime = TextureManager::BLANK_TEXTURE;
	}

	CulledRenderer cullingData;
	AABBStruct localAABB;
};