#include <map>
#include "MainMenu.h"
#include "HFEngine.h"
#include "ECS/Components/SkinAnimator.h"
#include "Resourcing/Prefab.h"
#include "GUI/GUIManager.h"
#include "GUI/Widgets.h"
#include "Resourcing/Texture.h"
#include "Scene/SceneManager.h"
#include "Audio/AudioManager.h"

namespace MainMenuUI
{
	std::shared_ptr<Panel> viniete = nullptr; // main menu parent
	GameObject creditsGameObject = NULL_GAMEOBJECT;
	std::shared_ptr<Prefab> creditsPrefab;

	std::map<int, std::shared_ptr<Button>> buttons;
	std::map<int, std::shared_ptr<Panel>> buttonHovers;
	//ALuint sourceMenu;

	void ButtonClicked(std::string name);

	void Show()
	{
		//AudioManager::CreateDefaultSourceAndPlay(sourceMenu, "menuKorpecki", true, 0.2f);
		if (viniete != nullptr)
		{
			// menu already created - just show it
			viniete->SetEnabled(true);
			return;
		}
		// otherwise create one

		auto library = TextureManager::GetLibrary("MainMenu");
		creditsPrefab = PrefabManager::GetPrefab("Credits");

		viniete = std::make_shared<Panel>();
		viniete->SetCoordinatesType(Widget::CoordinatesType::RELATIVE);
		viniete->SetPosition({0.0f, 0.0f, 0.0f});
		viniete->SetSize({ 1.0f, 1.0f });
		//viniete->SetSize({WindowManager::SCREEN_WIDTH, WindowManager::SCREEN_HEIGHT });
		viniete->textureColor.texture = library->GetTexture("menuViniete");
		GUIManager::AddWidget(viniete);

		glm::vec2 buttonSize = { 512, 72 };
		auto hoverBtnTexture = library->GetTexture("hoverBtn");
		auto MakeButton = [&](std::string name, int index, float yPos) {
			auto btnTexture = library->GetTexture(name);
			auto button = std::make_shared<Button>();
			button->SetAnchor(Anchor::BOTTOMRIGHT);
			button->SetPosition({ -buttonSize.x, yPos, 0.0f });
			button->SetSize(buttonSize);

			button->textureColors[Button::STATE::NORMAL].texture = btnTexture;
			button->textureColors[Button::STATE::NORMAL].color = { 1.0f, 1.0f, 1.0f, 0.5f };
			button->textureColors[Button::STATE::HOVER].texture = btnTexture;
			button->textureColors[Button::STATE::HOVER].color = { 1.0f, 1.0f, 1.0f, 1.0f };
			button->textureColors[Button::STATE::PRESSED].texture = btnTexture;
			button->textureColors[Button::STATE::PRESSED].color = { 1.0f, 1.0f, 1.0f, 0.5f };

			auto hoverBtn = std::make_shared<Panel>();
			hoverBtn->SetAnchor(Anchor::BOTTOMRIGHT);
			hoverBtn->SetPosition({ -buttonSize.x, yPos, 0.0f });
			hoverBtn->SetSize(buttonSize);
			hoverBtn->SetEnabled(false);
			hoverBtn->textureColor.texture = hoverBtnTexture;

			button->OnClickListener = std::bind(MainMenuUI::ButtonClicked, name);
			button->OnStateChanged = [index](Button::STATE newState) {
				MainMenuUI::buttonHovers[index]->SetEnabled(newState != Button::STATE::NORMAL);
			};

			GUIManager::AddWidget(hoverBtn, viniete);
			GUIManager::AddWidget(button, viniete);

			MainMenuUI::buttonHovers[index] = hoverBtn;
			MainMenuUI::buttons[index] = button;
		};

		// 72 + 1 to fix hovering 2 buttons at once
		MakeButton("btnStart", 0, -73.0f * 5.0f);
		MakeButton("btnStartLite", 1, -73.0f * 4.0f);
		MakeButton("btnCredits", 2, -73.0f * 3.0f);
		MakeButton("btnQuit", 3, -73.0f * 2.0f);
	}

	void Hide()
	{
		assert(viniete && "Menu not created yet");
		viniete->SetEnabled(false);
	}

	void ButtonClicked(std::string btnNaame)
	{
		if (btnNaame == "btnStart")
		{
			SceneManager::RequestLoadScene("Game");
		}
		else if (btnNaame == "btnStartLite")
		{
			SceneManager::RequestLoadScene("GameLite");
		}
		else if (btnNaame == "btnCredits")
		{
			if (creditsGameObject == NULL_GAMEOBJECT) creditsGameObject = creditsPrefab->Instantiate();
			else
			{
				if (!HFEngine::ECS.IsValidGameObject(creditsGameObject)) creditsGameObject = creditsPrefab->Instantiate();
			}
		}
		else if (btnNaame == "btnQuit")
		{
			HFEngine::Terminate();
		}
		//AudioManager::StopSource(sourceMenu);

	}
}


void MainMenuScene::OnLoad()
{
	//
	// SPAWN THINGS
	//
	AudioManager::StopBackground();
	AudioManager::PlayBackground("menuKorpecki", 0.2f);
	auto mainMenuPrefab = PrefabManager::GetPrefab("MainMenu");
	auto mainMenuObject = mainMenuPrefab->Instantiate();
	auto playerDummyObject = HFEngine::ECS.GetByNameInChildren(mainMenuObject, "PlayerDummy")[0];
	HFEngine::ECS.GetComponent<SkinAnimator>(playerDummyObject).SetAnimation("menuidle");

	//
	// SETUP ENVIRO
	//
	HFEngine::WorldLight.direction = { 0.45f, -0.2f, 0.75f };
	HFEngine::WorldLight.color = { 0.08f, 0.12f, 0.2f };
	HFEngine::WorldLight.ambient = { 0.01f, 0.02f, 0.04f };
	HFEngine::WorldLight.shadowIntensity = 1.0f;

	HFEngine::WorldLight.shadowmapMaxDistanceSteps = 17.0f;
	HFEngine::WorldLight.shadowmapScale = 1.75f;

	//HFEngine::MainCamera.SetScale(2.5f);
	//HFEngine::MainCamera.SetView({ -3.0f - 1.75f, 2.7f, -15.0f }, { 1.0f - 1.75f, 2.75f, 0.0f });
	HFEngine::MainCamera.SetScale(2.9f);
	HFEngine::MainCamera.SetView({ -3.0f - 1.95f, 2.75f, -15.0f }, { 1.0f - 1.95f, 2.8f, 0.0f });

	//
	// SHOW UI
	//
	MainMenuUI::Show();
}

void MainMenuScene::OnUnload()
{
	// close menu
	MainMenuUI::Hide();
}
