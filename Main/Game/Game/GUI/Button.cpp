#include "Button.h"
#include "../InputManager.h"
#include "../Resourcing/ShaderManager.h"
#include "../WindowManager.h"
#include "../Utility/Logger.h"

#include "GUIManager.h"

std::shared_ptr<Shader> Button::shader = nullptr;
GLuint Button::vao;
GLuint Button::vboVertices;

Button::Button()
{
	static bool initialized = false;

	if (!initialized)
	{
		shader = ShaderManager::GetShader("GUIShader");

		float vertices[] = {
			/*-0.5f, 0.5f, 0.0f,  0.0f, 0.0f,
			0.5f, 0.5f, 0.0f,  0.0f, 1.0f,
			0.5f, -0.5f, 0.0f,  1.0f, 1.0f,

			0.5f, -0.5f, 0.0f,  1.0f, 1.0f,
			-0.5f, -0.5f, 0.0f,  1.0f, 0.0f,
			-0.5f, 0.5f, 0.0f,  0.0f, 0.0f*/
			/*-1.0f, 1.0f ,0.0f,  0.0f, 0.0f,
			1.0f, 1.0f, 0.0f,  0.0f, 1.0f,
			1.0f, -1.0f, 0.0f,  1.0f, 1.0f,

			1.0f, -1.0f, 0.0f,  1.0f, 1.0f,
			-1.0f, -1.0f, 0.0f,  1.0f, 0.0f,
			-1.0f, 1.0f, 0.0f,  0.0f, 0.0f*/
			/*-1.0f, 1.0f, 0.0f,  0.0f, 0.0f,
			1.0f, 1.0f, 0.0f,  0.0f, 1.0f,
			1.0f, -1.0f, 0.0f,  1.0f, 1.0f,

			1.0f, -1.0f, 0.0f,  1.0f, 1.0f,
			-1.0f, -1.0f, 0.0f,  1.0f, 0.0f,
			-1.0f, 1.0f, 0.0f,  0.0f, 0.0f*/
			0.0f, -1.0f, 0.0f,  0.0f, 0.0f,
			1.0f, -1.0f, 0.0f,  0.0f, 1.0f,
			1.0f, 0.0f, 0.0f,  1.0f, 1.0f,

			1.0f, 0.0f, 0.0f,  1.0f, 1.0f,
			0.0f, 0.0f, 0.0f,  1.0f, 0.0f,
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

	// TODO: ogar to g�wno razem z enumami zjebanymi
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
	//LogInfo("xD {}", isOver);
	//LogInfo("xD {} {}", mousePosition.x, mousePosition.y);
	bool isClicked = InputManager::GetMouseButtonState(GLFW_MOUSE_BUTTON_LEFT);

	if (!isOver)
	{
		state = STATE::NORMAL;
	}
	else
	{
		if (isClicked)
		{
			if (state == STATE::HOVER)
			{
				state = STATE::PRESSED;
				//onClick
			}
		}
		else
		{
			state = STATE::HOVER;
		}
	}

	//Widget::Update(mousePosition);
}

void Button::Draw()
{
	shader->use();

	//glm::vec3 screenPos(2.0f * (GetPosition().x / WindowManager::SCREEN_WIDTH) - 1.0f, 2.0f * (1.0f - GetPosition().y / WindowManager::SCREEN_HEIGHT) - 1.0f, 0.0f);
	//glm::vec3 screenPos((2.0f * GetPosition().x + 1.0f) / WindowManager::SCREEN_WIDTH - 1.0f, (2.0f * GetPosition().y + 1.0f) / WindowManager::SCREEN_HEIGHT, 0.0f);
	glm::vec3 screenPos((GetPosition().x / WindowManager::SCREEN_WIDTH) * 2.0f, -(GetPosition().y / WindowManager::SCREEN_HEIGHT) * 2.0f, 0.0f);
	glm::vec2 screenSize(size.x / WindowManager::SCREEN_WIDTH, size.y / WindowManager::SCREEN_HEIGHT);

	glm::mat4 mat(1.0f);

	if (GetAnchor() == Anchor::TOPLEFT)
	{
		mat = glm::translate(mat, glm::vec3(-1.0f, 1.0f, 0.0f));
	}

	mat = glm::translate(mat, screenPos);
	mat = glm::scale(mat, glm::vec3(screenSize.x * 2.0f, screenSize.y * 2.0f, 1.0f));
	shader->setVector4F("uColor", textureColors[state].color);
	shader->setMat4("model", mat);

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