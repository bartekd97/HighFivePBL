#pragma once

#include "Widget.h"
#include "TextureColor.h"

class Panel : public Widget
{
public:
	Panel();

	void Draw() override;
	void Update(const glm::vec2& mousePosition) override;

	void SetCircleFilling(float filling);
	float GetCircleFilling();

	TextureColor textureColor;
private:
	static GLuint vao;
	static GLuint vboVertices;

	float circleFilling;
};
