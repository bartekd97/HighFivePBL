#include "Button.h"
#include "../InputManager.h"
#include "../WindowManager.h"
#include "../Utility/Logger.h"
#include "Text/TextRenderer.h"

#include "GUIManager.h"

GLuint Button::vao;
GLuint Button::vboVertices;

Button::Button()
{
	static bool initialized = false;

	if (!initialized)
	{
		float vertices[] = {
			0.0f, -1.0f, 0.0f,  0.0f, 0.0f,
			1.0f, -1.0f, 0.0f,  1.0f, 0.0f,
			1.0f, 0.0f, 0.0f,  1.0f, 1.0f,

			1.0f, 0.0f, 0.0f,  1.0f, 1.0f,
			0.0f, 0.0f, 0.0f,  0.0f, 1.0f,
			0.0f, -1.0f, 0.0f,  0.0f, 0.0f
		};

		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);

		glGenBuffers(1, &vboVertices);
		glBindBuffer(GL_ARRAY_BUFFER, vboVertices);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)nullptr);
		glEnableVertexAttribArray(0);

		glVertexAttribPointer(1, 2, GL_FLOAT, GL_TRUE, 5 * sizeof(float), (const GLvoid*)(3 * sizeof(GLfloat)));
		glEnableVertexAttribArray(1);

		glBindVertexArray(0);


		initialized = true;
	}

	// TODO: ogar to gówno razem z enumami zjebanymi
	for (int i = (int)STATE::NORMAL; i <= (int)STATE::HOVER; i++)
	{
		TextureColor tc;
		tc.color = glm::vec4(1.0f);
		textureColors[(STATE)i] = tc;
	}
	state = STATE::NORMAL;
	textureColors[state].texture = GUIManager::defaultTexture;
}

void Button::Update(const glm::vec2& mousePosition)
{
	bool isOver = IsMouseOver(mousePosition);
	bool isClicked = InputManager::GetMouseButtonState(GLFW_MOUSE_BUTTON_LEFT);

	if (!isOver)
	{
		if (state != STATE::NORMAL && OnStateChanged) OnStateChanged(STATE::NORMAL);
		state = STATE::NORMAL;
	}
	else
	{
		if (isClicked)
		{
			if (state == STATE::HOVER && OnClickListener)
			{
				OnClickListener();
			}
			if (state != STATE::PRESSED && OnStateChanged) OnStateChanged(STATE::PRESSED);
			state = STATE::PRESSED;
		}
		else
		{
			if (state != STATE::HOVER && OnStateChanged) OnStateChanged(STATE::HOVER);
			state = STATE::HOVER;
		}
	}

	Widget::Update(mousePosition);
}

void Button::Draw()
{
	GUIManager::guiShader->use();

	glm::vec3 screenPos((GetAbsolutePosition().x / WindowManager::SCREEN_WIDTH) * 2.0f - 1.0f, -(GetAbsolutePosition().y / WindowManager::SCREEN_HEIGHT) * 2.0f + 1.0f, 0.0f);
	glm::vec2 screenSize(GetSize().x / WindowManager::SCREEN_WIDTH, GetSize().y / WindowManager::SCREEN_HEIGHT);

	glm::mat4 mat(1.0f);
	mat = glm::translate(mat, screenPos);
	mat = glm::scale(mat, glm::vec3(screenSize.x * 2.0f, screenSize.y * 2.0f, 1.0f));
	GUIManager::guiShader->setVector4F("uColor", textureColors[state].color);
	GUIManager::guiShader->setMat4("model", mat);

	if (textureColors[state].texture != nullptr)
	{
		textureColors[state].texture->bind(0);
	}
	else
	{
		textureColors[STATE::NORMAL].texture->bind(0);
	}

	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLES, 0, 6);
}
