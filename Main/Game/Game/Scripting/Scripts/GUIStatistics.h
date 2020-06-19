#pragma once

#include <iomanip>
#include <sstream>

#include "../Script.h"
#include "../../HFEngine.h"

#include "../../GUI/GUIManager.h"
#include "../../GUI/Widgets.h"

class GUIStatistics : public Script
{
public:
	~GUIStatistics()
	{
		GUIManager::RemoveWidget(panel);
	}

	void Awake()
	{
		hidden = false;
		enabled = true;
		expandTexture = TextureManager::GetLibrary("GUI")->GetTexture("expand");
		collapseTexture = TextureManager::GetLibrary("GUI")->GetTexture("collapse");

		panel = std::make_shared<Panel>();
		panel->SetPositionAnchor(glm::vec3(-250.0f, 50.0f, 0.0f), Anchor::TOPRIGHT);
		panel->SetSize(glm::vec2(200.0f, 300.0f));
		panel->SetClipping(true);
		panel->textureColor.color = glm::vec4(1.0f, 0.0f, 0.0f, 0.8f);

		GUIManager::AddWidget(panel, nullptr, 2);

		std::shared_ptr<Label> label = std::make_shared<Label>();
		label->SetText("Statistics");
		label->SetFontSize(28);
		label->SetPivot(Anchor::CENTER);
		label->SetPositionAnchor(glm::vec3(0.0f, panel->GetSize().y / -2.0f + 15.0f, 0.0f), Anchor::CENTER);

		GUIManager::AddWidget(label, panel);

		hideShowButton = std::make_shared<Button>();
		hideShowButton->SetPositionAnchor(glm::vec3(-7.5f, 7.5f, 0.0f), Anchor::TOPRIGHT);
		hideShowButton->SetSize(glm::vec2(20.0f, 20.0f));
		hideShowButton->SetPivot(Anchor::TOPRIGHT);
		hideShowButton->OnClickListener = GUI_METHOD_POINTER(GUIStatistics::OnHideShowButtonClick);

		for (int i = (int)Button::STATE::NORMAL; i <= (int)Button::STATE::PRESSED; i++)
		{
			hideShowButton->textureColors[(Button::STATE)i].texture = collapseTexture;
			hideShowButton->textureColors[(Button::STATE)i].color = glm::vec4(glm::vec3(0.6f + (i * 0.2f)), 1.0f);
		}

		GUIManager::AddWidget(hideShowButton, panel);

		fpsLabel = std::make_shared<Label>();
		fpsLabel->SetText(fpsPrefix);
		fpsLabel->SetFontSize(16);
		fpsLabel->SetPivot(Anchor::TOPLEFT);
		fpsLabel->SetPositionAnchor(glm::vec3(10.0f, 45.0f, 0.0f), Anchor::TOPLEFT);
		GUIManager::AddWidget(fpsLabel, panel);

		gameObjectCountLabel = std::make_shared<Label>();
		gameObjectCountLabel->SetText(GOCountPrefix);
		gameObjectCountLabel->SetFontSize(16);
		gameObjectCountLabel->SetPivot(Anchor::TOPLEFT);
		gameObjectCountLabel->SetPositionAnchor(glm::vec3(10.0f, 65.0f, 0.0f), Anchor::TOPLEFT);
		GUIManager::AddWidget(gameObjectCountLabel, panel);

		renderedObjectsCountLabel = std::make_shared<Label>();
		renderedObjectsCountLabel->SetText(RenderedObjectsCountPrefix);
		renderedObjectsCountLabel->SetFontSize(16);
		renderedObjectsCountLabel->SetPivot(Anchor::TOPLEFT);
		renderedObjectsCountLabel->SetPositionAnchor(glm::vec3(10.0f, 85.0f, 0.0f), Anchor::TOPLEFT);
		GUIManager::AddWidget(renderedObjectsCountLabel, panel);

		renderedPointLightsCountLabel = std::make_shared<Label>();
		renderedPointLightsCountLabel->SetText(RenderedPointLightsCountPrefix);
		renderedPointLightsCountLabel->SetFontSize(16);
		renderedPointLightsCountLabel->SetPivot(Anchor::TOPLEFT);
		renderedPointLightsCountLabel->SetPositionAnchor(glm::vec3(10.0f, 105.0f, 0.0f), Anchor::TOPLEFT);
		GUIManager::AddWidget(renderedPointLightsCountLabel, panel);

		renderedParticlesCountLabel = std::make_shared<Label>();
		renderedParticlesCountLabel->SetText(RenderedParticlesCountPrefix);
		renderedParticlesCountLabel->SetFontSize(16);
		renderedParticlesCountLabel->SetPivot(Anchor::TOPLEFT);
		renderedParticlesCountLabel->SetPositionAnchor(glm::vec3(10.0f, 125.0f, 0.0f), Anchor::TOPLEFT);
		GUIManager::AddWidget(renderedParticlesCountLabel, panel);

		vsyncButton = std::make_shared<Button>();
		vsyncButton->SetPositionAnchor(glm::vec3(10.0f, 150.0f, 0.0f), Anchor::TOPLEFT);
		vsyncButton->SetSize(glm::vec2(100.0f, 30.0f));
		vsyncButton->SetPivot(Anchor::TOPLEFT);
		vsyncButton->OnClickListener = GUI_METHOD_POINTER(GUIStatistics::OnVsyncButtonClick);

		for (int i = (int)Button::STATE::NORMAL; i <= (int)Button::STATE::PRESSED; i++)
		{
			vsyncButton->textureColors[(Button::STATE)i].color = glm::vec4(glm::vec3(0.6f + (i * 0.2f)), 1.0f);
		}

		GUIManager::AddWidget(vsyncButton, panel);

		vsyncLabel = std::make_shared<Label>();
		vsyncLabel->SetText("Disable vsync");
		vsyncLabel->SetFontSize(14);
		vsyncLabel->SetPivot(Anchor::CENTER);
		vsyncLabel->SetPositionAnchor(glm::vec3(0.0f, 0.0f, 0.0f), Anchor::CENTER);
		GUIManager::AddWidget(vsyncLabel, vsyncButton);
		panel->SetEnabled(false);
	}

	void Update(float dt)
	{
		if (InputManager::GetKeyDown(GLFW_KEY_F2))
		{
			SwitchEnabled();
		}

		if (enabled)
		{
			accumulator += dt;
			frames += 1;
			if (isgreaterequal(accumulator, reportInterval))
			{
				std::stringstream ss;
				ss << std::fixed << std::setprecision(4) << frames / accumulator;

				fpsLabel->SetText(fpsPrefix + ss.str());
				gameObjectCountLabel->SetText(GOCountPrefix + std::to_string(HFEngine::ECS.GetLivingGameObjectsCount()));
				renderedObjectsCountLabel->SetText(RenderedObjectsCountPrefix + std::to_string(HFEngine::Renderer.GetLastFrameStats().renderedObjects));
				renderedPointLightsCountLabel->SetText(RenderedPointLightsCountPrefix + std::to_string(HFEngine::Renderer.GetLastFrameStats().renderedPointLights));
				renderedParticlesCountLabel->SetText(RenderedParticlesCountPrefix + std::to_string(HFEngine::Renderer.GetLastFrameStats().renderedParticleEmitters));

				accumulator = 0.0f;
				frames = 0;
			}
		}
	}

	void OnHideShowButtonClick()
	{
		for (int i = (int)Button::STATE::NORMAL; i <= (int)Button::STATE::PRESSED; i++)
		{
			hideShowButton->textureColors[(Button::STATE)i].texture = hidden ? collapseTexture : expandTexture;
		}
		if (hidden)
		{
			panel->SetSize(glm::vec2(panel->GetSize().x, panelFullHeight));
			hidden = false;
		}
		else
		{
			panel->SetSize(glm::vec2(panel->GetSize().x, panelHiddenHeight));
			hidden = true;
		}
	}

	void OnVsyncButtonClick()
	{
		static bool vsyncEnabled = true;
		vsyncEnabled = !vsyncEnabled;
		glfwSwapInterval((int)vsyncEnabled);
		vsyncLabel->SetText(vsyncEnabled ? "Disable vsync" : "Enable vsync");
	}

private:
	void SwitchEnabled()
	{
		if (enabled)
		{
			enabled = false;
			accumulator = 0.0f;
			frames = 0;
		}
		else
		{
			enabled = true;
		}
		panel->SetEnabled(enabled);
	}

	const float panelFullHeight = 300.0f;
	const float panelHiddenHeight = 35.0f;
	const float reportInterval = 0.2f;
	const char* fpsPrefix = "FPS: ";
	const char* GOCountPrefix = "GameObjects: ";
	const char* RenderedObjectsCountPrefix = "Vis Objects: ";
	const char* RenderedPointLightsCountPrefix = "Vis PointLights: ";
	const char* RenderedParticlesCountPrefix = "Vis Particles: ";
	std::shared_ptr<Texture> expandTexture;
	std::shared_ptr<Texture> collapseTexture;
	std::shared_ptr<Panel> panel;
	std::shared_ptr<Button> hideShowButton;
	std::shared_ptr<Label> fpsLabel;
	std::shared_ptr<Label> gameObjectCountLabel;
	std::shared_ptr<Label> renderedObjectsCountLabel;
	std::shared_ptr<Label> renderedPointLightsCountLabel;
	std::shared_ptr<Label> renderedParticlesCountLabel;
	std::shared_ptr<Button> vsyncButton;
	std::shared_ptr<Label> vsyncLabel;

	bool hidden;
	bool enabled;

	float accumulator;
	int frames;
};
