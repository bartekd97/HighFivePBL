#pragma once

#include "../Script.h"
#include "../../GUI/GUIManager.h"
#include "../../GUI/Panel.h"
#include "../../InputManager.h"
#include "../../Scene/SceneManager.h"

class Credits : public Script
{
private:
	std::shared_ptr<Panel> creditsPanel;
	float scrollingSpeed = 0.1f;
	bool scrolling = false;
	TimerAnimator timerAnimator;
	float fadeTime = 6.5f;

public:
	~Credits()
	{
		GUIManager::RemoveWidget(creditsPanel);
	}

	void Start()
	{
		creditsPanel = std::make_shared<Panel>();
		creditsPanel->SetCoordinatesType(Widget::CoordinatesType::RELATIVE);
		creditsPanel->SetPositionAnchor({ 0.0f, 0.0f, 0.0f }, Anchor::TOPLEFT);
		creditsPanel->SetSize({ 1.0f, 3.0f });
		creditsPanel->textureColor.texture = TextureManager::GetTexture("GUI/Credits", "credits");

		GUIManager::AddWidget(creditsPanel, nullptr, 5);

		timerAnimator.DelayAction(fadeTime, [&]() {
			scrolling = true;
			});
		timerAnimator.AnimateVariable(&creditsPanel->textureColor.color, glm::vec4(glm::vec3(1.0f), 0.0f), glm::vec4(1.0f), fadeTime);
	}

	void Update(float dt)
	{
		timerAnimator.Process(dt);
		if (scrolling)
		{
			creditsPanel->SetPosition(creditsPanel->GetPosition() - (glm::vec3(0.0f, 1.0f, 0.0f) * scrollingSpeed * dt));
			if (creditsPanel->GetPosition().y <= -2.0f) creditsPanel->SetPosition({ 0.0f, -2.0f, 0.0f });
		}

		if (InputManager::GetKeyDown(GLFW_KEY_SPACE)) Hide();
	}

	void Hide()
	{
		if (SceneManager::GetLoadedScene() != "MainMenu")
		{
			SceneManager::RequestLoadScene("MainMenu");
		}
		else
		{
			DestroyGameObjectSafely();
		}

	}
};
