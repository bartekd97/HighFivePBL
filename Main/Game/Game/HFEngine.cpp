#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "HFEngine.h"
#include "Utility/Logger.h"

#include "ECS/Components.h"
#include "ECS/Systems.h"

#include "Resourcing/Texture.h"
#include "Resourcing/Material.h"
#include "Resourcing/Model.h"
#include "Resourcing/Shader.h"
#include "Rendering/PrimitiveRenderer.h"
#include "WindowManager.h"

namespace HFEngine
{
	bool initialized = false;
	ECSCore ECS;
	RenderPipeline Renderer;
	int RENDER_WIDTH;
	int RENDER_HEIGHT;

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

		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		{
			LogError("Failed to initialize GLAD");
			return false;
		}

		ShaderManager::Initialize();
		TextureManager::Initialize();
		MaterialManager::Initialize();
		ModelManager::Initialize();

		ECS.Init();

		// general components
		ECS.RegisterComponent<Transform>();
		ECS.RegisterComponent<RigidBody>();
		ECS.RegisterComponent<Gravity>();
		// render components
		ECS.RegisterComponent<CubeRenderer>();
		ECS.RegisterComponent<MeshRenderer>();
		// script components
		ECS.RegisterComponent<LifeTime>();
		ECS.RegisterComponent<CubeSpawner>();

		auto cubeRenderSystem = ECS.RegisterSystem<CubeRenderSystem>();
		{
			Signature signature;
			signature.set(ECS.GetComponentType<Transform>());
			signature.set(ECS.GetComponentType<CubeRenderer>());
			ECS.SetSystemSignature<CubeRenderSystem>(signature);
		}
		auto cubeSpawnerSystem = ECS.RegisterSystem<CubeSpawnerSystem>();
		{
			Signature signature;
			signature.set(ECS.GetComponentType<CubeSpawner>());
			ECS.SetSystemSignature<CubeSpawnerSystem>(signature);
		}
		auto physicsSystem = ECS.RegisterSystem<PhysicsSystem>();
		{
			Signature signature;
			signature.set(ECS.GetComponentType<Transform>());
			signature.set(ECS.GetComponentType<RigidBody>());
			signature.set(ECS.GetComponentType<Gravity>());
			ECS.SetSystemSignature<PhysicsSystem>(signature);
		}
		auto lifeTimeSystem = ECS.RegisterSystem<LifeTimeSystem>();
		{
			Signature signature;
			signature.set(ECS.GetComponentType<LifeTime>());
			ECS.SetSystemSignature<LifeTimeSystem>(signature);
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
	}
}
