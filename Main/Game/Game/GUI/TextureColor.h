#pragma once

#include <memory>
#include <glm/glm.hpp>
#include "../Resourcing/Texture.h"

//Texture resizing mode?
struct TextureColor
{
	std::shared_ptr<Texture> texture;
	glm::vec4 color;
};
