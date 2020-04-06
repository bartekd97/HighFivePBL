#pragma once

#include "ECS/ECSCore.h"
#include "Rendering/RenderPipeline.h"
#include "Rendering/Camera.h"
#include "Rendering/Lights.h"

namespace HFEngine
{
	bool Initialize(const int& screenWidth, const int& screenHeight, const char* windowTitle);
	void Terminate();

	extern ECSCore ECS;
	extern RenderPipeline Renderer;
	extern Camera MainCamera;
	extern DirectionalLight WorldLight;
	
	extern int RENDER_WIDTH;
	extern int RENDER_HEIGHT;
}
