#include <filesystem>
#include <tinyxml2.h>
#include <stb_image.h>
#include "../Global.h"
#include "../Utility/Logger.h"
#include "Texture.h"

using namespace tinyxml2;

// variables
namespace TextureManager {
	std::shared_ptr<TextureLibrary> GENERIC_LIBRARY;
	std::shared_ptr<Texture> BLANK_TEXTURE; // 4x4 white (255) rgb texture

	unsigned char* BLANK_TEXTURE_DATA = new unsigned char[4*4*3] {
		255,255,255, 255,255,255, 255,255,255, 255,255,255,
		255,255,255, 255,255,255, 255,255,255, 255,255,255,
		255,255,255, 255,255,255, 255,255,255, 255,255,255,
		255,255,255, 255,255,255, 255,255,255, 255,255,255,
	};

	bool Initialized = false;

	std::unordered_map<std::string, std::shared_ptr<TextureLibrary>> LibraryContainer;
}

// privates
namespace TextureManager {
	GLuint MakeEmptyTexture2D(int width, int height, GLint dataFormat, GLenum dataType, GLint internalFormat, GLenum filtering)
	{
		GLuint textureId;

		glGenTextures(1, &textureId);
		glActiveTexture(GL_TEXTURE31); // use last slot to prevent accidential rewriting current texture
		glBindTexture(GL_TEXTURE_2D, textureId);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filtering);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filtering);

		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat,
			width, height,
			0, // deprecated
			dataFormat, dataType, NULL);

		glBindTexture(GL_TEXTURE_2D, 0);

		return textureId;
	}
	GLuint MakeTexture2DFromData(unsigned char* data, int width, int height, GLint dataFormat, TextureConfig& config)
	{
		GLuint textureId;

		glGenTextures(1, &textureId);
		glActiveTexture(GL_TEXTURE31); // use last slot to prevent accidential rewriting current texture
		glBindTexture(GL_TEXTURE_2D, textureId);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, config.filteringMin);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, config.filteringMag);

		glTexImage2D(GL_TEXTURE_2D, 0, config.format,
			width, height,
			0, // deprecated
			dataFormat, GL_UNSIGNED_BYTE, data);
		if (config.generateMipmaps)
			glGenerateMipmap(GL_TEXTURE_2D);

		glBindTexture(GL_TEXTURE_2D, 0);

		return textureId;
	}
}

// publics
void TextureManager::Initialize()
{
	if (Initialized)
	{
		LogWarning("TextureManager::Initialize(): Already initialized");
		return;
	}

	GENERIC_LIBRARY = std::shared_ptr<TextureLibrary>(new TextureLibrary());
	TextureConfig blankTextureConfig;
	BLANK_TEXTURE = CreateTextureFromRawData(BLANK_TEXTURE_DATA, 4, 4, GL_RGB, blankTextureConfig);

	Initialized = true;

	LogInfo("TextureManager initialized.");
}


std::shared_ptr<Texture> TextureManager::CreateTextureFromRawData(unsigned char* data, int width, int height, GLint dataFormat, TextureConfig& config)
{
	// TODO: Logging when texture creation failed
	GLuint textureId = MakeTexture2DFromData(data, width, height, dataFormat, config);
	return std::shared_ptr<Texture>(new Texture(textureId, width, height, config.format));
}
std::shared_ptr<Texture> TextureManager::CreateEmptyTexture(int width, int height, GLint dataFormat, GLenum dataType, GLint internalFormat, GLenum filtering)
{
	// TODO: Logging when texture creation failed
	GLuint textureId = MakeEmptyTexture2D(width, height, dataFormat, dataType, internalFormat, filtering);
	return std::shared_ptr<Texture>(new Texture(textureId, width, height, internalFormat));
}
std::shared_ptr<Texture> TextureManager::CreateTextureFromFile(std::string filename, TextureConfig& config)
{
	int width, height, noChannels;
	stbi_set_flip_vertically_on_load(true);
	unsigned char* texData = stbi_load(filename.c_str(), &width, &height, &noChannels, 0);

	if (texData == NULL)
	{
		const char* failreason = stbi_failure_reason();
		if (failreason == NULL)
			failreason = "Unknown error";
		LogError("TextureManager::CreateTextureFromFile(): Cannot load {}, Reason: {}", filename, failreason);
		return BLANK_TEXTURE;
	}

	GLint dataFormat = GL_RGB; // default (3 channels)
	if (noChannels == 1)
		dataFormat = GL_RED;
	else if (noChannels == 2)
		dataFormat = GL_RG;
	else if (noChannels == 4)
		dataFormat = GL_RGBA;

	GLuint textureId = MakeTexture2DFromData(texData, width, height, dataFormat, config);

	stbi_image_free(texData);

	return std::shared_ptr<Texture>(new Texture(textureId, width, height, config.format));
}

std::shared_ptr<TextureLibrary> TextureManager::GetLibrary(std::string name)
{
#ifdef _DEBUG
	for (auto lib : LibraryContainer)
	{
		const char* key = lib.first.c_str();
		if (_stricmp(key, name.c_str()) == 0 &&
			strcmp(key, name.c_str()) != 0)
		{
			LogWarning("TextureLibrary::GetLibrary(): Trying to get duplciate of '{}' library but with different letter size. It causes dupliacate and increased memory usage!!!", name);
		}
	}
#endif
	try
	{
		return LibraryContainer.at(name);
	}
	catch (std::out_of_range ex)
	{
		if (std::filesystem::is_directory(Global::ASSETS_DIRECTORY + name))
		{
			LibraryContainer[name] = std::shared_ptr<TextureLibrary>(new TextureLibrary(name));
			return LibraryContainer[name];
		}
		else
		{
			LogWarning("TextureManager::GetLibrary(): Cannot find library '{}'", name);
			return GENERIC_LIBRARY;
		}
	}
}

std::shared_ptr<Texture> TextureManager::GetTexture(std::string libraryName, std::string textureName)
{
	return TextureManager::GetLibrary(libraryName)->GetTexture(textureName);
}


// classes

TextureLibrary::TextureLibrary() : name("GENERIC")
{}
TextureLibrary::TextureLibrary(std::string name) : name(name)
{
	const std::string libraryFolder = Global::ASSETS_DIRECTORY + name + "/";

	XMLDocument doc;
	auto loaded = doc.LoadFile((libraryFolder + "textures.xml").c_str());
	if (loaded != XML_SUCCESS)
	{
		LogError("TextureLibrary::TextureLibrary(): Cannot load textures.xml for '{}'. Code: {}", name, loaded);
		return;
	}

	auto root = doc.FirstChildElement();
	if (root == nullptr)
	{
		LogError("TextureLibrary::TextureLibrary(): Cannot find root element in textures.xml for '{}'", name);
		return;
	}

	int i = 0;
	for (XMLElement* node = root->FirstChildElement("texture");
		node != nullptr;
		node = node->NextSiblingElement("texture"), i++)
	{
		const char* textureName = node->Attribute("name");
		const char* textureFile = node->Attribute("file");
		
		if (textureName == nullptr || textureFile == nullptr)
		{
			LogWarning("TextureLibrary::TextureLibrary(): Invalid texture node at #{}", i);
			continue;
		}

		std::string filepath = libraryFolder + textureFile;
		if (!std::filesystem::exists(filepath))
		{
			LogWarning("TextureLibrary::TextureLibrary(): No file found for texture '{}'", textureName);
			continue;
		}

		LibraryEntity* entity = new LibraryEntity();
		entity->path = filepath;

		if (node->Attribute("format", "srgb"))
			entity->config.format = GL_SRGB8;
		else if (node->Attribute("format", "srgba"))
			entity->config.format = GL_SRGB8_ALPHA8;
		else if (node->Attribute("format", "r"))
			entity->config.format = GL_R8;
		else if (node->Attribute("format", "rg"))
			entity->config.format = GL_RG8;
		else if (node->Attribute("format", "rgba"))
			entity->config.format = GL_RGBA8;
		
		entity->config.filteringMin = GL_LINEAR_MIPMAP_LINEAR;
		entity->config.filteringMag = GL_LINEAR;
		entity->config.generateMipmaps = true;

		entities[textureName] = entity;
	}
	LogInfo("TextureLibrary::TextureLibrary(): Initialized '{}' library with {} entities", name, entities.size());
}

TextureLibrary::~TextureLibrary()
{
	for (auto e : entities)
		delete e.second;
}

std::shared_ptr<Texture> TextureLibrary::LoadEntity(std::string& name, LibraryEntity* entity)
{
	auto ptr = TextureManager::CreateTextureFromFile(entity->path, entity->config);
	entity->texture = ptr;
	LogInfo("TextureLibrary::LoadEntity(): Loaded '{}' in '{}'", name, this->name);
	return ptr;
}

std::shared_ptr<Texture> TextureLibrary::GetTexture(std::string name)
{
	if (this->name == "GENERIC")
		return TextureManager::BLANK_TEXTURE;
	try
	{
		LibraryEntity* entity = entities.at(name);
		if (entity->texture.expired())
			return LoadEntity(name, entity);
		else
			return entity->texture.lock();
	}
	catch (std::out_of_range ex)
	{
		LogWarning("TextureLibrary::GetTexture(): Cannot find texture '{}' in '{}' library", name, this->name);
		return TextureManager::BLANK_TEXTURE;
	}
}