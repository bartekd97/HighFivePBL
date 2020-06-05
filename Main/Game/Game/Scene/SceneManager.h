#pragma once

#include <memory>
#include <string>

class IScene;

namespace SceneManager
{
	void Initialize();

	void RegisterScene(std::string name, std::shared_ptr<IScene> scene);
	void RequestLoadScene(std::string name);
}