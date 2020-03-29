#pragma once

#include "ECSCore.h"

namespace HFEngine
{
	bool Initialize(const int& screenWidth, const int& screenHeight, const char* windowTitle);
	void Terminate();

	extern ECSCore ECS;
}
