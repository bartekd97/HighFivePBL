#pragma once

#include <memory>
#include <vector>
#include <glm/glm.hpp>
#include "Resourcing/Texture.h"

class UniformBuffer;
struct ParticleRenderer
{
	std::shared_ptr<UniformBuffer> particlesBuffer;
	FrameCounter lastUpdate = 0;

	std::shared_ptr<Texture> spriteSheet;
	int spriteSheetCount = 1;

	std::shared_ptr<Texture> colorOverTime;
	std::shared_ptr<Texture> opacityOverTime;

	ParticleRenderer()
	{
		spriteSheet = TextureManager::BLANK_TEXTURE;
		colorOverTime = TextureManager::BLANK_TEXTURE;
		opacityOverTime = TextureManager::BLANK_TEXTURE;
	}
};