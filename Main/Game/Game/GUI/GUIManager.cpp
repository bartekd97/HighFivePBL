#include <vector>

#include "GUIManager.h"
#include "../InputManager.h"
#include "../Resourcing/ShaderManager.h"

namespace GUIManager
{
	std::vector<std::shared_ptr<Widget>> root;
	std::vector<std::shared_ptr<Widget>> allWidgets;
	std::shared_ptr<Shader> guiShader;
	std::shared_ptr<Texture> defaultTexture;

	void Initialize()
	{
		guiShader = ShaderManager::GetShader("GUIShader");
		defaultTexture = TextureManager::GetLibrary("Sample")->GetTexture("GUI_blank");
	}

	void Update()
	{
		glm::vec2 mousePosition = InputManager::GetMousePosition();
		for (auto widget : root)
		{
			widget->Update(mousePosition);
		}
	}

	void Draw()
	{
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		for (auto widget : allWidgets)
		{
			widget->Draw();
		}

		glDisable(GL_BLEND);
	}

	void AddWidget(std::shared_ptr<Widget> widget, std::shared_ptr<Widget> parent)
	{
		if (parent == nullptr)
		{
			root.push_back(widget);
		}
		else
		{
			widget->parent = parent;
			parent->AddChild(widget);
		}
		allWidgets.push_back(widget);
	}
}
