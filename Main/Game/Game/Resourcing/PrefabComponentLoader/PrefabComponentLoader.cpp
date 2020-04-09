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

	class ScriptComponentLoader : public IPrefabComponentLoader
	{
	public:
		std::string name;

		void Preprocess(std::unordered_map<std::string, std::string>& properties) override {
			if (!properties.contains("name"))
			{
				LogWarning("ScriptComponentLoader::Preprocess(): script name empty");
			}
			else
			{
				name = properties["name"];
			}
		}

		void Create(GameObject target) override {
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
	PrefabManager::RegisterComponentLoader("MeshRenderer", []() {return std::make_shared<MeshRendererLoader>();});
	PrefabManager::RegisterComponentLoader("ScriptComponent", []() {return std::make_shared<ScriptComponentLoader>(); });
}
