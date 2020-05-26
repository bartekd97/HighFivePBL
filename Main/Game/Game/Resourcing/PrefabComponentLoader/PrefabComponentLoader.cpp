#include "PrefabComponentLoader.h"
#include "Resourcing/Prefab.h"
#include "HFEngine.h"
#include "ECS/Components.h"
#include "Utility/Utility.h"
#include "Utility/TextureTools.h"
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
		bool castShadows = true;

		bool useMeshPath = false;
		std::pair<std::string, std::string> meshPath;

		bool useMaterialPath = false;
		std::pair<std::string, std::string> materialPath;

		virtual void Preprocess(PropertyReader& properties) override {
			properties.GetBool("configureFromHolder", configureFromHolder, false);
			properties.GetBool("castShadows", castShadows, true);

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
			renderer.castShadows = castShadows;
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
			renderer.castShadows = castShadows;
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


	class PointLightRendererLoader : public IPrefabComponentLoader {
	public:
		PointLight light;

		virtual void Preprocess(PropertyReader& properties) override {
			if (!properties.GetVec3("color", light.color, light.color)) {
				LogWarning("PointLightRendererLoader::Preprocess(): Missing 'color' value. Using default: {}", light.color);
			}
			if (!properties.GetFloat("radius", light.radius, light.radius)) {
				LogWarning("PointLightRendererLoader::Preprocess(): Missing 'radius' value. Using default: {}", light.radius);
			}
			if (!properties.GetFloat("intensity", light.intensity, light.intensity)) {
				LogWarning("PointLightRendererLoader::Preprocess(): Missing 'intensity' value. Using default: {}", light.intensity);
			}
			if (!properties.GetFloat("shadowIntensity", light.shadowIntensity, light.shadowIntensity)) {
				LogWarning("PointLightRendererLoader::Preprocess(): Missing 'shadowIntensity' value. Using default: {}", light.shadowIntensity);
			}
		}

		virtual void Create(GameObject target) override {
			PointLightRenderer renderer;
			renderer.light = light;
			HFEngine::ECS.AddComponent<PointLightRenderer>(target, renderer);
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



	class ParticleContainerLoader : public IPrefabComponentLoader {
	public:
		int maxParticles = 0;

		virtual void Preprocess(PropertyReader& properties) override {
			if (!properties.GetInt("maxParticles", maxParticles, maxParticles)) {
				LogWarning("ParticleContainerLoader::Preprocess(): Missing 'maxParticles' value. Using default: {}", maxParticles);
			}
			assert(maxParticles >= 0 && maxParticles <= ParticleContainer::MAX_ALLOWED_PARTICLES && "Particles amount is out of range");
		}

		virtual void Create(GameObject target) override {
			ParticleContainer container;
			container.SetMaxParticles(maxParticles);
			HFEngine::ECS.AddComponent<ParticleContainer>(target, container);
		}
	};
	class ParticleEmitterLoader : public IPrefabComponentLoader {
	public:
		ParticleEmitter emitter;

		virtual void Preprocess(PropertyReader& properties) override {
			static std::string emitterShapeStr;
			if (properties.GetString("shape", emitterShapeStr, "RECTANGLE")) {
				if (emitterShapeStr == "RECTANGLE")
					emitter.shape = ParticleEmitter::EmitterShape::RECTANGLE;
				else if (emitterShapeStr == "CIRCLE")
					emitter.shape = ParticleEmitter::EmitterShape::CIRCLE;
				else {
					LogWarning("ParticleEmitterLoader::Preprocess(): Invalid 'shape' value. Using default: {}", "RECTANGLE");
					emitter.shape = ParticleEmitter::EmitterShape::RECTANGLE;
				}
			}
			else {
				LogWarning("ParticleEmitterLoader::Preprocess(): Missing 'shape' value. Using default: {}", "RECTANGLE");
				emitter.shape = ParticleEmitter::EmitterShape::RECTANGLE;
			}

			if (!properties.GetVec2("sourcShapeeSize", emitter.sourcShapeeSize, emitter.sourcShapeeSize)) {
				LogWarning("ParticleEmitterLoader::Preprocess(): Missing 'sourcShapeeSize' value. Using default: {}", emitter.sourcShapeeSize);
			}
			if (!properties.GetVec2("targetShapeSize", emitter.targetShapeSize, emitter.targetShapeSize)) {
				LogWarning("ParticleEmitterLoader::Preprocess(): Missing 'targetShapeSize' value. Using default: {}", emitter.targetShapeSize);
			}
			if (!properties.GetVec2("lifetime", emitter.lifetime, emitter.lifetime)) {
				LogWarning("ParticleEmitterLoader::Preprocess(): Missing 'lifetime' value. Using default: {}", emitter.lifetime);
			}
			if (!properties.GetVec2("velocity", emitter.velocity, emitter.velocity)) {
				LogWarning("ParticleEmitterLoader::Preprocess(): Missing 'velocity' value. Using default: {}", emitter.velocity);
			}
			if (!properties.GetVec2("size", emitter.size, emitter.size)) {
				LogWarning("ParticleEmitterLoader::Preprocess(): Missing 'size' value. Using default: {}", emitter.size);
			}
			if (!properties.GetFloat("rate", emitter.rate, emitter.rate)) {
				LogWarning("ParticleEmitterLoader::Preprocess(): Missing 'rate' value. Using default: {}", emitter.rate);
			}

			properties.GetBool("emitting", emitter.emitting, emitter.emitting);
		}

		virtual void Create(GameObject target) override {
			ParticleEmitter emitter = this->emitter;
			HFEngine::ECS.AddComponent<ParticleEmitter>(target, emitter);
		}
	};
	class ParticleRendererLoader : public IPrefabComponentLoader {
	public:
		ParticleRenderer renderer;

		virtual void Preprocess(PropertyReader& properties) override {
			// color over time
			std::string colorOverTimeStr = "1.0,1.0,1.0";
			if (!properties.GetString("colorOverTime", colorOverTimeStr, colorOverTimeStr)) {
				LogWarning("ParticleRendererLoader::Preprocess(): Missing 'colorOverTime' value. Using default: {}", colorOverTimeStr);
			}
			else {
				std::vector<glm::vec3> colorOverTimeVec3;
				for (auto& color : Utility::StringSplit(colorOverTimeStr, ';'))
				{
					glm::vec3 cvec;
					if (!Utility::TryConvertStringToVec3(color, cvec)) {
						LogWarning("ParticleRendererLoader::Preprocess(): Cannot parse '{}' in 'colorOverTime' value. Using: {}", color, cvec);
					}
					colorOverTimeVec3.push_back(cvec);
				}
				renderer.colorOverTime = TextureTools::GenerateGradientTexture(colorOverTimeVec3);
			}

			// opacity over time
			std::string opacityOverTimeStr = "1.0";
			if (!properties.GetString("opacityOverTime", opacityOverTimeStr, opacityOverTimeStr)) {
				LogWarning("ParticleRendererLoader::Preprocess(): Missing 'opacityOverTime' value. Using default: {}", opacityOverTimeStr);
			}
			else {
				std::vector<float> opacityOverTimeFloats;
				for (auto& opacity : Utility::StringSplit(opacityOverTimeStr, ';'))
				{
					float of;
					if (!Utility::TryConvertStringToFloat(opacity, of)) {
						LogWarning("ParticleRendererLoader::Preprocess(): Cannot parse '{}' in 'opacityOverTime' value. Using: {}", opacity, of);
					}
					opacityOverTimeFloats.push_back(of);
				}
				renderer.opacityOverTime = TextureTools::GenerateGradientTexture(opacityOverTimeFloats);
			}


			static std::string tmpSpriteSheetPath;
			if (properties.GetString("spriteSheet", tmpSpriteSheetPath, "")) {
				auto parts = Utility::StringSplit(tmpSpriteSheetPath, ':');
				if (parts.size() == 2) {
					renderer.spriteSheet = TextureManager::GetTexture(parts[0], parts[1]);
				}
				else {
					LogWarning("ParticleRendererLoader::Preprocess(): Cannot parse 'spriteSheet' value: {}", tmpSpriteSheetPath);
				}
			}

			if (!properties.GetInt("spriteSheetCount", renderer.spriteSheetCount, renderer.spriteSheetCount)) {
				LogWarning("ParticleRendererLoader::Preprocess(): Missing 'spriteSheetCount' value. Using default: {}", renderer.spriteSheetCount);
			}
		}

		virtual void Create(GameObject target) override {
			ParticleRenderer renderer = this->renderer;
			HFEngine::ECS.AddComponent<ParticleRenderer>(target, renderer);
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

					static float floatTmp;
					if (Utility::TryConvertStringToFloat(property.second, floatTmp))
						set = script->SetFloat(property.first, floatTmp);

					static glm::vec3 vec3tmp;
					if (!set && Utility::TryConvertStringToVec3(property.second, vec3tmp))
						set = script->SetVec3(property.first, vec3tmp);
				
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
	};


	class BoneAttacherLoader : public IPrefabComponentLoader
	{
	public:
		std::string boneName;

		void Preprocess(PropertyReader& properties) override
		{
			if (!properties.GetString("boneName", boneName, ""))
			{
				LogWarning("BoneAttacherLoader::Preprocess(): Missing 'boneName' parameter.");
			}
		}

		void Create(GameObject target) override
		{
			BoneAttacher attacher;
			attacher.boneName = boneName;
			HFEngine::ECS.AddComponent<BoneAttacher>(target, attacher);
		}
	};

} // end of namespace


void PrefabComponentLoader::RegisterLoaders()
{
	PrefabManager::RegisterComponentLoader("ModelHolder", []() { return std::make_shared<ModelHolderLoader>(); });
	PrefabManager::RegisterComponentLoader("MeshRenderer", []() { return std::make_shared<MeshRendererLoader>(); });
	PrefabManager::RegisterComponentLoader("SkinnedMeshRenderer", []() { return std::make_shared<SkinnedMeshRendererLoader>(); });
	PrefabManager::RegisterComponentLoader("PointLightRenderer", []() { return std::make_shared<PointLightRendererLoader>(); });

	PrefabManager::RegisterComponentLoader("SkinAnimator", []() { return std::make_shared<SkinAnimatorLoader>(); });

	PrefabManager::RegisterComponentLoader("ParticleContainer", []() { return std::make_shared<ParticleContainerLoader>(); });
	PrefabManager::RegisterComponentLoader("ParticleEmitter", []() { return std::make_shared<ParticleEmitterLoader>(); });
	PrefabManager::RegisterComponentLoader("ParticleRenderer", []() { return std::make_shared<ParticleRendererLoader>(); });


	PrefabManager::RegisterComponentLoader("RigidBody", []() { return std::make_shared<RigidBodyLoader>(); });
	PrefabManager::RegisterComponentLoader("CircleCollider", []() { return std::make_shared<CircleColliderLoader>(); });
	PrefabManager::RegisterComponentLoader("BoxCollider", []() { return std::make_shared<BoxColliderLoader>(); });
	PrefabManager::RegisterComponentLoader("GravityCollider", []() { return std::make_shared<GravityColliderLoader>(); });
	PrefabManager::RegisterComponentLoader("ScriptComponent", []() { return std::make_shared<ScriptComponentLoader>(); });

	PrefabManager::RegisterComponentLoader("BoneAttacher", []() { return std::make_shared<BoneAttacherLoader>(); });
}
