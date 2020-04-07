#pragma once

#include <string>
#include <memory>
#include <unordered_map>
#include <vector>
#include <memory>
#include <functional>
#include <glm/glm.hpp>
#include "ECS/ECSTypes.h"


class Prefab;
class IPrefabComponentLoader
{
public:
	virtual void Preprocess(std::unordered_map<std::string,std::string>& properties) {}
	virtual void Create(GameObject target) = 0;
};

namespace PrefabManager {
	void Initialize();

	extern std::shared_ptr<Prefab> GENERIC_PREFAB;

	std::shared_ptr<Prefab> GetPrefab(std::string name);
	void RegisterComponentLoader(std::string name, std::function<std::shared_ptr<IPrefabComponentLoader>()> provider);
}