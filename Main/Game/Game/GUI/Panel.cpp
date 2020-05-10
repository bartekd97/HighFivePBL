#include "Panel.h"

#include "GUIManager.h"
#include "../WindowManager.h"

GLuint Panel::vao;
GLuint Panel::vboVertices;

Panel::Panel()
{
	static bool initialized = false;

	if (!initialized)
	{
		float vertices[] = {
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

	textureColor.texture = GUIManager::defaultTexture;
	textureColor.color = glm::vec4(1.0f);
}

void Panel::Update(const glm::vec2& mousePosition)
{
	Widget::Update(mousePosition);
}

void Panel::Draw()
{
	GUIManager::guiShader->use();

	glm::vec3 screenPos((GetAbsolutePosition().x / WindowManager::SCREEN_WIDTH) * 2.0f - 1.0f, -(GetAbsolutePosition().y / WindowManager::SCREEN_HEIGHT) * 2.0f + 1.0f, 0.0f);
	glm::vec2 screenSize(size.x / WindowManager::SCREEN_WIDTH, size.y / WindowManager::SCREEN_HEIGHT);

	glm::mat4 mat(1.0f);

	mat = glm::translate(mat, screenPos);
	mat = glm::scale(mat, glm::vec3(screenSize.x * 2.0f, screenSize.y * 2.0f, 1.0f));
	GUIManager::guiShader->setVector4F("uColor", textureColor.color);
	GUIManager::guiShader->setMat4("model", mat);

	textureColor.texture->bind(0);

	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLES, 0, 6);
}
