#pragma once

#include "ECSCore.h"
#include "RenderPipeline.h"

namespace HFEngine
{
	bool Initialize(const int& screenWidth, const int& screenHeight, const char* windowTitle);
	void Terminate();

	extern ECSCore ECS;
	extern RenderPipeline Renderer;
	extern int RENDER_WIDTH;
	extern int RENDER_HEIGHT;
}
