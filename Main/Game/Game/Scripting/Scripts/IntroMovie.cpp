#include <glm/glm.hpp>
#include <glm/gtx/easing.hpp>
#include "IntroMovie.h"
#include "PlayerCameraFollower.h"
#include "ECS/Components/MapLayoutComponents.h"
#include "ECS/Components/ScriptContainer.h"
#include "Physics/Physics.h"
#include "Audio/AudioManager.h"
#include "InputManager.h"
#include "MapCellOptimizer.h"

IntroMovie::~IntroMovie()
{
	GUIManager::RemoveWidget(blackScreenPanel);
	GUIManager::RemoveWidget(topBarPanel);
	GUIManager::RemoveWidget(bottomBarPanel);
	GUIManager::RemoveWidget(subtitlesLabel);
}

void IntroMovie::Start()
{
	playerObject = HFEngine::ECS.GetGameObjectByName("Player").value();
	bossObject = HFEngine::ECS.GetGameObjectByName("boss").value();


	blackScreenPanel = std::make_shared<Panel>();
	blackScreenPanel->SetCoordinatesType(Widget::CoordinatesType::RELATIVE);
	blackScreenPanel->SetPosition({ 0.0f, 0.0f, 0.0f });
	blackScreenPanel->SetSize({ 1.0f, 1.0f });
	blackScreenPanel->textureColor.color = { 0.0f, 0.0f, 0.0f, 1.0f };
	GUIManager::AddWidget(blackScreenPanel, nullptr, introGUILayer);

	topBarPanel = std::make_shared<Panel>();
	topBarPanel->SetCoordinatesType(Widget::CoordinatesType::RELATIVE);
	topBarPanel->SetPivot(Anchor::TOPLEFT);
	topBarPanel->SetPositionAnchor({ 0.0f, 0.0f, 0.0f }, Anchor::TOPLEFT);
	topBarPanel->SetSize({ 1.0f, screenBarSize });
	topBarPanel->textureColor.color = { 0.0f, 0.0f, 0.0f, 1.0f };
	GUIManager::AddWidget(topBarPanel, nullptr, introGUILayer);

	bottomBarPanel = std::make_shared<Panel>();
	bottomBarPanel->SetCoordinatesType(Widget::CoordinatesType::RELATIVE);
	bottomBarPanel->SetPivot(Anchor::BOTTOMLEFT);
	bottomBarPanel->SetPositionAnchor({ 0.0f, 0.0f, 0.0f }, Anchor::BOTTOMLEFT);
	bottomBarPanel->SetSize({ 1.0f, screenBarSize });
	bottomBarPanel->textureColor.color = { 0.0f, 0.0f, 0.0f, 1.0f };
	GUIManager::AddWidget(bottomBarPanel, nullptr, introGUILayer);


	subtitlesLabel = std::make_shared<Label>();
	subtitlesLabel->SetCoordinatesType(Widget::CoordinatesType::RELATIVE);
	subtitlesLabel->SetAnchor(Anchor::CENTER);
	subtitlesLabel->SetPivot(Anchor::CENTER);
	subtitlesLabel->SetFontSize(32);
	subtitlesLabel->SetPosition({ 0.0f, 0.5f - screenBarSize * 1.325f, 0.0f });
	subtitlesLabel->color = { 0.75f, 0.75f, 0.4f, 1.0f };
	GUIManager::AddWidget(subtitlesLabel, nullptr, introGUILayer);


	for (int i = 0; i < introGUILayer; i++)
		GUIManager::SetLayerEnabled(i, false);

	auto& scriptContainer = HFEngine::ECS.GetComponent<ScriptContainer>(playerObject);
	auto cameraFollower = scriptContainer.GetScript<PlayerCameraFollower>();

	finalPoint.target = cameraFollower->GetCameraTargetPosition();
	finalPoint.position = cameraFollower->GetCameraTargetSourcePosition(finalPoint.target);
	finalPoint.scale = HFEngine::MainCamera.GetScale();

	auto& bossTransform = HFEngine::ECS.GetComponent<Transform>(bossObject);
	auto& playerTransform = HFEngine::ECS.GetComponent<Transform>(playerObject);

	CameraPoint point;
	// 1
	point.position = bossTransform.GetWorldPosition() + mainCameraOffset + mainCameraLookOffset;
	point.target = bossTransform.GetWorldPosition() + mainCameraLookOffset;
	point.scale = HFEngine::MainCamera.GetScale();
	point.timeToThisPoint = 2.5f;
	point.OnReach = [&]() {
		timerAnimator.AnimateVariable(&blackScreenPanel->textureColor.color.a, 1.0f, 0.0f, 2.5f);
	};
	cameraPoints.push_back(point);

	// 2
	//point.position.y *= 0.75f;
	point.scale *= 0.5f;
	point.timeToThisPoint = 2.5f;
	point.OnReach = [&]() {
		EventManager::FireEventTo(bossObject, Events::Gameplay::Boss::INTRO_ACTION);
	};
	cameraPoints.push_back(point);
	point.OnReach = nullptr;

	point.scale *= 0.85f;
	point.timeToThisPoint = 4.0f;
	cameraPoints.push_back(point);

	// 3
	point.position = glm::mix(point.position, finalPoint.position + glm::vec3{ 0.0f, 10.0f, 0.0f }, 0.1f);
	point.scale /= 0.85f;
	point.scale *= 3.0f;
	point.timeToThisPoint = 7.0f;
	cameraPoints.push_back(point);

	// 4
	point.position = finalPoint.position + glm::vec3{0.0f, 10.0f, 0.0f};
	point.target = glm::mix(point.target, finalPoint.target, 0.9f);
	point.scale *= 1.5f;
	point.timeToThisPoint = 22.0f;
	cameraPoints.push_back(point);

	// 5
	finalPoint.timeToThisPoint = 8.0f;
	cameraPoints.push_back(finalPoint);


	// subtitles
	subtitles = {
		{0.0f, ""},
		{4.75f, "Dark times have come for the whole known world."},
		{9.25f, "The powerful... Necromancer..."},
		{12.5f, "...led the most filthy creatures to crawl out of the pit of hell."}, 
		{17.5f, "When everything seemed to be lost,"},
		{20.25f, "help came from the least expected place."},
		{24.5f, "The last hope for the victory of the forces of good..."},
		{29.0f, "...turned out to be an exile from evil lands,"},
		{34.0f, "who has sworn revenge on his old companions."},
		{38.0f, "The first goal on his way to reckoning is death..."},
		{43.25f, "...of The Necromancer."}, 
		{45.75f, ""}
	};


	cameraFollower->Paused = true;

	defaultPhysicsMaxDelta = Physics::maxDelta;
	Physics::maxDelta = -1.0f;

	cameraAnimating = true;
	GUIManager::KeybindLock::Set(GUILockName);

	AudioManager::PlayIntro();
}

void IntroMovie::LateUpdate(float dt)
{
	// fix high first dt
	if (dt < 0.2f)
	{
		dtsum += dt;
		timerAnimator.Process(dt);
	}

	int newSubutitleIndex = subtitles.size() - 1;
	for (int i = 1; i < subtitles.size(); i++)
	{
		if (subtitles[i].time > dtsum)
		{
			newSubutitleIndex = i - 1;
			break;
		}
	}
	if (newSubutitleIndex != subtitleIndex)
	{
		subtitleIndex = newSubutitleIndex;
		subtitlesLabel->SetText(subtitles[subtitleIndex].text);
	}

	if (!cameraAnimating) return;

	int indexTo = -1;
	float t = dtsum;

	for (int i = 0; i < cameraPoints.size(); i++)
	{
		CameraPoint& p = cameraPoints[i];
		if (p.timeToThisPoint > t)
		{
			t /= p.timeToThisPoint;
			indexTo = i;
			break;
		}
		else
		{
			t -= p.timeToThisPoint;
		}
	}

	int indexFrom = indexTo - 1;
	if (indexFrom < 0)
		indexFrom = 0;

	if (indexTo == -1)
	{
		UpdateCamera(finalPoint);

		CameraMoveFinished();
	}
	else
	{
		CameraPoint& a = cameraPoints[indexFrom];
		CameraPoint& b = cameraPoints[indexTo];
		if (!a.wasReached)
		{
			if (a.OnReach)
				a.OnReach();
			a.wasReached = true;
		}
		CameraPoint point = InterpolatePoint(a, b, t);
		UpdateCamera(point);
	}

	if (InputManager::GetKeyDown(GLFW_KEY_ESCAPE))
	{
		FinishIntroAndClear();
	}
}

void IntroMovie::CameraMoveFinished()
{
	cameraAnimating = false;

	timerAnimator.UpdateInTime(1.5f, [&](float prog) {
		topBarPanel->SetSize({ 1.0f, glm::mix(screenBarSize, 0.0f, glm::sineEaseIn(prog))});
		bottomBarPanel->SetSize({ 1.0f, glm::mix(screenBarSize, 0.0f, glm::sineEaseIn(prog)) });
		});

	timerAnimator.DelayAction(1.5f, std::bind(&IntroMovie::FinishIntroAndClear, this));
}

void IntroMovie::FinishIntroAndClear()
{
	UpdateCamera(finalPoint);

	auto& scriptContainer = HFEngine::ECS.GetComponent<ScriptContainer>(playerObject);
	auto cameraFollower = scriptContainer.GetScript<PlayerCameraFollower>();
	cameraFollower->Paused = false;

	auto mapCellOptimizer = scriptContainer.GetScript<MapCellOptimizer>();
	mapCellOptimizer->EnableOptimizer();

	for (int i = 0; i < introGUILayer; i++)
		GUIManager::SetLayerEnabled(i, true);

	Physics::maxDelta = defaultPhysicsMaxDelta;

	GUIManager::KeybindLock::Release(GUILockName);
	AudioManager::StopIntro();

	DestroyGameObjectSafely();
}

IntroMovie::CameraPoint IntroMovie::InterpolatePoint(CameraPoint& a, CameraPoint& b, float t)
{
	CameraPoint point;
	point.position = glm::mix(a.position, b.position, glm::sineEaseInOut(t));
	point.target = glm::mix(a.target, b.target, glm::sineEaseInOut(t));
	point.scale = glm::mix(a.scale, b.scale, glm::sineEaseInOut(t));
	return point;
}

void IntroMovie::UpdateCamera(CameraPoint& point)
{
	HFEngine::MainCamera.SetView(point.position, point.target);
	HFEngine::MainCamera.SetScale(point.scale);
}
