#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "HFEngine.h"
#include "Utility/Logger.h"

#include "ECS/Components.h"
#include "ECS/Systems.h"

#include "Event/EventManager.h"
#include "Event/Events.h"

#include "Scripting/ScriptManager.h"

#include "Resourcing/Texture.h"
#include "Resourcing/Material.h"
#include "Resourcing/Model.h"
#include "Resourcing/Shader.h"
#include "Resourcing/Prefab.h"
#include "Rendering/PrimitiveRenderer.h"
#include "WindowManager.h"
#include "InputManager.h"
#include "GUI/GUIManager.h"
#include "Physics/Physics.h"

namespace HFEngine
{
	bool initialized = false;
	ECSCore ECS;
	RenderPipeline Renderer;
	Camera MainCamera;
	DirectionalLight WorldLight;
	int RENDER_WIDTH;
	int RENDER_HEIGHT;
	FrameCounter CURRENT_FRAME_NUMBER = 1;
	int SHADOWMAP_SIZE = 1024;

	bool Initialize(const int& screenWidth, const int& screenHeight, const char* windowTitle)
	{
		if (initialized)
		{
			LogError("HFEngine already initialized");
			return false;
		}

		LoggerInitialize();

		glfwInit();
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

		WindowManager::Initialize(screenWidth, screenHeight, windowTitle);

		RENDER_WIDTH = screenWidth;
		RENDER_HEIGHT = screenHeight;

		if (WindowManager::GetWindow() == nullptr)
		{
			LogError("Failed to create GLFW window");
			glfwTerminate();
			return false;
		}

		InputManager::Initialize();

		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		{
			LogError("Failed to initialize GLAD");
			return false;
		}

		ShaderManager::Initialize();
		TextureManager::Initialize();
		MaterialManager::Initialize();
		ModelManager::Initialize();
		PrefabManager::Initialize();
		ScriptManager::Initialize();
		EventManager::Initialize();
		GUIManager::Initialize();

		MainCamera.SetMode(Camera::ORTHOGRAPHIC);
		MainCamera.SetSize((float)RENDER_WIDTH / 100.0f, (float)RENDER_HEIGHT / 100.0f);
		//MainCamera.SetScale(0.015625f); // 1/64
		//MainCamera.SetScale(0.03125f); // 1/32
		//MainCamera.SetScale(0.0625f); // 1/16
		MainCamera.SetScale(1.75f);

		ECS.Init();

		// general components
		ECS.RegisterComponent<Transform>();
		ECS.RegisterComponent<ModelHolder>();
		ECS.RegisterComponent<RigidBody>();
		ECS.RegisterComponent<GravityCollider>();
		ECS.RegisterComponent<Collider>();
		ECS.RegisterComponent<CircleCollider>();
		ECS.RegisterComponent<BoxCollider>();
		ECS.RegisterComponent<SkinAnimator>();
		ECS.RegisterComponent<BoneAttacher>();
		// render components
		ECS.RegisterComponent<MeshRenderer>();
		ECS.RegisterComponent<SkinnedMeshRenderer>();
		ECS.RegisterComponent<PointLightRenderer>();
		// particle components
		ECS.RegisterComponent<ParticleContainer>();
		ECS.RegisterComponent<ParticleEmitter>();
		ECS.RegisterComponent<ParticleRenderer>();
		// script components
		ECS.RegisterComponent<LifeTime>();
		ECS.RegisterComponent<ScriptContainer>();
		// map layout components
		ECS.RegisterComponent<MapCell>();
		ECS.RegisterComponent<CellGate>();
		ECS.RegisterComponent<CellBridge>();

		auto mapCellCollectorSystem = ECS.RegisterSystem<MapCellCollectorSystem>();
		{
			Signature signature;
			signature.set(ECS.GetComponentType<MapCell>());
			ECS.SetSystemSignature<MapCellCollectorSystem>(signature);
		}

		auto scriptStartSystem = ECS.RegisterSystem<ScriptStartSystem>(true);
		auto scriptUpdateSystem = ECS.RegisterSystem<ScriptUpdateSystem>();
		{
			Signature signature;
			signature.set(ECS.GetComponentType<ScriptContainer>());
			ECS.SetSystemSignature<ScriptUpdateSystem>(signature);
		}


		auto skinAnimatorSystem = ECS.RegisterSystem<SkinAnimatorSystem>();
		{
			Signature signature;
			signature.set(ECS.GetComponentType<SkinAnimator>());
			signature.set(ECS.GetComponentType<SkinnedMeshRenderer>());
			ECS.SetSystemSignature<SkinAnimatorSystem>(signature);
		}
		auto boneAttacherSystem = ECS.RegisterSystem<BoneAttacherSystem>();
		{
			Signature signature;
			signature.set(ECS.GetComponentType<BoneAttacher>());
			ECS.SetSystemSignature<BoneAttacherSystem>(signature);
		}
		auto particleEmitterSystem = ECS.RegisterSystem<ParticleEmitterSystem>();
		{
			Signature signature;
			signature.set(ECS.GetComponentType<ParticleContainer>());
			signature.set(ECS.GetComponentType<ParticleEmitter>());
			ECS.SetSystemSignature<ParticleEmitterSystem>(signature);
		}
		auto colliderCollectorSystem = ECS.RegisterSystem<ColliderCollectorSystem>();
		{
			Signature signature;
			signature.set(ECS.GetComponentType<Collider>());
			ECS.SetSystemSignature<ColliderCollectorSystem>(signature);
		}
		auto physicsSystem = ECS.RegisterSystem<PhysicsSystem>();
		{
			Signature signature;
			signature.set(ECS.GetComponentType<Transform>());
			signature.set(ECS.GetComponentType<RigidBody>());
			ECS.SetSystemSignature<PhysicsSystem>(signature);
		}
		physicsSystem->SetCollector(colliderCollectorSystem);
		auto gravitySystem = ECS.RegisterSystem<GravitySystem>();
		{
			Signature signature;
			signature.set(ECS.GetComponentType<Transform>());
			signature.set(ECS.GetComponentType<RigidBody>());
			ECS.SetSystemSignature<GravitySystem>(signature);
		}
		gravitySystem->SetCollector(mapCellCollectorSystem);
		auto rigidBodyCollectorSystem = ECS.RegisterSystem<RigidBodyCollectorSystem>(); // TODO: mo¿e u¿yæ PhysicsSystem po prostu?
		{
			Signature signature;
			signature.set(ECS.GetComponentType<Transform>());
			signature.set(ECS.GetComponentType<RigidBody>());
			ECS.SetSystemSignature<RigidBodyCollectorSystem>(signature);
		}
		Physics::SetRigidBodyCollector(rigidBodyCollectorSystem);
		auto lifeTimeSystem = ECS.RegisterSystem<LifeTimeSystem>();
		{
			Signature signature;
			signature.set(ECS.GetComponentType<LifeTime>());
			ECS.SetSystemSignature<LifeTimeSystem>(signature);
		}


		auto scriptLateUpdateSystem = ECS.RegisterSystem<ScriptLateUpdateSystem>();
		{
			Signature signature;
			signature.set(ECS.GetComponentType<ScriptContainer>());
			ECS.SetSystemSignature<ScriptLateUpdateSystem>(signature);
		}

		Renderer.Init();

		initialized = true;

		return true;
	}

	void Terminate()
	{
		if (!initialized)
		{
			LogError("HFEngine not initialized");
			return;
		}
		glfwTerminate();
		GUIManager::Terminate();
	}

/*
	=================== FRAME START ====================

	--------------
	| Update GUI |
	--------------
	      |
		  | <-- Send General::UPDATE event
		  |
	      \/
	----------------------------
	|    ------------------    |
	|    | Script::Update |    |
	|    ------------------    |
	|            |             |
	|  	         \/            |
	|    ------------------    |
	|    | System::Update |    |
	|    ------------------    |
	|            |             |
	|  	         \/            |
	|  ----------------------  |
	|  | Script::LateUpdate |  |
	|  ----------------------  |
	----------------------------
		  |
		  | <-- Send General::POST_UPDATE event
		  |
		  \/
	----------------------
	| System::PostUpdate |
	----------------------
	      |
		  |
		  \/
	----------------------
	|  ----------------  |
	|  | Render World |  |
	|  ----------------  |
	|         |          |
    |		  \/         |
    |   --------------   |
	|   | Render GUI |   |
	|   --------------   |
	----------------------

	=================== FRAME END ====================
*/
	void ProcessGameFrame(float dt)
	{
		CURRENT_FRAME_NUMBER++;

		GUIManager::Update();

		Event updateEvent(Events::General::UPDATE);
		updateEvent.SetParam(Events::General::DELTA_TIME, dt);
		EventManager::FireEvent(updateEvent);

		HFEngine::ECS.UpdateSystems(dt);

		Event lateUpdateEvent(Events::General::POST_UPDATE);
		lateUpdateEvent.SetParam(Events::General::DELTA_TIME, dt);
		EventManager::FireEvent(lateUpdateEvent);

		HFEngine::ECS.PostUpdateSystems(dt);

		HFEngine::Renderer.Render();
		GUIManager::Draw();
	}
}
