#pragma once

#include "Widget.h"
#include "TextureColor.h"
#include "../Resourcing/Texture.h"
#include "../Resourcing/Shader.h"

class Button : public Widget
{
public:
	// TODO: ogar te enumy zjebane
	enum class STATE
	{
		NORMAL,
		PRESSED,
		HOVER
	};

	Button();

	void Update(const glm::vec2& mousePosition) override;
	void Draw() override;

	STATE state;
	// TODO: tutaj te� wyjeb t� map� i zr�b co� normalnego jak ju� b�dziesz ogarnia�  te enumy zjebane
	std::unordered_map<STATE, TextureColor> textureColors;

	std::function<void()> OnClickListener;
	std::function<void(STATE)> OnStateChanged;
private:
	static GLuint vao;
	static GLuint vboVertices;
};
