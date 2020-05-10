#pragma once

#include <memory>

#include "Widget.h"
#include "../Resourcing/Texture.h"

#define GUI_METHOD_POINTER(method) std::bind(&method, this)

namespace GUIManager
{
	extern std::shared_ptr<Texture> defaultTexture;

	void Initialize();

	void Update();
	void Draw();

	void AddWidget(std::shared_ptr<Widget> widget, std::shared_ptr<Widget> parent = nullptr);
};
