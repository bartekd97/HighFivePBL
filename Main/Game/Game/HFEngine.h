#pragma once

#include "ECS/ECSCore.h"
#include "Rendering/RenderPipeline.h"
#include "Rendering/Camera.h"
#include "Rendering/Lights.h"

namespace HFEngine
{
	struct HFEngineConfigStruct {
		std::string WindowTitle;
		int ScreenWidth;
		int ScreenHeight;
		bool FullscreenMode;
	};

	//bool Initialize(const int& screenWidth, const int& screenHeight, const char* windowTitle);
	bool Initialize(const HFEngineConfigStruct& config);
	void Terminate();

	const HFEngineConfigStruct& GetConfig();

	void ProcessGameFrame(float dt);

	void ClearGameObjects();

	extern ECSCore ECS;
	extern RenderPipeline Renderer;
	extern Camera MainCamera;
	extern DirectionalLight WorldLight;
	
	extern int RENDER_WIDTH;
	extern int RENDER_HEIGHT;

	extern FrameCounter CURRENT_FRAME_NUMBER;

	extern int SHADOWMAP_SIZE;
}
