#include <vector>

#include "GUIManager.h"
#include "../InputManager.h"
#include "../Resourcing/ShaderManager.h"
#include "Text/TextRenderer.h"
#include "../Utility/Logger.h"
#include "Event/EventManager.h"
#include "Event/Events.h"
#include "../WindowManager.h"

namespace GUIManager
{
	std::vector<std::shared_ptr<Widget>> root;
	std::map<int, std::vector<std::shared_ptr<Widget>>> indexedWidgets;
	std::shared_ptr<Shader> guiShader;
	std::shared_ptr<Texture> defaultTexture;
	bool initialized = false;

	void OnWindowResize(Event& ev)
	{
		std::vector<std::shared_ptr<Widget>> all = root;
		int i = 0, max = all.size();

		while (i < max)
		{
			for (auto child : all[i]->children) all.push_back(child);
			max += all[i]->children.size();
			i++;
		}

		for (auto& widget : all)
		{
			widget->Recalculate();
		}
	}

	void Initialize()
	{
		if (initialized)
		{
			LogWarning("GUIManager already initialized!");
			return;
		}
		TextRenderer::Initialize();
		TextRenderer::LoadFont("Arial", "Data/Assets/Fonts/arial.ttf");
		TextRenderer::SetFont("Arial");

		guiShader = ShaderManager::GetShader("GUIShader");
		defaultTexture = TextureManager::GetLibrary("GUI")->GetTexture("blank");
		EventManager::AddListener(FUNCTION_LISTENER(Events::General::WINDOW_RESIZE, OnWindowResize));
	}

	void Terminate()
	{
		TextRenderer::Terminate();
	}

	void Update()
	{
		glm::vec2 mousePosition = InputManager::GetMousePosition();
		for (auto widget : root)
		{
			if (widget->GetEnabled())
			{
				widget->Update(mousePosition);
			}
		}
	}

	void Draw()
	{
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		TextRenderer::UpdateProjection();

		glm::vec2 screen(WindowManager::SCREEN_WIDTH, WindowManager::SCREEN_HEIGHT);
		glm::vec2 minMax[2];
		for (auto it = indexedWidgets.begin(); it != indexedWidgets.end(); it++)
		{
			for (auto widget : it->second)
			{
				if (widget->GetEnabled())
				{
					minMax[0] = glm::vec2(widget->GetAbsolutePosition());
					minMax[1] = minMax[0] + widget->GetLocalSize();
					if (minMax[1].x < 0.0f || minMax[1].y < 0.0f || minMax[0].x > screen.x || minMax[0].y > screen.y) continue;
					widget->PreDraw();
					widget->Draw();
					widget->PostDraw();
				}
			}
		}

		glDisable(GL_BLEND);
	}

	void AddWidget(std::shared_ptr<Widget> widget, std::shared_ptr<Widget> parent, int zIndex)
	{
		if (parent == nullptr)
		{
			root.push_back(widget);
		}
		else
		{
			if (zIndex < parent->GetZIndex())
			{
				zIndex = parent->GetZIndex();
			}
			widget->parent = parent;
			parent->AddChild(widget);
		}
		widget->SetZIndex(zIndex);
		indexedWidgets[zIndex].push_back(widget);
	}

	void RemoveWidget(std::shared_ptr<Widget> widget)
	{
		// TODO: check crash
		std::vector<std::shared_ptr<Widget>> all = { widget };
		int i = 0, max = all.size();

		root.erase(std::remove(root.begin(), root.end(), widget));

		while (i < max)
		{
			for (auto child : all[i]->children) all.push_back(child);
			max += all[i]->children.size();
			i++;
		}

		for (auto itMap = indexedWidgets.begin(); itMap != indexedWidgets.end(); itMap++)
		{
			for (auto itVector = itMap->second.begin(); itVector != itMap->second.end();)
			{
				for (auto& toRemove : all)
				{
					if (*itVector == toRemove)
					{
						itVector = itMap->second.erase(itVector);
					}
					else
					{
						++itVector;
					}
				}
			}
		}

		
	}
}
