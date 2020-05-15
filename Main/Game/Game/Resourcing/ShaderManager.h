#pragma once

#include <memory>
#include <string>

class Shader;

namespace ShaderManager {
	void Initialize();

	extern std::shared_ptr<Shader> NULL_SHADER; // shader program with id 0

	std::shared_ptr<Shader> GetShader(std::string name);
}
