#include <random>
#include <glm/glm.hpp>
#include "ECS/Components/MapLayoutComponents.h"
#include "ECS/Components/ParticleEmitter.h"
#include "ECS/Components/PointLightRenderer.h"
#include "Audio/AudioManager.h"
#include "Necromancer.h"

#define GetBossTransform() HFEngine::ECS.GetComponent<Transform>(GetGameObject())
#define GetPlayerTransform() HFEngine::ECS.GetComponent<Transform>(bossController->GetPlayerObject())

namespace {
	float RandomFloat(float min = 0.0f, float max = 1.0f)
	{
		static std::random_device rd;
		static std::mt19937 gen(rd());
		static std::uniform_real_distribution<float> dis(0.0f, 1.0f);

		return glm::mix(min, max, dis(gen));
	}
}

namespace Bosses {

	void Necromancer::Awake()
	{
		EventManager::AddScriptListener(SCRIPT_LISTENER(Events::Gameplay::Boss::INITLIAZE_SCRIPT, Necromancer::OnBossScriptInitialize));
		EventManager::AddScriptListener(SCRIPT_LISTENER(Events::Gameplay::Boss::TRIGGERED, Necromancer::OnBossTriggered));
		EventManager::AddScriptListener(SCRIPT_LISTENER(Events::Gameplay::Boss::DEAD, Necromancer::OnBossDead));
	}

	void Necromancer::Start()
	{
		enemyPrefabs = {
			PrefabManager::GetPrefab("Enemies/Axer"),
			PrefabManager::GetPrefab("Enemies/Flyer")
		};

		smokeSplashEffect = PrefabManager::GetPrefab("SmokeSplashEffect");

		for (auto& ep : enemyPrefabs)
			ep->MakeWarm();
	}

	void Necromancer::OnBossScriptInitialize(Event& ev)
	{
		auto& scriptContainer = HFEngine::ECS.GetComponent<ScriptContainer>(GetGameObject());
		bossController = scriptContainer.GetScript<BossController>();
		bossController->OnRequestToTakeDamage = std::bind(&Necromancer::OnRequestToTakeDamage, this, std::placeholders::_1);
	}

	void Necromancer::OnBossTriggered(Event& ev)
	{
		if (GetGameObject() != ev.GetParam<GameObject>(Events::GameObject::GameObject)) return;

		AudioManager::CreateDefaultSourceAndPlay(sourceNecromancerInit, "necro_boss", false, 1.0f);
		AudioManager::StopBackground();
		AudioManager::PlayBackground("bossKorpecki", 0.2f);

		ScheduleWavesSpawning();
	}

	void Necromancer::OnBossDead(Event& ev)
	{
		// TODO
		AudioManager::StopBackground();
		AudioManager::PlayBackground("gameplayKorpecki", 0.1f);


		auto magicBallEffects = HFEngine::ECS.GetByNameInChildren(GetGameObject(), "MagicBallEffect");
		for (auto go : magicBallEffects)
		{
			auto& emitter = HFEngine::ECS.GetComponent<ParticleEmitter>(go);
			auto& light = HFEngine::ECS.GetComponent<PointLightRenderer>(go);
			timerAnimator.AnimateVariable(&emitter.rate, emitter.rate, 0.0f, 0.75f);
			timerAnimator.AnimateVariable(&light.light.intensity, light.light.intensity, 0.0f, 0.75f);
		}
	}

	void Necromancer::OnRequestToTakeDamage(float value)
	{
		AudioManager::CreateDefaultSourceAndPlay(sourceNecromancerDamage, "damage1", false, 1.0f);
		bossController->TakeDamage(value);
	}

	void Necromancer::Update(float dt)
	{
		if (!bossController || !bossController->IsTriggered()) return;

		timerAnimator.Process(dt);

		if (bossController->IsDead()) return;

		glm::vec3 currentPos = GetBossTransform().GetPosition();
		Raycaster& raycaster = bossController->GetRaycaster();

		if (shouldSpawnWave)
		{
			AudioManager::CreateDefaultSourceAndPlay(sourceNecromancerInit, "necro_spawning_enemies", false, 1.0f);

			float randomRot = RandomFloat(0.0f, M_PI * 2.0f);
			glm::vec3 direction = {
				glm::sin(randomRot), 0.0f, glm::cos(randomRot)
			};

			bool clearPath = true;
			if (raycaster.Raycast(currentPos, direction))
			{
				float pathLength = waveDistance + waveEnemyDistance * enemiesInWave;
				if (raycaster.GetOut().distance < waveDistance)
					clearPath = false;
			}

			if (clearPath)
			{
				auto enemyPrefab = enemyPrefabs[HFEngine::CURRENT_FRAME_NUMBER % enemyPrefabs.size()];
				float roty = GetBossTransform().GetRotationEuler().y;
				for (int i = 0; i < enemiesInWave; i++)
				{
					SpawnEnemy(enemyPrefab, currentPos + direction * (waveDistance + waveEnemyDistance * i), roty);
				}
				shouldSpawnWave = false;
			}
		}
	}

	void Necromancer::ScheduleWavesSpawning()
	{
		if (bossController->IsDead()) return;

		for (int i = 0; i < amountOfWaves; i++)
		{
			timerAnimator.DelayAction(timeBetweenWaves * i, std::bind(&Necromancer::CastWaveSpawn, this));
		}
	}

	void Necromancer::CastWaveSpawn()
	{
		if (bossController->IsDead()) return;

		bossController->RequestAnimationAction("cast", 0.15f, 0.5f, 0.9f,
			[&]() {
				shouldSpawnWave = true;
			});
	}

	void Necromancer::SpawnEnemy(std::shared_ptr<Prefab> prefab, glm::vec3 position, float rotation)
	{
		GameObject smokeSplash = smokeSplashEffect->Instantiate(position);

		timerAnimator.DelayAction(0.5f, [&, prefab, position, rotation, smokeSplash]() {
			CellChild& cellChild = HFEngine::ECS.GetComponent<CellChild>(GetGameObject());
			GameObject enemy = prefab->Instantiate(position, { 0.0f, rotation , 0.0f });
			HFEngine::ECS.AddComponent<CellChild>(enemy, { cellChild.cell });
			spawnedEnemies.push_back(enemy);

			(*HFEngine::ECS.GetComponentInChildren<ParticleEmitter>(smokeSplash))->emitting = false;
			});

		timerAnimator.DelayAction(2.0f, [smokeSplash]() {
			HFEngine::ECS.DestroyGameObject(smokeSplash);
			});
	}

	void Necromancer::LateUpdate(float dt)
	{
		if (!bossController || !bossController->IsTriggered()) return;
		if (bossController->IsDead()) return;

		auto& bossTransform = GetBossTransform();
		auto& playerTransform = GetPlayerTransform();

		float dist = glm::distance(bossTransform.GetPosition(), playerTransform.GetPosition());
		glm::vec3 directionToPlayer = glm::normalize(playerTransform.GetPosition() - bossTransform.GetPosition());

		if (dist < keepDistance.x)
		{
			bossController->TargetMovePosition = bossTransform.GetPosition() - directionToPlayer * moveAwayDistance;
		}
		else if (dist > keepDistance.y)
		{
			bossController->TargetMovePosition = bossTransform.GetPosition() + directionToPlayer * moveAwayDistance;
		}

		bossController->PositionToLookAt = playerTransform.GetPosition();
	}

}