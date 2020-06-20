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

	GameObject playerObject;
	GameObject bossObject;

	float defaultPhysicsMaxDelta;

	CameraPoint finalPoint;
	std::vector<CameraPoint> cameraPoints;

	std::shared_ptr<Panel> blackScreenPanel;
	std::shared_ptr<Panel> topBarPanel;
	std::shared_ptr<Panel> bottomBarPanel;
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
