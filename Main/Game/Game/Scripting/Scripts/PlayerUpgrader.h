#pragma once

#include <vector>
#include <random>
#include <glm/glm.hpp>
#include <glm/gtx/easing.hpp>
#include "../Script.h"
#include "GUI/GUIManager.h"
#include "GUI/Widgets.h"
#include "Resourcing/Prefab.h"
#include "Event/Events.h"
#include "Utility/TimerAnimator.h"
#include "HFEngine.h"

class PlayerUpgrader : public Script
{
public:
	struct UpgradeButton
	{
		std::shared_ptr<Button> button;
		std::shared_ptr<Prefab> upgradePrefab;
	};

private: // parameters
	float buttonYPos = -0.1f;

private: // variables
	GameObject upgradeContainer;

	std::shared_ptr<Panel> upgradePanel;
	UpgradeButton upgradeButtons[3];
	int currentHoveredButton = -1;
	glm::vec2 upgradeButtonSize = { 0.14f, 0.37f };
	std::shared_ptr<Label> descriptionLabel;
	bool canClickUpgradeButton = false;

	std::vector<std::shared_ptr<Prefab>> upgradePrefabs;

	TimerAnimator timerAnimator;

	void MakeUpgradeButton(int index, float posX)
	{
		auto button = std::make_shared<Button>();
		button->SetCoordinatesType(Widget::CoordinatesType::RELATIVE);
		button->SetAnchor(Anchor::CENTER);
		button->SetPivot(Anchor::CENTER);
		button->SetPosition({ posX, buttonYPos, 0.0f });
		button->SetSize(upgradeButtonSize);
		button->textureColors[Button::STATE::NORMAL].color = glm::vec4(0.7f, 0.7f, 0.7f, 0.7f);
		button->textureColors[Button::STATE::HOVER].color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
		button->OnStateChanged = std::bind(&PlayerUpgrader::ButtonStateChanged, this, index, std::placeholders::_1);
		button->OnClickListener = std::bind(&PlayerUpgrader::ButtonClicked, this, index);

		upgradeButtons[index].button = button;
		GUIManager::AddWidget(button, upgradePanel);
	}

public:
	void Awake()
	{
		upgradeContainer = HFEngine::ECS.CreateGameObject(GetGameObject(), "Upgrades");
		EventManager::AddScriptListener(SCRIPT_LISTENER(Events::Gameplay::Upgrades::REQUEST_UPGRADE, PlayerUpgrader::OnRequestUpgrade));
	}

	void Start()
	{
		upgradePanel = std::make_shared<Panel>();
		upgradePanel->SetCoordinatesType(Widget::CoordinatesType::RELATIVE);
		upgradePanel->SetPivot(Anchor::CENTER);
		upgradePanel->SetPositionAnchor({ 0.0f, 0.0f, 0.0f }, Anchor::CENTER);
		upgradePanel->SetSize({ 1.0f, 1.0f });
		upgradePanel->textureColor.color = {0.0f, 0.0f, 0.0f, 0.5f};
		GUIManager::AddWidget(upgradePanel, nullptr, 2);

		MakeUpgradeButton(0, -upgradeButtonSize.x * 1.5f);
		MakeUpgradeButton(1, 0.0f);
		MakeUpgradeButton(2, upgradeButtonSize.x * 1.5f);

		descriptionLabel = std::make_shared<Label>();
		descriptionLabel->SetCoordinatesType(Widget::CoordinatesType::RELATIVE);
		descriptionLabel->SetAnchor(Anchor::CENTER);
		descriptionLabel->SetPivot(Anchor::CENTER);
		descriptionLabel->SetFontSize(32);
		descriptionLabel->SetPosition({ 0.0f, 0.3f, 0.0f });
		descriptionLabel->color = {1.0f, 1.0f, 1.0f, 1.0f};
		GUIManager::AddWidget(descriptionLabel, upgradePanel);

		upgradePanel->SetEnabled(false);

		upgradePrefabs = {
			PrefabManager::GetPrefab("Upgrades/GhostDistance"),
			PrefabManager::GetPrefab("Upgrades/GhostRecovery"),
			PrefabManager::GetPrefab("Upgrades/GhostSpeed"),
		};
	}

	void ButtonStateChanged(int index, Button::STATE newState)
	{
		bool hovered = newState == Button::STATE::HOVER || newState == Button::STATE::PRESSED;
		if (hovered)
		{
			currentHoveredButton = index;
		}
		else
		{
			if (currentHoveredButton == index)
				currentHoveredButton = -1;
		}

		if (currentHoveredButton == -1)
		{
			descriptionLabel->SetText("");
		}
		else
		{
			std::string description;
			upgradeButtons[currentHoveredButton].upgradePrefab->Properties().GetString("description", description);
			descriptionLabel->SetText(description);
		}
	}


	void ButtonClicked(int index)
	{
		if (!canClickUpgradeButton) return;

		// create upgrade
		upgradeButtons[index].upgradePrefab->Instantiate(upgradeContainer);

		// fade out buttons
		for (int i = 0; i < 3; i++)
		{
			float posY = upgradeButtons[i].button->GetPosition().x;
			timerAnimator.UpdateInTime(i == index ? 1.2f : 0.5f, [&, i, posY](float prog) {
				upgradeButtons[i].button->SetPosition({
					posY, glm::mix(buttonYPos, -1.0f, glm::sineEaseInOut(prog)), 0.0f
					});
				});
		}
		timerAnimator.AnimateVariable(&upgradePanel->textureColor.color.a, upgradePanel->textureColor.color.a, 0.0f, 0.5f);
		timerAnimator.DelayAction(1.5f, [&]() {upgradePanel->SetEnabled(false);});

		canClickUpgradeButton = false;
	}


	void OnRequestUpgrade(Event& ev)
	{
		std::vector<std::shared_ptr<Prefab>> upgrades = upgradePrefabs; // make copy

		static std::random_device rd;
		static std::mt19937 gen(rd());
		for (int i = 0; i < 3; i++)
		{
			std::uniform_int_distribution<> dis(0, upgrades.size() - 1);
			int ui = dis(gen);
			upgradeButtons[i].upgradePrefab = upgrades[ui];
			upgrades.erase(upgrades.begin() + ui);
		}

		upgradePanel->SetEnabled(true);
		timerAnimator.AnimateVariable(&upgradePanel->textureColor.color.a, 0.0f, 0.6f, 1.0f);

		float posX0 = upgradeButtons[0].button->GetPosition().x;
		float posX1 = upgradeButtons[1].button->GetPosition().x;
		float posX2 = upgradeButtons[2].button->GetPosition().x;

		upgradeButtons[0].button->SetPosition({ posX0, -1.0f, 0.0f });
		upgradeButtons[1].button->SetPosition({ posX1, -1.0f, 0.0f });
		upgradeButtons[2].button->SetPosition({ posX2, -1.0f, 0.0f });

		timerAnimator.DelayAction(0.6f, [&, posX0]() {
			timerAnimator.UpdateInTime(0.8f, [&, posX0](float prog) {
				upgradeButtons[0].button->SetPosition({
					posX0, glm::mix(-1.0f, buttonYPos, glm::backEaseOut(prog)), 0.0f
					});
				});
			});
		timerAnimator.DelayAction(0.75f, [&, posX1]() {
			timerAnimator.UpdateInTime(0.8f, [&, posX1](float prog) {
				upgradeButtons[1].button->SetPosition({
					posX1, glm::mix(-1.0f, buttonYPos, glm::backEaseOut(prog)), 0.0f
					});
				});
			});
		timerAnimator.DelayAction(0.9f, [&, posX2]() {
			timerAnimator.UpdateInTime(0.8f, [&, posX2](float prog) {
				upgradeButtons[2].button->SetPosition({
					posX2, glm::mix(-1.0f, buttonYPos, glm::backEaseOut(prog)), 0.0f
					});
				});
			});

		descriptionLabel->SetText("");

		timerAnimator.DelayAction(1.5f, [&]() {
			canClickUpgradeButton = true;
			});
	}


	void Update(float dt)
	{
		timerAnimator.Process(dt);
	}
};
