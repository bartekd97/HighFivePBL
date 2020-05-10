#pragma once

#include "../Script.h"
#include "../../HFEngine.h"

#include "../../GUI/GUIManager.h"
#include "../../GUI/Widgets.h"

class TestGUIScript : public Script
{
public:
	void Awake()
	{
		std::shared_ptr<Panel> panel = std::make_shared<Panel>();
		panel->SetPosition(glm::vec3(150.0f, 50.0f, 0.0f));
		panel->size.x = 250.0f;
		panel->size.y = 200.0f;
		panel->textureColor.color = glm::vec4(1.0f, 0.0f, 0.0f, 0.3f);

		GUIManager::AddWidget(panel);


		std::shared_ptr<Button> button = std::make_shared<Button>();
		button->SetPositionAnchor(glm::vec3(20.0f, 10.0f, 0.0f), Anchor::TOPRIGHT);
		button->size.x = 150.0f;
		button->size.y = 100.0f;

		button->textureColors[Button::STATE::NORMAL].color.a = 0.5f;
		button->textureColors[Button::STATE::HOVER].color.a = 0.7f;
		button->textureColors[Button::STATE::PRESSED].color.a = 0.9f;

		button->OnClickListener = GUI_METHOD_POINTER(TestGUIScript::OnButtonClick);

		GUIManager::AddWidget(button, panel);
	}

	void OnButtonClick()
	{
		LogInfo("TestGUIScript::OnButtonClick");
	}
};
