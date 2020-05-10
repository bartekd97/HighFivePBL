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
		panel->SetSize(glm::vec2(250.0f, 200.0f));
		panel->textureColor.color = glm::vec4(1.0f, 0.0f, 0.0f, 0.3f);

		GUIManager::AddWidget(panel);

		std::shared_ptr<Label> label = std::make_shared<Label>();
		label->SetText("Test label");
		label->SetFontSize(32);
		label->SetPositionAnchor(glm::vec3(15.0f, 15.0f, 0.0f), Anchor::TOPLEFT);

		GUIManager::AddWidget(label, panel);

		std::shared_ptr<Button> button = std::make_shared<Button>();
		button->SetSize(glm::vec2(150.0f, 100.0f));
		button->SetPivot(Anchor::CENTER);
		button->SetPositionAnchor(glm::vec3(0.0f, 0.0f, 0.0f), Anchor::CENTER);

		button->textureColors[Button::STATE::NORMAL].color.a = 0.5f;
		button->textureColors[Button::STATE::HOVER].color.a = 0.7f;
		button->textureColors[Button::STATE::PRESSED].color.a = 0.9f;

		button->OnClickListener = GUI_METHOD_POINTER(TestGUIScript::OnButtonClick);

		GUIManager::AddWidget(button, panel);

		std::shared_ptr<Label> buttonLabel = std::make_shared<Label>();
		buttonLabel->SetText("Button");
		buttonLabel->SetFontSize(24);
		buttonLabel->SetPivot(Anchor::CENTER);
		buttonLabel->SetPositionAnchor(glm::vec3(0.0f), Anchor::CENTER);

		GUIManager::AddWidget(buttonLabel, button);
	}

	void OnButtonClick()
	{
		LogInfo("TestGUIScript::OnButtonClick");
	}
};
