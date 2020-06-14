#pragma once

#include <glm/glm.hpp>
#include "../Script.h"
#include "../Types/GhostTypes.h"
#include "ECS/Components/SkinAnimator.h"
#include "ECS/Components/SkinnedMeshRenderer.h"
#include "ECS/Components/MeshRenderer.h"
#include "ECS/Components/MapLayoutComponents.h"
#include "Event/Events.h"
#include "HFEngine.h"
#include "Utility/Utility.h"
#include "Utility/TimerAnimator.h"

class TutorialPlayground : public Script
{
	// params
public:
	int minGhostCountPerLine = 4;
	float maxGhostDistance = 0.75f;
	float maxEndpointDistance = 1.5f;

	// variables
private:
	GameObject lineTemplateObject[3];
	GameObject helperGhostObject;

	GameObject cellObject;
	GameObject cellGateObject;

	int activeLineTemplate = 0;
	bool playgroundActive = false;

	TimerAnimator timerAnimator;

	float dtsum = 0.0f;

public:
	TutorialPlayground()
	{
		RegisterIntParameter("minGhostCountPerLine", &minGhostCountPerLine);
		RegisterFloatParameter("maxGhostDistance", &maxGhostDistance);
		RegisterFloatParameter("maxEndpointDistance", &maxEndpointDistance);
	}

	void Awake()
	{
		EventManager::AddScriptListener(SCRIPT_LISTENER(Events::Gameplay::Ghost::LINE_CREATED, TutorialPlayground::OnGhostLineCreated));
		EventManager::AddScriptListener(SCRIPT_LISTENER(Events::Gameplay::Tutorial::ENEMYTOY_KILLED, TutorialPlayground::OnTutorialEnemyToyKilled));
	}

	void Start()
	{
		lineTemplateObject[0] = HFEngine::ECS.GetByNameInChildren(GetGameObject(), "TemplateLine1")[0];
		lineTemplateObject[1] = HFEngine::ECS.GetByNameInChildren(GetGameObject(), "TemplateLine2")[0];
		lineTemplateObject[2] = HFEngine::ECS.GetByNameInChildren(GetGameObject(), "TemplateLine3")[0];

		helperGhostObject = HFEngine::ECS.GetByNameInChildren(GetGameObject(), "HelperGhost")[0];
		HFEngine::ECS.GetComponent<SkinAnimator>(helperGhostObject).SetAnimation("ghostrunning");

		cellObject = HFEngine::ECS.GetComponent<CellChild>(GetGameObject()).cell;
		cellGateObject = HFEngine::ECS.GetComponent<MapCell>(cellObject).Bridges[0].Gate;
		EventManager::FireEventTo(cellGateObject, Events::Gameplay::Gate::CLOSE_ME);

		playgroundActive = true;
	}

	void Update(float dt)
	{
		timerAnimator.Process(dt);

		if (!playgroundActive) return;

		dtsum += dt;

		// animate active line template
		if (activeLineTemplate < 3)
		{
			auto& lineRenderer = HFEngine::ECS.GetComponent<MeshRenderer>(lineTemplateObject[activeLineTemplate]);
			lineRenderer.material->opacityValue = 0.2f + glm::sin(dtsum * 10.0f) * 0.1f;

			auto& lineTtransform = HFEngine::ECS.GetComponent<Transform>(lineTemplateObject[activeLineTemplate]);
			glm::vec3 linePos = lineTtransform.GetPosition();
			glm::vec3 lineScale = lineTtransform.GetScale();
			glm::vec3 lineRight = lineTtransform.GetRight();

			glm::vec3 lineStartPos = linePos - lineRight * lineScale.x * 1.0f;
			glm::vec3 lineEndPos = linePos + lineRight * lineScale.x * 1.0f;

			float lineProgress = glm::mod(dtsum, 1.25f) / 1.25f;

			auto& helperGhostTransform = HFEngine::ECS.GetComponent<Transform>(helperGhostObject);
			helperGhostTransform.SetPositionRotation(
				glm::mix(lineStartPos, lineEndPos, lineProgress),
				lineTtransform.GetRotationEuler() + glm::vec3{0.0f, 90.0f, 0.0f}
				);

			auto& helperGhostRenderer = HFEngine::ECS.GetComponent<SkinnedMeshRenderer>(helperGhostObject);
			helperGhostRenderer.material->opacityValue = glm::sin(lineProgress * M_PI) * 0.3f;
		}

	}


	void OnGhostLineCreated(Event& ev)
	{
		if (!playgroundActive || activeLineTemplate >= 3) return;

		auto gline = ev.GetParam<std::shared_ptr<GhostLine>>(Events::Gameplay::Ghost::GhostLine);

		auto& lineTtransform = HFEngine::ECS.GetComponent<Transform>(lineTemplateObject[activeLineTemplate]);
		glm::vec3 linePos = lineTtransform.GetWorldPosition();
		glm::vec3 lineScale = lineTtransform.GetWorldScale();
		glm::vec3 lineRight = lineTtransform.GetWorldRight();

		glm::vec3 lineStartPos = linePos - lineRight * lineScale.x * 1.0f;
		glm::vec3 lineEndPos = linePos + lineRight * lineScale.x * 1.0f;
		glm::vec2 lineStartPos2 = { lineStartPos.x, lineStartPos.z, };
		glm::vec2 lineEndPos2 = { lineEndPos.x, lineEndPos.z, };

		// first check if there is enough ghosts on line
		int validGhostsOnLine = 0;
		for (auto& ghost : gline->ghosts)
		{
			glm::vec3 gpos = HFEngine::ECS.GetComponent<Transform>(ghost).GetWorldPosition();
			glm::vec2 gpos2 = { gpos.x, gpos.z };
			float dist = Utility::GetDistanceBetweenPointAndSegment(
				gpos2,
				lineStartPos2,
				lineEndPos2
				);
			if (dist <= maxGhostDistance)
				validGhostsOnLine++;
		}

		if (validGhostsOnLine < minGhostCountPerLine)
		{
			ev.Cancel();
			return;
		}

		// then check if line endpoints are connected properly
		float minStartDistance = 9999.9f;
		float minEndDistance = 9999.9f;
		for (auto& point : gline->points)
		{
			minStartDistance = glm::min(minStartDistance, glm::distance(point, lineStartPos2));
			minEndDistance = glm::min(minEndDistance, glm::distance(point, lineEndPos2));
		}

		if (minStartDistance > maxEndpointDistance || minEndDistance > maxEndpointDistance)
		{
			ev.Cancel();
			return;
		}

		// now if it's second or third line - amount of crossing should be one or two
		if (gline->crosings.size() != activeLineTemplate)
		{
			ev.Cancel();
			return;
		}

		// line is proper now - we can switch to next line
		auto& lineRenderer = HFEngine::ECS.GetComponent<MeshRenderer>(lineTemplateObject[activeLineTemplate]);
		lineRenderer.material->opacityValue = 0.0f;
		activeLineTemplate++;

		if (activeLineTemplate == 3)
		{
			auto& helperGhostRenderer = HFEngine::ECS.GetComponent<SkinnedMeshRenderer>(helperGhostObject);
			helperGhostRenderer.material->opacityValue = 0.0f;
			playgroundActive = false;
		}
	}

	void OnTutorialEnemyToyKilled(Event& ev)
	{
		for (int i = 0; i < 3; i++)
		{
			auto& lineRenderer = HFEngine::ECS.GetComponent<MeshRenderer>(lineTemplateObject[i]);
			lineRenderer.material->opacityValue = 0.0f;
		}
		auto& helperGhostRenderer = HFEngine::ECS.GetComponent<SkinnedMeshRenderer>(helperGhostObject);
		helperGhostRenderer.material->opacityValue = 0.0f;

		playgroundActive = false;

		timerAnimator.DelayAction(0.5f, [&]() {
			EventManager::FireEventTo(cellGateObject, Events::Gameplay::Gate::OPEN_ME);
			DestroyGameObjectSafely();
			});
	}

	void LateUpdate(float dt)
	{
#ifdef HF_DEBUG_RENDER
		if (!playgroundActive || activeLineTemplate >= 3) return;

		auto& lineTtransform = HFEngine::ECS.GetComponent<Transform>(lineTemplateObject[activeLineTemplate]);
		glm::vec3 linePos = lineTtransform.GetWorldPosition();
		glm::vec3 lineScale = lineTtransform.GetWorldScale();
		glm::vec3 lineRight = lineTtransform.GetWorldRight();

		glm::vec3 lineStartPos = linePos - lineRight * lineScale.x * 1.0f;
		glm::vec3 lineEndPos = linePos + lineRight * lineScale.x * 1.0f;

		PrimitiveRenderer::DrawLine(lineStartPos, lineEndPos);
#endif
	}
};
