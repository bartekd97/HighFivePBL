#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "HFEngine.h"
#include "Logger.h"
#include "LifeTime.h"
#include "LifeTimeSystem.h"

#include "Texture.h"
#include "Material.h"
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

		ECS.Init();
		ECS.RegisterComponent<LifeTime>();
		auto lifeTimeSystem = ECS.RegisterSystem<LifeTimeSystem>();
		{
			Signature signature;
			signature.set(ECS.GetComponentType<LifeTime>());
			ECS.SetSystemSignature<LifeTimeSystem>(signature);
		}

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