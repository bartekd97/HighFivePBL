#pragma once

#include <vector>
#include <functional>
#include "../Script.h"
#include "HFEngine.h"
#include "GUI/GUIManager.h"
#include "GUI/Widgets.h"
#include "Utility/TimerAnimator.h"

class IntroMovie : public Script
{
private:
	const std::string GUILockName = "IntroMovie";

	glm::vec3 mainCameraLookOffset = { 0.0f, 2.5f, 0.0f };
	glm::vec3 mainCameraOffset = { 0.0f, 27.0f * 1.5f, 17.0f * 1.5f };

	struct CameraPoint
	{
		glm::vec3 position;
		glm::vec3 target;
		float scale;
		float timeToThisPoint;

		std::function<void()> OnReach;
		bool wasReached = false;
	};

	struct Subtitle
	{
		float time;
		std::string text;
	};

	GameObject playerObject;
	GameObject bossObject;

	float defaultPhysicsMaxDelta;

	CameraPoint finalPoint;
	std::vector<CameraPoint> cameraPoints;

	int subtitleIndex = -1;
	std::vector<Subtitle> subtitles;

	std::shared_ptr<Panel> blackScreenPanel;
	std::shared_ptr<Panel> topBarPanel;
	std::shared_ptr<Panel> bottomBarPanel;
	std::shared_ptr<Label> subtitlesLabel;
	int introGUILayer = 50;
	float screenBarSize = 0.15f;

	TimerAnimator timerAnimator;

	float dtsum = 0.0f;
	bool cameraAnimating = false;

public:
	
	~IntroMovie();

	void Start();

	void LateUpdate(float dt);

	void CameraMoveFinished();

	void FinishIntroAndClear();

	CameraPoint InterpolatePoint(CameraPoint& a, CameraPoint& b, float t);
	void UpdateCamera(CameraPoint& point);
};
