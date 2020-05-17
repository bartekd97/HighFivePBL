#include "PrefabComponentLoader.h"
#include "Resourcing/Prefab.h"
#include "HFEngine.h"
#include "ECS/Components.h"
#include "Utility/Utility.h"
#include "Utility/Logger.h"
#include "Resourcing/Model.h"

namespace {

	class ModelHolderLoader : public IPrefabComponentLoader {
	public:
		std::string libraryName, modelName;

		void Preprocess(PropertyReader& properties) override {
			static std::string model;
			if (properties.GetString("model", model, "")) {
				auto parts = Utility::StringSplit(model, ':');
				if (parts.size() == 2) {
					libraryName = parts[0];
					modelName = parts[1];
				}
				else {
					LogWarning("ModelHolderLoader::Preprocess(): Cannot parse 'model' value: {}", model);
				}
			}
			else {
				LogWarning("ModelHolderLoader::Preprocess(): Missing 'model' value.");
			}
		}
		void Create(GameObject target) override {
			ModelHolder holder;
			holder.model = ModelManager::GetModel(libraryName, modelName);
			HFEngine::ECS.AddComponent<ModelHolder>(target, holder);
		}
	};

	class MeshRendererLoader : public IPrefabComponentLoader {
	protected:
		virtual inline std::string _cName() { return "ModelHolderLoader"; }
	public:
		bool configureFromHolder = false;

		bool useMeshPath = false;
		std::pair<std::string, std::string> meshPath;

		bool useMaterialPath = false;
		std::pair<std::string, std::string> materialPath;

		virtual void Preprocess(PropertyReader& properties) override {
			properties.GetBool("configureFromHolder", configureFromHolder, false);

			static std::string tmpMeshPath;
			if (properties.GetString("mesh", tmpMeshPath,"")) {
				auto parts = Utility::StringSplit(tmpMeshPath, ':');
				if (parts.size() == 2) {
					meshPath = { parts[0], parts[1] };
					useMeshPath = true;
				}
				else {
					LogWarning("{}::Preprocess(): Cannot parse 'mesh' value: {}", _cName(), tmpMeshPath);
				}
			}

			static std::string tmpMaterialPath;
			if (properties.GetString("material", tmpMaterialPath, "")) {
				auto parts = Utility::StringSplit(tmpMaterialPath, ':');
				if (parts.size() == 2) {
					materialPath = { parts[0], parts[1] };
					useMaterialPath = true;
				}
				else {
					LogWarning("{}::Preprocess(): Cannot parse 'material' value: {}", _cName(), tmpMaterialPath);
				}
			}
		}

		virtual void Create(GameObject target) override {
			MeshRenderer renderer;
			if (configureFromHolder) {
				auto& holder = HFEngine::ECS.GetComponent<ModelHolder>(target);
				renderer.mesh = holder.model->mesh;
				renderer.material = holder.model->material;
			}
			if (useMeshPath) {
				auto model = ModelManager::GetModel(meshPath.first, meshPath.second);
				renderer.mesh = model->mesh;
			}
			if (useMaterialPath) {
				auto material = MaterialManager::GetMaterial(materialPath.first, materialPath.second);
				renderer.material = material;
			}
			assert(
				renderer.mesh != nullptr &&
				renderer.material != nullptr &&
				"Missing MeshRenderer configuration"
				);
			HFEngine::ECS.AddComponent<MeshRenderer>(target, renderer);
		}
	};
	class SkinnedMeshRendererLoader : public MeshRendererLoader {
	protected:
		virtual inline std::string _cName() override { return "SkinnedMeshRendererLoader"; }
	public:
		virtual void Preprocess(PropertyReader& properties) override {
			MeshRendererLoader::Preprocess(properties);
		}

		virtual void Create(GameObject target) override {
			SkinnedMeshRenderer renderer;
			if (configureFromHolder) {
				auto& holder = HFEngine::ECS.GetComponent<ModelHolder>(target);
				renderer.mesh = holder.model->mesh;
				renderer.skinningData = holder.model->skinningData;
				renderer.material = holder.model->material;
			}
			if (useMeshPath) {
				auto model = ModelManager::GetModel(meshPath.first, meshPath.second);
				renderer.mesh = model->mesh;
				renderer.skinningData = model->skinningData;
			}
			if (useMaterialPath) {
				auto material = MaterialManager::GetMaterial(materialPath.first, materialPath.second);
				renderer.material = material;
			}
			assert(
				renderer.mesh != nullptr &&
				renderer.material != nullptr &&
				renderer.skinningData != nullptr &&
				"Missing SkinnedMeshRenderer configuration"
				);
			HFEngine::ECS.AddComponent<SkinnedMeshRenderer>(target, renderer);
		}
	};

	class SkinAnimatorLoader : public IPrefabComponentLoader {
	public:
		bool configureFromHolder = false;

		bool useClipsPath = false;
		std::pair<std::string, std::string> clipsPath;

		virtual void Preprocess(PropertyReader& properties) override {
			properties.GetBool("configureFromHolder", configureFromHolder, false);

			static std::string tmpClipsPath;
			if (properties.GetString("clips", tmpClipsPath, "")) {
				auto parts = Utility::StringSplit(tmpClipsPath, ':');
				if (parts.size() == 2) {
					clipsPath = { parts[0], parts[1] };
					useClipsPath = true;
				}
				else {
					LogWarning("SkinAnimatorLoader::Preprocess(): Cannot parse 'clips' value: {}", tmpClipsPath);
				}
			}
		}

		virtual void Create(GameObject target) override {
			SkinAnimator animator;
			if (configureFromHolder) {
				auto& holder = HFEngine::ECS.GetComponent<ModelHolder>(target);
				animator.clips = holder.model->animations;
			}
			if (useClipsPath) {
				auto model = ModelManager::GetModel(clipsPath.first, clipsPath.second);
				animator.clips = model->animations;
			}
			HFEngine::ECS.AddComponent<SkinAnimator>(target, animator);
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
	PrefabManager::RegisterComponentLoader("ModelHolder", []() { return std::make_shared<ModelHolderLoader>(); });
	PrefabManager::RegisterComponentLoader("MeshRenderer", []() { return std::make_shared<MeshRendererLoader>(); });
	PrefabManager::RegisterComponentLoader("SkinnedMeshRenderer", []() { return std::make_shared<SkinnedMeshRendererLoader>(); });
	PrefabManager::RegisterComponentLoader("SkinAnimator", []() { return std::make_shared<SkinAnimatorLoader>(); });

	PrefabManager::RegisterComponentLoader("RigidBody", []() { return std::make_shared<RigidBodyLoader>(); });
	PrefabManager::RegisterComponentLoader("CircleCollider", []() { return std::make_shared<CircleColliderLoader>(); });
	PrefabManager::RegisterComponentLoader("BoxCollider", []() { return std::make_shared<BoxColliderLoader>(); });
	PrefabManager::RegisterComponentLoader("ScriptComponent", []() { return std::make_shared<ScriptComponentLoader>(); });
}
