#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/vector_angle.hpp>
#include "../Script.h"
#include "HFEngine.h"
#include "Utility/TimerAnimator.h"
#include "GUI/GUIManager.h"
#include "GUI/Panel.h"

class TutorialEnemy : public Script
{
private: // parameters
	float moveSpeed = 5.0f;
	float maxHealth = 10.0f;
	float dmgAnimationDuration = 0.5f;
	float mainGhostDamage = 1.0f;
	float miniGhostDamage = 2.75f;

private: // variables
	GameObject visualObject;
	GameObject cellObject;

	float currentMoveSpeed = 0.0f;
	float moveSpeedSmoothing = 50.0f; // set in Start()
	float rotateSpeedSmoothing = 4.0f * M_PI;

	float health;

	glm::vec3 defaultColor;
	glm::vec3 damagedColor = { 1.0f, 0.0f, 0.0f };
	TimerAnimator timerAnimator;

	glm::vec3 originalPos;
	glm::vec3 originalLookPos;

	std::shared_ptr<Panel> healthBarPanel;
	std::shared_ptr<Panel> healthRedPanel;
	std::shared_ptr<Panel> healthValuePanel;
	glm::vec2 healthBarSize = { 60.0f, 10.0f };
	float healthBorderSize = 2.0f;

	bool hadMainGhostDamage = false;

public:
	TutorialEnemy();

	~TutorialEnemy();

	void Awake();

	void Start();

	void GhostMovementStart(Event& event);

	void Update(float dt);

	float GetRotationdifferenceToPoint(glm::vec3 point);

	void LateUpdate(float dt);

	void OnPushBackEnemies(Event& ev);

	void RestoreDefaultEmissive();

	void OnTriggerEnter(GameObject that, GameObject other);
};