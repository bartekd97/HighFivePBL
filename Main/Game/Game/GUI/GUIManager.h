#pragma once

#include <memory>

#include "Widget.h"
#include "../Resourcing/Texture.h"

namespace GUIManager
{
	extern std::shared_ptr<Texture> defaultTexture;

	void Initialize();

	void Update();
	void Draw();

	void AddWidget(std::shared_ptr<Widget> widget, std::shared_ptr<Widget> parent = nullptr);
};
