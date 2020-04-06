#include <string>
#include <unordered_map>
#include <stdexcept>
#include <tinyxml2.h>
#include "Shader.h"
#include "../Utility/Utility.h"
#include "../Utility/Logger.h"

using namespace tinyxml2;

// variables
namespace ShaderManager {
	const char* CONFIG_FILE = "Data/Shaders/config.xml";

	std::string VERTEX_PATH = "Data/Shaders/Vertex/";
	std::string FRAGMENT_PATH = "Data/Shaders/Fragment/";

	bool Initialized = false;

	std::unordered_map<std::string, std::shared_ptr<Shader>> ShaderContainer;
}

// privates
namespace ShaderManager {
	bool TryMakeProgram(GLuint& shaderProgram,
		const char* vertexShaderContent,
		const char* geometryShaderContent,
		const char* fragmentShaderContent)
	{
		GLint successStatus;
		static GLchar errorLog[512];
		int errorCount = 0;

		GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertexShader, 1, &vertexShaderContent, NULL);
		glCompileShader(vertexShader);

		glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &successStatus);
		if (!successStatus)
		{
			glGetShaderInfoLog(vertexShader, 512, NULL, errorLog);
			LogError("ShaderManager::MakeProgram(): VertexShader Compile Problem:\n{}", std::string(errorLog));
			errorCount++;
		}

		GLuint geometryShader;
		if (geometryShaderContent != NULL)
		{
			geometryShader = glCreateShader(GL_GEOMETRY_SHADER);
			glShaderSource(geometryShader, 1, &geometryShaderContent, NULL);
			glCompileShader(geometryShader);

			glGetShaderiv(geometryShader, GL_COMPILE_STATUS, &successStatus);
			if (!successStatus)
			{
				glGetShaderInfoLog(geometryShader, 512, NULL, errorLog);
				LogError("ShaderManager::MakeProgram(): GeometryShader Compile Problem:\n{}", std::string(errorLog));
				errorCount++;
			}
		}


		GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragmentShader, 1, &fragmentShaderContent, NULL);
		glCompileShader(fragmentShader);

		glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &successStatus);
		if (!successStatus)
		{
			glGetShaderInfoLog(fragmentShader, 512, NULL, errorLog);
			LogError("ShaderManager::MakeProgram(): FragmentShader Compile Problem:\n{}", std::string(errorLog));
			errorCount++;
		}


		shaderProgram = glCreateProgram();
		if (errorCount == 0)
		{
			glAttachShader(shaderProgram, vertexShader);
			if (geometryShaderContent != NULL)
				glAttachShader(shaderProgram, geometryShader);
			glAttachShader(shaderProgram, fragmentShader);
			glLinkProgram(shaderProgram);

			glGetProgramiv(shaderProgram, GL_LINK_STATUS, &successStatus);
			if (!successStatus)
			{
				glGetProgramInfoLog(shaderProgram, 512, NULL, errorLog);
				LogError("ShaderManager::MakeProgram(): Shader Link Problem:\n{}", std::string(errorLog));
				errorCount++;
			}
		}

		glDeleteShader(vertexShader);
		if (geometryShaderContent != NULL)
			glDeleteShader(geometryShader);
		glDeleteShader(fragmentShader);

		return errorCount == 0;
	}
}

// publics
void ShaderManager::Initialize()
{
	if (Initialized)
	{
		LogWarning("ShaderManager::Initialize(): Already initialized");
		return;
	}

	XMLDocument doc;
	auto loaded = doc.LoadFile(CONFIG_FILE);
	if (loaded != XML_SUCCESS)
	{
		LogError("ShaderManager::Initialize(): Cannot load xml config file. Code: {}", loaded);
		return;
	}

	auto root = doc.FirstChildElement();
	if (root == nullptr)
	{
		LogError("ShaderManager::Initialize(): Cannot find root element in XML config file.");
		return;
	}

	int i = 0;
	for (XMLElement* node = root->FirstChildElement("shader");
		node != nullptr;
		node = node->NextSiblingElement("shader"), i++)
	{
		const XMLAttribute* attrName = node->FindAttribute("name");
		const XMLAttribute* attrVertex = node->FindAttribute("vertex");
		const XMLAttribute* attrFragment = node->FindAttribute("fragment");

		if (attrName == nullptr || attrVertex == nullptr || attrFragment == nullptr)
		{
			LogWarning("ShaderManager::Initialize(): Invalid shader node at #{}", i);
			continue;
		}

		std::string shaderName = attrName->Value();

		std::string contentVertex;
		std::string contentFragment;
		if (!Utility::ReadTextFile(VERTEX_PATH + attrVertex->Value(), contentVertex)
			|| !Utility::ReadTextFile(FRAGMENT_PATH + attrFragment->Value(), contentFragment))
		{
			LogWarning("ShaderManager::Initialize(): Cannot load source files for shader '{}'", shaderName);
			continue;
		}

		GLuint shaderProgram;
		if (!TryMakeProgram(shaderProgram, contentVertex.c_str(), NULL, contentFragment.c_str()))
		{
			LogWarning("ShaderManager::Initialize(): Failed creating shader '{}'", shaderName);
			continue;
		}

		Shader* shader = new Shader(shaderProgram);
		ShaderContainer[shaderName] = std::shared_ptr<Shader>(shader);
		LogInfo("Initialized shader '{}'.", shaderName);
	}

	Initialized = true;

	LogInfo("ShaderManager initialized.");
}

std::shared_ptr<Shader> ShaderManager::GetShader(std::string name)
{
	try
	{
		return ShaderContainer.at(name);
	}
	catch (std::out_of_range ex)
	{
		LogWarning("ShaderManager::GetShader(): Cannot find shader '{}'", name);
		return nullptr;
	}
}
