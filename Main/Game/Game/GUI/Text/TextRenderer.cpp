#include <map>
#include <ft2build.h>
#include FT_FREETYPE_H

#include "TextRenderer.h"
#include "../../Utility/Logger.h"
#include "../../Resourcing/ShaderManager.h"
#include "../../Resourcing/Shader.h"
#include "../../HFEngine.h"

namespace TextRenderer
{
	FT_Library ft;
	std::shared_ptr<Shader> textShader;
	std::map<std::string, std::shared_ptr<Font>> fonts;
	std::shared_ptr<Font> currentFont;
	unsigned int VAO, VBO;
	bool initialized = false;

	void Initialize()
	{
		if (initialized)
		{
			LogWarning("TextRenderer already initialized");
			return;
		}

		if (FT_Init_FreeType(&ft))
		{
			LogError("TextRenderer::Initialize(): failed to initialize FreeType");
			return;
		}

		textShader = ShaderManager::GetShader("TextShader");

		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		initialized = true;
	}

	void Terminate()
	{
		if (initialized)
		{
			FT_Done_FreeType(ft);
		}
	}

	void LoadFont(std::string name, std::string path)
	{
		std::shared_ptr<Font> font = std::make_shared<Font>(ft, path.c_str());
		if (font->IsInitialized())
		{
			fonts[name] = font;
		}
	}

	void SetFont(std::string fontName)
	{
		auto fontIt = fonts.find(fontName);
		if (fontIt == fonts.end())
		{
			LogWarning("TextRenderer::RenderText(): font not found");
			return;
		}
		currentFont = fontIt->second;
	}

	std::shared_ptr<Font> GetCurrentFont()
	{
		return currentFont;
	}

	void RenderText(std::string text, float x, float y, float scale, glm::vec3 color)
	{
		if (currentFont == nullptr)
		{
			LogWarning("TextRednerer::RenderText(): font not setted");
			return;
		}

		textShader->use();
		textShader->setVector3F("textColor", color);
		glActiveTexture(GL_TEXTURE0);

		glBindVertexArray(VAO);

		// iterate through all characters
		std::string::const_iterator c;
		for (c = text.begin(); c != text.end(); c++)
		{
			Character ch = currentFont->GetCharacter(*c);

			float xpos = x + ch.Bearing.x * scale;
			float ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

			float w = ch.Size.x * scale;
			float h = ch.Size.y * scale;
			// update VBO for each character
			float vertices[6][4] = {
				{ xpos,     ypos + h,   0.0f, 0.0f },
				{ xpos,     ypos,       0.0f, 1.0f },
				{ xpos + w, ypos,       1.0f, 1.0f },

				{ xpos,     ypos + h,   0.0f, 0.0f },
				{ xpos + w, ypos,       1.0f, 1.0f },
				{ xpos + w, ypos + h,   1.0f, 0.0f }
			};
			// render glyph texture over quad
			glBindTexture(GL_TEXTURE_2D, ch.TextureID);
			// update content of VBO memory
			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); // be sure to use glBufferSubData and not glBufferData

			glBindBuffer(GL_ARRAY_BUFFER, 0);
			// render quad
			glDrawArrays(GL_TRIANGLES, 0, 6);
			// now advance cursors for next glyph (note that advance is number of 1/64 pixels)
			x += (ch.Advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
		}
		glBindVertexArray(0);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void UpdateProjection()
	{
		textShader->use();
		textShader->setMat4("projection", glm::ortho(0.0f, static_cast<float>(WindowManager::SCREEN_WIDTH), 0.0f, static_cast<float>(WindowManager::SCREEN_HEIGHT)));
	}
}
