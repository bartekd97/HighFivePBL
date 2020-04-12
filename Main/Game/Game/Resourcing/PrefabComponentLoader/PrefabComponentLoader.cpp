#include "PrefabComponentLoader.h"
#include "Resourcing/Prefab.h"
#include "HFEngine.h"
#include "ECS/Components.h"
#include "Utility/Utility.h"
#include "Utility/Logger.h"
#include "Resourcing/Model.h"

namespace {

	class MeshRendererLoader : public IPrefabComponentLoader {
	public:
		bool useModel = false;
		std::string libraryName, modelName;

		void Preprocess(std::unordered_map<std::string,std::string>& properties) override {
			if (properties.contains("model")) {
				auto parts = Utility::StringSplit(properties["model"], ':');
				if (parts.size() == 2) {
					libraryName = parts[0];
					modelName = parts[1];
					useModel = true;
				}
				else {
					LogWarning("MeshRendererLoader::Preprocess(): Cannot parse 'model' value: {}", properties["model"]);
				}
			}
		}
		void Create(GameObject target) override {
			MeshRenderer renderer;
			if (useModel) {
				auto model = ModelManager::GetModel(libraryName, modelName);
				renderer.mesh = model->mesh;
				renderer.material = model->material;
			}
			HFEngine::ECS.AddComponent<MeshRenderer>(target, renderer);
		}
	};

	class RigidBodyLoader : public IPrefabComponentLoader
	{
	public:
		glm::vec3 velocity, acceleration;

		void Preprocess(std::unordered_map<std::string, std::string>& properties) override
		{
			velocity = glm::vec3(0.0f);
			acceleration = glm::vec3(0.0f);

			if (properties.contains("velocity"))
			{
				auto parts = Utility::StringSplit(properties["velocity"], ',');
				if (parts.size() == 3) {
					velocity = glm::vec3(std::stof(parts[0]), std::stof(parts[1]), std::stof(parts[2]));
				}
				else {
					LogWarning("RigidBodyLoader::Preprocess(): Cannot parse 'velocity' value: {}", properties["velocity"]);
				}
			}
			if (properties.contains("acceleration"))
			{
				auto parts = Utility::StringSplit(properties["acceleration"], ',');
				if (parts.size() == 3) {
					acceleration = glm::vec3(std::stof(parts[0]), std::stof(parts[1]), std::stof(parts[2]));
				}
				else {
					LogWarning("RigidBodyLoader::Preprocess(): Cannot parse 'acceleration' value: {}", properties["acceleration"]);
				}
			}
		}

		void Create(GameObject target) override
		{
			RigidBody rb;
			rb.velocity = velocity;
			rb.acceleration = acceleration;
			HFEngine::ECS.AddComponent<RigidBody>(target, rb);
		}
	};

	class CircleColliderLoader : public IPrefabComponentLoader
	{
	public:
		float radius;

		void Preprocess(std::unordered_map<std::string, std::string>& properties) override
		{
			radius = 0.0f;

			if (properties.contains("radius"))
			{
				radius = std::stof(properties["radius"]);
			}
		}

		void Create(GameObject target) override
		{
			CircleCollider cc;
			cc.radius = radius;
			HFEngine::ECS.AddComponent<CircleCollider>(target, cc);
		}
	};

	class ScriptComponentLoader : public IPrefabComponentLoader
	{
	public:
		std::string name;

		void Preprocess(std::unordered_map<std::string, std::string>& properties) override
		{
			if (!properties.contains("name"))
			{
				LogWarning("ScriptComponentLoader::Preprocess(): script name empty");
			}
			else
			{
				name = properties["name"];
			}
		}

		void Create(GameObject target) override
		{
			if (name.length() > 0)
			{
				if (!HFEngine::ECS.SearchComponent<ScriptContainer>(target))
				{
					HFEngine::ECS.AddComponent<ScriptContainer>(target, {});
				}
				auto& scriptContainer = HFEngine::ECS.GetComponent<ScriptContainer>(target);
				scriptContainer.AddScript(target, name);
			}
		}
	};

} // end of namespace


void PrefabComponentLoader::RegisterLoaders()
{
	PrefabManager::RegisterComponentLoader("MeshRenderer", []() { return std::make_shared<MeshRendererLoader>(); });
	PrefabManager::RegisterComponentLoader("RigidBody", []() { return std::make_shared<RigidBodyLoader>(); });
	PrefabManager::RegisterComponentLoader("CircleCollider", []() { return std::make_shared<CircleColliderLoader>(); });
	PrefabManager::RegisterComponentLoader("ScriptComponent", []() { return std::make_shared<ScriptComponentLoader>(); });
}
