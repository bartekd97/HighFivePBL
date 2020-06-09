#pragma once

#include <string>
#include <memory>
#include <unordered_map>
#include <vector>
#include <memory>
#include <functional>
#include <glm/glm.hpp>
#include "ECS/ECSTypes.h"
#include "Utility/PropertyReader.h"


class Prefab;
class IPrefabComponentLoader
{
public:
	virtual void Preprocess(PropertyReader& properties) {}
	virtual void Warm() {}
	virtual void Create(GameObject target) = 0;
};

namespace PrefabManager {
	void Initialize();

	extern std::shared_ptr<Prefab> GENERIC_PREFAB;

	std::shared_ptr<Prefab> GetPrefab(std::string name);
	void RegisterComponentLoader(std::string name, std::function<std::shared_ptr<IPrefabComponentLoader>()> provider);
}