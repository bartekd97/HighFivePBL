#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "HFEngine.h"
#include "Logger.h"

#include "LifeTime.h"
#include "Transform.h"
#include "CubeRenderer.h"
#include "CubeSpawner.h"
#include "RigidBody.h"
#include "Gravity.h"

#include "LifeTimeSystem.h"
#include "CubeRenderSystem.h"
#include "CubeSpawnerSystem.h"
#include "PhysicsSystem.h"

#include "Texture.h"
#include "Material.h"
#include "Model.h"
#include "Shader.h"
#include "PrimitiveRenderer.h"
#include "WindowManager.h"

namespace HFEngine
{
	bool initialized = false;
	ECSCore ECS;

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

		ECS.RegisterComponent<LifeTime>();
		ECS.RegisterComponent<Transform>();
		ECS.RegisterComponent<CubeRenderer>();
		ECS.RegisterComponent<CubeSpawner>();
		ECS.RegisterComponent<RigidBody>();
		ECS.RegisterComponent<Gravity>();

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
