#pragma once

#include <string>
#include <memory>
#include <unordered_map>
#include <glad/glad.h>

class Texture;
class TextureLibrary;
struct TextureConfig {
	GLint filteringMin = GL_LINEAR;
	GLint filteringMag = GL_LINEAR;
	GLint format = GL_RGB;
	bool generateMipmaps = true;
};


namespace TextureManager {
	void Initialize();

	extern std::shared_ptr<TextureLibrary> GENERIC_LIBRARY;
	extern std::shared_ptr<Texture> BLANK_TEXTURE; // 4x4 white (255) rgb texture

	std::shared_ptr<Texture> CreateTextureFromRawData(unsigned char* data, int width, int height, GLint dataFormat, TextureConfig& config);
	std::shared_ptr<Texture> CreateEmptyTexture(int width, int height, GLint dataFormat, GLenum dataType, GLint internalFormat, GLenum filtering = GL_LINEAR);
	std::shared_ptr<Texture> CreateTextureFromFile(std::string filename, TextureConfig& config);
	std::shared_ptr<TextureLibrary> GetLibrary(std::string name);
	std::shared_ptr<Texture> GetTexture(std::string libraryName, std::string textureName);
}

class TextureLibrary {
	friend void TextureManager::Initialize();
	friend std::shared_ptr<TextureLibrary> TextureManager::GetLibrary(std::string name);

public:
	const std::string name;

private:
	struct LibraryEntity {
		std::string path;
		std::weak_ptr<Texture> texture;
		TextureConfig config;
	};
	std::unordered_map<std::string, LibraryEntity*> entities;

	TextureLibrary();
	TextureLibrary(std::string name);
	std::shared_ptr<Texture> LoadEntity(std::string& name, LibraryEntity* entity);
public:
	~TextureLibrary();
	std::shared_ptr<Texture> GetTexture(std::string name);
};