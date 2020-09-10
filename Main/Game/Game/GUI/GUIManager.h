#pragma once

#include <memory>

#include "Widget.h"
#include "../Resourcing/Texture.h"
#include "../Resourcing/Shader.h"

#define GUI_METHOD_POINTER(method) std::bind(&method, this)

namespace GUIManager
{
	extern std::shared_ptr<Texture> defaultTexture;
	extern std::shared_ptr<Shader> guiShader;

	void Initialize();
	void Terminate();

	void Update();
	void Draw();

	void AddWidget(std::shared_ptr<Widget> widget, std::shared_ptr<Widget> parent = nullptr, int zIndex = 0);
	void RemoveWidget(std::shared_ptr<Widget> widget);
	void SetLayerEnabled(int zIndex, bool status);

	namespace KeybindLock
	{
		void Set(std::string name);
		void Release(std::string name);
		bool Is(std::string name);
		bool Any(); // true if any lock is active, false when no lock is currently active
	};
};
