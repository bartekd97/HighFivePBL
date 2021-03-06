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
#include "Scene/SceneManager.h"
#include "Physics/Physics.h"
#include "Audio/AudioManager.h"

namespace HFEngine
{
	GLFWcursor* cursor = nullptr;
	HFEngineConfigStruct configStruct;
	bool initialized = false;
	ECSCore ECS;
	RenderPipeline Renderer;
	Camera MainCamera;
	DirectionalLight WorldLight;
	int RENDER_WIDTH;
	int RENDER_HEIGHT;
	FrameCounter CURRENT_FRAME_NUMBER = 1;
	int SHADOWMAP_SIZE = 1024;

	void InitializeCursor()
	{
		static bool cursorInitialized = false;
		if (!cursorInitialized)
		{
			GLFWimage image;
			auto data = TextureManager::LoadRawDataFromFile("GUI", "Cursor", image.width, image.height);
			if (data.size() == 0)
			{
				LogError("HFEngine::InitializeCursor: failed to load cursor image");
				return;
			}
			image.pixels = data.data();
			cursor = glfwCreateCursor(&image, 0, 0);
			if (cursor == nullptr)
			{
				LogError("HFEngine::InitializeCursor: failed to initialize cursor");
				return;
			}
			glfwSetCursor(WindowManager::GetWindow(), cursor);
			cursorInitialized = true;
		}
	}

	//bool Initialize(const int& screenWidth, const int& screenHeight, const char* windowTitle)
	bool Initialize(const HFEngineConfigStruct& config)
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

		WindowManager::Initialize(config.ScreenWidth, config.ScreenHeight, config.WindowTitle, config.FullscreenMode);

		RENDER_WIDTH = config.ScreenWidth;
		RENDER_HEIGHT = config.ScreenHeight;

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
		SceneManager::Initialize();

		AudioManager::Init_al();

		MainCamera.SetMode(Camera::ORTHOGRAPHIC);
		MainCamera.SetSize((float)RENDER_WIDTH / (float)RENDER_HEIGHT, 1.0f);
		//MainCamera.SetSize((float)RENDER_WIDTH / 100.0f, (float)RENDER_HEIGHT / 100.0f);
		//MainCamera.SetScale(0.015625f); // 1/64
		//MainCamera.SetScale(0.03125f); // 1/32
		//MainCamera.SetScale(0.0625f); // 1/16
		//MainCamera.SetScale(1.75f);

		InitializeCursor();

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
		ECS.RegisterComponent<GrassPatchRenderer>();
		// particle components
		ECS.RegisterComponent<ParticleContainer>();
		ECS.RegisterComponent<ParticleEmitter>();
		ECS.RegisterComponent<ParticleRenderer>();
		// script components
		ECS.RegisterComponent<LifeTime>();
		ECS.RegisterComponent<ScriptContainer>();
		ECS.RegisterComponent<CellPathfinder>();
		// map layout components
		ECS.RegisterComponent<MapCell>();
		ECS.RegisterComponent<CellGate>();
		ECS.RegisterComponent<CellBridge>();
		ECS.RegisterComponent<CellChild>();
		// grass components
		ECS.RegisterComponent<GrassCircleSimulationPrimitive>();
		ECS.RegisterComponent<GrassSimulator>();

		auto nextFrameDestroySystem = ECS.RegisterSystem<NextFrameDestroySystem>(true);

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
			signature.set(ECS.GetComponentType<Collider>());
			ECS.SetSystemSignature<PhysicsSystem>(signature);
		}
		physicsSystem->SetCollector(colliderCollectorSystem);
		auto triggerColliderCollectorSystem = ECS.RegisterSystem<TriggerColliderCollectorSystem>();
		{
			Signature signature;
			signature.set(ECS.GetComponentType<Collider>());
			ECS.SetSystemSignature<TriggerColliderCollectorSystem>(signature);
		}
		auto physicsSpawnTriggerSystem = ECS.RegisterSystem<PhysicsSpawnTriggerSystem>();
		{
			Signature signature;
			signature.set(ECS.GetComponentType<Transform>());
			signature.set(ECS.GetComponentType<RigidBody>());
			signature.set(ECS.GetComponentType<Collider>());
			ECS.SetSystemSignature<PhysicsSpawnTriggerSystem>(signature);
		}
		physicsSpawnTriggerSystem->SetCollector(triggerColliderCollectorSystem);
		auto gravitySystem = ECS.RegisterSystem<GravitySystem>();
		{
			Signature signature;
			signature.set(ECS.GetComponentType<Transform>());
			signature.set(ECS.GetComponentType<RigidBody>());
			ECS.SetSystemSignature<GravitySystem>(signature);
		}
		gravitySystem->SetCollector(mapCellCollectorSystem);
		auto rigidBodyCollectorSystem = ECS.RegisterSystem<RigidBodyCollectorSystem>(); // TODO: mo�e u�y� PhysicsSystem po prostu?
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
		auto cellPathfinderSystem = ECS.RegisterSystem<CellPathfinderSystem>();
		{
			Signature signature;
			signature.set(ECS.GetComponentType<CellChild>());
			signature.set(ECS.GetComponentType<CellPathfinder>());
			ECS.SetSystemSignature<CellPathfinderSystem>(signature);
		}

		auto grassPrimitiveCollectorSystem = ECS.RegisterSystem<GrassPrimitiveCollectorSystem>();
		{
			Signature signature;
			signature.set(ECS.GetComponentType<GrassCircleSimulationPrimitive>());
			ECS.SetSystemSignature<GrassPrimitiveCollectorSystem>(signature);
		}
		auto grassSimulatorSystem = ECS.RegisterSystem<GrassSimulatorSystem>();
		{
			Signature signature;
			signature.set(ECS.GetComponentType<GrassSimulator>());
			ECS.SetSystemSignature<GrassSimulatorSystem>(signature);
		}


		auto scriptLateUpdateSystem = ECS.RegisterSystem<ScriptLateUpdateSystem>();
		{
			Signature signature;
			signature.set(ECS.GetComponentType<ScriptContainer>());
			ECS.SetSystemSignature<ScriptLateUpdateSystem>(signature);
		}
		auto garbageCollectorUpdateSystem = ECS.RegisterSystem<GarbageCollectorSystem>();
		{
			Signature signature;
			signature.set(ECS.GetComponentType<Transform>());
			signature.set(ECS.GetComponentType<RigidBody>());
			ECS.SetSystemSignature<GarbageCollectorSystem>(signature);
		}

		Renderer.Init();


		initialized = true;

		return true;
	}

	void ClearGameObjects()
	{
		ECS.ClearGameObjects();
		Physics::ClearGameObjects();
		EventManager::FireEvent(Events::General::GAMEOBJECTS_CLEAR);
	}

	void Terminate()
	{
		if (!initialized)
		{
			LogError("HFEngine not initialized");
			return;
		}
		if (cursor != nullptr)
		{
			glfwDestroyCursor(cursor);
			cursor = nullptr;
		}

		if (WindowManager::IsClosing())
		{
			glfwTerminate();
			GUIManager::Terminate();
			AudioManager::Exit_al();
		}
		else
		{
			WindowManager::Close();
		}
	}

	const HFEngineConfigStruct& GetConfig()
	{
		return configStruct;
	}

/*
	=================== FRAME START ====================

	      | <-- Send General::FRAME_START event
		  |
		  \/
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

		EventManager::FireEvent(Events::General::FRAME_START);

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
		HFEngine::ECS.PostRenderSystems();
		GUIManager::Draw();
	}
}
