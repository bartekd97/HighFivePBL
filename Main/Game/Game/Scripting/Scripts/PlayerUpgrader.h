#pragma once

#include <vector>
#include <unordered_map>
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
		std::shared_ptr<Panel> buttonIcon;
		std::shared_ptr<Prefab> upgradePrefab;

		std::shared_ptr<Texture> iconNormal;
		std::shared_ptr<Texture> iconHover;
	};

	struct BarIcon
	{
		std::shared_ptr<Panel> background;
		std::shared_ptr<Panel> icon;
	};

private: // parameters
	float buttonYPos = -0.1f;

	const int MaxUpgradesOnBar = 8;
	const std::string GUILockName = "PlayerUpgrader";

private: // variables
	GameObject upgradeContainer;

	std::shared_ptr<Panel> upgradePanel;
	UpgradeButton upgradeButtons[3];
	int currentHoveredButton = -1;
	glm::vec2 upgradeButtonSize = glm::vec2{ 0.25f, 0.7111f } * 0.75f;
	glm::vec3 upgradeButtonIconPosition = glm::vec3{ 0.0f, -0.012f, 0.0f };
	glm::vec2 upgradeButtonIconSize = glm::vec2{ 0.3125f, 0.1953f } * 1.5f; // 100x100 x multiplier
	std::shared_ptr<Label> descriptionLabel;
	bool canClickUpgradeButton = false;

	std::shared_ptr<Panel> upgradesBarAvatarPanel;
	std::shared_ptr<Panel> upgradesBarPanel;
	std::vector<BarIcon> upgradesBarIcons;
	std::shared_ptr<Panel> upgradeAnimIconPanel;

	std::vector<std::shared_ptr<Prefab>> upgradePrefabs;
	std::unordered_map<std::string, std::shared_ptr<Texture>> iconTexturesCache;

	TimerAnimator timerAnimator;

	void MakeUpgradeButton(int index, float posX)
	{
		auto button = std::make_shared<Button>();
		button->SetCoordinatesType(Widget::CoordinatesType::RELATIVE);
		button->SetAnchor(Anchor::CENTER);
		button->SetPivot(Anchor::CENTER);
		button->SetPosition({ posX, buttonYPos, 0.0f });
		button->SetSize(upgradeButtonSize);
		button->textureColors[Button::STATE::NORMAL].texture = TextureManager::GetTexture("GUI/Upgrades", "UpgradeStone");
		button->textureColors[Button::STATE::HOVER].color = {0.75f, 0.75f, 0.75f, 1.0f};
		button->OnStateChanged = std::bind(&PlayerUpgrader::ButtonStateChanged, this, index, std::placeholders::_1);
		button->OnClickListener = std::bind(&PlayerUpgrader::ButtonClicked, this, index);

		auto buttonIcon = std::make_shared<Panel>();
		buttonIcon->SetCoordinatesType(Widget::CoordinatesType::RELATIVE);
		buttonIcon->SetAnchor(Anchor::CENTER);
		buttonIcon->SetPivot(Anchor::CENTER);
		buttonIcon->SetPosition(upgradeButtonIconPosition);
		buttonIcon->SetSize(upgradeButtonIconSize);


		upgradeButtons[index].button = button;
		upgradeButtons[index].buttonIcon = buttonIcon;
		GUIManager::AddWidget(button, upgradePanel);
		GUIManager::AddWidget(buttonIcon, button);
	}

public:
	~PlayerUpgrader()
	{
		GUIManager::RemoveWidget(upgradePanel);
		GUIManager::RemoveWidget(upgradesBarPanel);
		GUIManager::RemoveWidget(upgradeAnimIconPanel);
	}

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

		// create bar with avatar and icons
		upgradesBarPanel = std::make_shared<Panel>();
		upgradesBarPanel->SetCoordinatesType(Widget::CoordinatesType::RELATIVE);
		upgradesBarPanel->SetPivot(Anchor::BOTTOMLEFT);
		upgradesBarPanel->SetPositionAnchor({ 0.045f, -0.06f, 0.0f }, Anchor::BOTTOMLEFT);
		upgradesBarPanel->SetSize(glm::vec2{ 0.357f, 0.0695f * 0.286f } * 1.0f);
		upgradesBarPanel->textureColor.texture = TextureManager::GetTexture("GUI/Upgrades", "UpgradesThinBar");
		GUIManager::AddWidget(upgradesBarPanel);

		upgradesBarAvatarPanel = std::make_shared<Panel>();
		upgradesBarAvatarPanel->SetCoordinatesType(Widget::CoordinatesType::RELATIVE);
		upgradesBarAvatarPanel->SetPivot(Anchor::CENTER);
		upgradesBarAvatarPanel->SetPositionAnchor({ 0.0f, 0.5f, 0.0f }, Anchor::TOPLEFT);
		upgradesBarAvatarPanel->SetSize(glm::vec2{ 0.165f, 1.5f / 0.286f } * 0.8f);
		upgradesBarAvatarPanel->textureColor.texture = TextureManager::GetTexture("GUI/Upgrades", "UpgradesBarAvatar");
		GUIManager::AddWidget(upgradesBarAvatarPanel, upgradesBarPanel);

		for (int i = 0; i < MaxUpgradesOnBar; i++)
		{
			BarIcon iconPanel;
			iconPanel.background = std::make_shared<Panel>();
			iconPanel.background->SetCoordinatesType(Widget::CoordinatesType::RELATIVE);
			iconPanel.background->SetPivot(Anchor::CENTER);
			iconPanel.background->SetPositionAnchor({ 0.15f + 0.11f*float(i), 0.5f, 0.0f }, Anchor::TOPLEFT);
			iconPanel.background->SetSize(glm::vec2{ 0.0825f, 0.77f / 0.286f } * 0.95f);
			iconPanel.background->textureColor.texture = TextureManager::GetTexture("GUI/Upgrades", "UpgradesBarUpgradeBackground");
			iconPanel.icon = std::make_shared<Panel>();
			iconPanel.icon->SetCoordinatesType(Widget::CoordinatesType::RELATIVE);
			iconPanel.icon->SetSize(glm::vec2{ 1.0f, 1.0f });
			upgradesBarIcons.push_back(iconPanel);
			GUIManager::AddWidget(iconPanel.background, upgradesBarPanel);
			GUIManager::AddWidget(iconPanel.icon, iconPanel.background);
			iconPanel.background->SetEnabled(false);
			iconPanel.icon->SetEnabled(false);
		}

		// create icon for animating
		upgradeAnimIconPanel = std::make_shared<Panel>();
		upgradeAnimIconPanel->SetEnabled(false);
		GUIManager::AddWidget(upgradeAnimIconPanel, nullptr, 2);


		// setup upgrade prefabs
		upgradePrefabs = {
			PrefabManager::GetPrefab("Upgrades/GhostDistance"),
			PrefabManager::GetPrefab("Upgrades/GhostRecovery"),
			PrefabManager::GetPrefab("Upgrades/GhostSpeed"),
			PrefabManager::GetPrefab("Upgrades/GhostActiveLines"),
			PrefabManager::GetPrefab("Upgrades/PushbackImprovement"),
		};
		
		// cache upgrade icons
		for (auto& upgrade : upgradePrefabs)
		{
			std::string iconName;
			upgrade->Properties().GetString("blackIcon", iconName);
			iconTexturesCache[iconName] = TextureManager::GetTexture("GUI/Upgrades", iconName);
			upgrade->Properties().GetString("whiteIcon", iconName);
			iconTexturesCache[iconName] = TextureManager::GetTexture("GUI/Upgrades", iconName);
		}
	}

	void ButtonStateChanged(int index, Button::STATE newState)
	{
		bool hovered = newState == Button::STATE::HOVER || newState == Button::STATE::PRESSED;
		if (hovered)
		{
			currentHoveredButton = index;
			if (upgradeButtons[index].iconHover)
				upgradeButtons[index].buttonIcon->textureColor.texture = upgradeButtons[index].iconHover;
		}
		else
		{
			if (currentHoveredButton == index)
				currentHoveredButton = -1;

			if (upgradeButtons[index].iconNormal)
				upgradeButtons[index].buttonIcon->textureColor.texture = upgradeButtons[index].iconNormal;
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

	std::optional<BarIcon> GetFreeUpgradeSlotOnBar()
	{
		for (auto& iconSlot : upgradesBarIcons)
		{
			if (!iconSlot.background->GetEnabled())
			{
				return iconSlot;
			}
		}
		return std::nullopt;
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


		std::optional<BarIcon> freeIconSlot = GetFreeUpgradeSlotOnBar();
		if (freeIconSlot.has_value())
		{
			glm::vec3 posFrom = upgradeButtons[index].buttonIcon->GetAbsolutePosition();
			glm::vec2 scaleFrom = upgradeButtons[index].buttonIcon->GetLocalSize();
			glm::vec3 posTo = freeIconSlot->background->GetAbsolutePosition();
			glm::vec2 scaleTo = freeIconSlot->background->GetLocalSize();

			freeIconSlot->background->SetEnabled(true);
			freeIconSlot->icon->SetEnabled(false);

			upgradeAnimIconPanel->SetEnabled(true);
			upgradeAnimIconPanel->textureColor.texture = upgradeButtons[index].buttonIcon->textureColor.texture;
			timerAnimator.UpdateInTime(1.2f, [&, posFrom, scaleFrom, posTo, scaleTo](float prog) {
				upgradeAnimIconPanel->SetPosition(
					glm::mix(posFrom, posTo, glm::sineEaseInOut(prog))
					);
				upgradeAnimIconPanel->SetSize(
					glm::mix(scaleFrom, scaleTo, glm::sineEaseInOut(prog))
					);
				});
			timerAnimator.DelayAction(1.2f, [&, freeIconSlot, index]() {
				upgradeAnimIconPanel->SetEnabled(false);
				freeIconSlot->icon->SetEnabled(true);
				freeIconSlot->icon->textureColor.texture = upgradeAnimIconPanel->textureColor.texture;
				});

			upgradeButtons[index].buttonIcon->textureColor.color.a = 0.25f;
		}

		canClickUpgradeButton = false;

		GUIManager::KeybindLock::Release(GUILockName);
	}


	void OnRequestUpgrade(Event& ev)
	{
		if (!GetFreeUpgradeSlotOnBar().has_value())
		{
			AudioManager::PlayFromDefaultSource("upgradesFull", false, 0.2f);

			return;
		}

		std::vector<std::shared_ptr<Prefab>> upgrades = upgradePrefabs; // make copy

		static std::random_device rd;
		static std::mt19937 gen(rd());
		for (int i = 0; i < 3; i++)
		{
			std::uniform_int_distribution<> dis(0, upgrades.size() - 1);
			int ui = dis(gen);
			upgradeButtons[i].upgradePrefab = upgrades[ui];

			std::string iconNameBlack, iconNameWhite;
			upgrades[ui]->Properties().GetString("blackIcon", iconNameBlack);
			upgrades[ui]->Properties().GetString("whiteIcon", iconNameWhite);
			upgradeButtons[i].iconNormal = iconTexturesCache[iconNameBlack];
			upgradeButtons[i].iconHover = iconTexturesCache[iconNameWhite];
			upgradeButtons[i].buttonIcon->textureColor.texture = upgradeButtons[i].iconNormal;
			upgradeButtons[i].buttonIcon->textureColor.color.a = 1.0f;
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

		GUIManager::KeybindLock::Set(GUILockName);
	}


	void Update(float dt)
	{
		timerAnimator.Process(dt);
	}
};
