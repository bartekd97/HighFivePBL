#pragma once

#include <vector>
#include <memory>
#include <glm/glm.hpp>

class Texture;

namespace TextureTools
{
	std::shared_ptr<Texture> GenerateGradientTexture(std::vector<float> floats);
	std::shared_ptr<Texture> GenerateGradientTexture(std::vector<glm::vec3> vec3s);
}