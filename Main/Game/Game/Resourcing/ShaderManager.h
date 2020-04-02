#pragma once

#include <memory>
#include <string>

class Shader;

namespace ShaderManager {
	void Initialize();

	std::shared_ptr<Shader> GetShader(std::string name);
}
