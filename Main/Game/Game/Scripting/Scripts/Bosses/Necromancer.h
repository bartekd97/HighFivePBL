#pragma once

#include "../../Script.h"
#include "..\BossController.h"
#include "Event/Events.h"
#include "HFEngine.h"
#include "Utility/TimerAnimator.h"
#include "Resourcing/Prefab.h"

namespace Bosses {
	class Necromancer : public ::Script
	{
		struct Stage
		{
			float hpPercentage;
			int instantWavesNumber;
			float randomWaveInterval;
			int maxWaves;
			int enemiesInWave;
		};

	private:
		glm::vec2 keepDistance = { 4.5f, 9.0 };
		float moveAwayDistance = 3.0f;

		float waveDistance = 6.0f;
		float waveEnemyDistance = 1.5f;
		float timeBetweenWaves = 7.5f;
		int enemiesInWave = 3;
		int amountOfWaves = 3;

	private:
		std::shared_ptr<BossController> bossController;

		std::vector<std::shared_ptr<Prefab>> enemyPrefabs;
		bool shouldSpawnWave = false;
		bool isCasting = false;
		std::vector<GameObject> spawnedEnemies;
		std::vector<Stage> stages;
		int currentStage = 0;
		std::chrono::steady_clock::time_point lastSpawnTime;

		std::shared_ptr<Prefab> smokeSplashEffect;

		TimerAnimator timerAnimator;
		ALuint sourceNecromancerInit;
		ALuint sourceNecromancerDamage;
		ALuint sourceNecromancerSpawner;


		int GetCurrentWaveNumber();
		void ClearSpawnedEnemies();

	public:
		void Awake();

		void Start();

		void OnBossScriptInitialize(Event& ev);
		void OnBossTriggered(Event& ev);
		void OnBossDead(Event& ev);
		void OnRequestToTakeDamage(float value);

		void Update(float dt);

		void TrySpawnWave();
		void CastWaveSpawn();
		void SpawnEnemy(std::shared_ptr<Prefab> prefab, glm::vec3 position, float rotation);

		void LateUpdate(float dt);
	};
}