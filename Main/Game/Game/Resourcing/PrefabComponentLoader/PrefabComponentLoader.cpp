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

		void Preprocess(PropertyReader& properties) override {
			static std::string model;
			if (properties.GetString("model",model,"")) {
				auto parts = Utility::StringSplit(model, ':');
				if (parts.size() == 2) {
					libraryName = parts[0];
					modelName = parts[1];
					useModel = true;
				}
				else {
					LogWarning("MeshRendererLoader::Preprocess(): Cannot parse 'model' value: {}", model);
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
		float mass;

		void Preprocess(PropertyReader& properties) override
		{
			static std::string tmpString;

			velocity = glm::vec3(0.0f);
			acceleration = glm::vec3(0.0f);
			mass = 10.0f;

			if (properties.GetString("velocity", tmpString, "0.0,0.0,0.0"))
			{
				auto parts = Utility::StringSplit(tmpString, ',');
				if (parts.size() == 3) {
					velocity = glm::vec3(std::stof(parts[0]), std::stof(parts[1]), std::stof(parts[2]));
				}
				else {
					LogWarning("RigidBodyLoader::Preprocess(): Cannot parse 'velocity' value: {}", tmpString);
				}
			}
			if (properties.GetString("acceleration", tmpString, "0.0,0.0,0.0"))
			{
				auto parts = Utility::StringSplit(tmpString, ',');
				if (parts.size() == 3) {
					acceleration = glm::vec3(std::stof(parts[0]), std::stof(parts[1]), std::stof(parts[2]));
				}
				else {
					LogWarning("RigidBodyLoader::Preprocess(): Cannot parse 'acceleration' value: {}", tmpString);
				}
			}
			if (properties.GetString("mass", tmpString, "0.0"))
			{
				mass = std::stof(tmpString);
			}
		}

		void Create(GameObject target) override
		{
			RigidBody rb;
			rb.velocity = velocity;
			rb.acceleration = acceleration;
			rb.mass = mass;
			HFEngine::ECS.AddComponent<RigidBody>(target, rb);
		}
	};

	class CircleColliderLoader : public IPrefabComponentLoader
	{
	public:
		float radius;
		bool frozen;
		Collider::ColliderTypes type;

		void Preprocess(PropertyReader& properties) override
		{
			static std::string tmpString;
			radius = 0.0f;
			frozen = false;
			type = Collider::ColliderTypes::DYNAMIC;

			if (properties.GetString("radius", tmpString, "0.0"))
			{
				radius = std::stof(tmpString);
			}
			if (properties.GetString("frozen", tmpString))
			{
				if (tmpString.compare("true") == 0) frozen = true;
			}
			if (properties.GetString("type", tmpString))
			{
				if (tmpString.compare("STATIC") == 0) type = Collider::ColliderTypes::STATIC;
				else if (tmpString.compare("TRIGGER") == 0) type = Collider::ColliderTypes::TRIGGER;
				else if (tmpString.compare("DYNAMIC") != 0) LogWarning("BoxColliderLoader::Preprocess(): unknown collider type: {}", tmpString);
			}
		}

		void Create(GameObject target) override
		{
			Collider collider;
			collider.type = type;
			collider.shape = Collider::ColliderShapes::CIRCLE;
			collider.frozen = frozen;
			CircleCollider cc;
			cc.radius = radius;
			HFEngine::ECS.AddComponent<Collider>(target, collider);
			HFEngine::ECS.AddComponent<CircleCollider>(target, cc);
		}
	};

	class BoxColliderLoader : public IPrefabComponentLoader
	{
	public:
		float width;
		float height;
		bool frozen;
		Collider::ColliderTypes type;

		void Preprocess(PropertyReader& properties) override
		{
			static std::string tmpString;
			width = 0.0f;
			height = 0.0f;
			frozen = false;
			type = Collider::ColliderTypes::DYNAMIC;

			if (properties.GetString("width", tmpString, "0.0"))
			{
				width = std::stof(tmpString);
			}
			if (properties.GetString("height", tmpString, "0.0"))
			{
				height = std::stof(tmpString);
			}
			if (properties.GetString("frozen", tmpString))
			{
				auto xD = tmpString.compare("true");
				if (tmpString.compare("true") == 0) frozen = true;
			}
			if (properties.GetString("type", tmpString))
			{
				if (tmpString.compare("STATIC") == 0) type = Collider::ColliderTypes::STATIC;
				else if (tmpString.compare("TRIGGER") == 0) type = Collider::ColliderTypes::TRIGGER;
				else if (tmpString.compare("DYNAMIC") != 0) LogWarning("BoxColliderLoader::Preprocess(): unknown collider type: {}", tmpString);
			}
		}

		void Create(GameObject target) override
		{
			Collider collider;
			collider.type = type;
			collider.shape = Collider::ColliderShapes::BOX;
			collider.frozen = frozen;
			BoxCollider bc;
			bc.width = width;
			bc.height = height;
			HFEngine::ECS.AddComponent<Collider>(target, collider);
			HFEngine::ECS.AddComponent<BoxCollider>(target, bc);
		}
	};

	class GravityColliderLoader : public IPrefabComponentLoader
	{
	public:
		std::vector<std::pair<float, float>> heights;

		void Preprocess(PropertyReader& properties) override
		{
			static std::string tmpString;
			std::vector<float> ZValues;
			std::vector<float> heightValues;

			if (properties.GetString("ZValues", tmpString))
			{
				auto parts = Utility::StringSplit(tmpString, ',');
				for (auto& part : parts)
				{
					ZValues.push_back(std::stof(part));
				}
			}

			if (properties.GetString("heights", tmpString))
			{
				auto parts = Utility::StringSplit(tmpString, ',');
				for (auto& part : parts)
				{
					heightValues.push_back(std::stof(part));
				}
			}

			if (ZValues.size() == heightValues.size())
			{
				for (int i = 0; i < ZValues.size(); i++)
				{
					heights.push_back(std::make_pair(ZValues[i], heightValues[i]));
				}
			}
			else
			{
				LogWarning("GravityColliderLoader::Preprocess(): sizes of ZValues and heights doesn't match");
			}
		}

		void Create(GameObject target) override
		{
			if (heights.empty())
			{
				LogWarning("GravityColliderLoader::Create(): empty heights");
				return;
			}
			Collider collider;
			collider.shape = Collider::ColliderShapes::GRAVITY;
			collider.type = Collider::ColliderTypes::STATIC;
			collider.frozen = true;
			GravityCollider gc;
			gc.heights = heights;
			HFEngine::ECS.AddComponent<Collider>(target, collider);
			HFEngine::ECS.AddComponent<GravityCollider>(target, gc);
		}
	};

	class ScriptComponentLoader : public IPrefabComponentLoader
	{
	public:
		std::string name;
		std::unordered_map<std::string, std::string> rawProperties;

		void Preprocess(PropertyReader& properties) override
		{
			if (!properties.GetString("name", name, ""))
			{
				LogWarning("ScriptComponentLoader::Preprocess(): script name empty");
			}
			rawProperties = properties.GetRawCopy();
			rawProperties.erase("name");
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
				auto script = scriptContainer.AddScript(target, name);
				for (auto property : rawProperties)
				{
					bool set = false;
					if (IsFloat(property.second)) set = script->SetFloat(property.first, stof(property.second));
					if (!set) set = script->SetString(property.first, property.second);
					if (!set)
					{
						LogWarning("ScriptComponentLoader::Create(): script {} has no property named {}", name, property.first);
					}
				}
				if (script->GetUnsettedParams() > 0)
				{
					LogWarning("ScriptComponentLoader::Create(): script {} has unsetted parameters", name);
				}
			}
		}
	private:
		bool IsFloat(const std::string& str)
		{
			if (str.empty()) return false;

			char* ptr;
			strtof(str.c_str(), &ptr);
			return (*ptr) == '\0';
		}
	};

} // end of namespace


void PrefabComponentLoader::RegisterLoaders()
{
	PrefabManager::RegisterComponentLoader("MeshRenderer", []() { return std::make_shared<MeshRendererLoader>(); });
	PrefabManager::RegisterComponentLoader("RigidBody", []() { return std::make_shared<RigidBodyLoader>(); });
	PrefabManager::RegisterComponentLoader("CircleCollider", []() { return std::make_shared<CircleColliderLoader>(); });
	PrefabManager::RegisterComponentLoader("BoxCollider", []() { return std::make_shared<BoxColliderLoader>(); });
	PrefabManager::RegisterComponentLoader("GravityCollider", []() { return std::make_shared<GravityColliderLoader>(); });
	PrefabManager::RegisterComponentLoader("ScriptComponent", []() { return std::make_shared<ScriptComponentLoader>(); });
}
