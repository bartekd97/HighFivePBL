#include <random>
#include <queue>
#include "ECS/Components/ParticleContainer.h"
#include "ECS/Components/ParticleEmitter.h"
#include "ECS/Components/ParticleRenderer.h"
#include "ECS/Components/Transform.h"
#include "HFEngine.h"
#include "ParticleEmitterSystem.h"

namespace {
	float RandomFloat(float min = 0.0f, float max = 1.0f)
	{
		static std::random_device rd;
		static std::mt19937 gen(rd());
		static std::uniform_real_distribution<float> dis(0.0f, 1.0f);
		
		return glm::mix(min, max, dis(gen));
	}

	void UpdateParticles(ParticleContainer& container, float dt)
	{
		container._activeParticles = 0;
		for (auto& particle : container.particles)
		{
			if (particle.IsExpired()) continue;
			particle.position += particle.direction * particle.velocity * dt;
			particle.currentLifetime += dt;
			container._activeParticles++;
		}
	}

	void EmitParticle(Particle* particle, ParticleEmitter& emitter, Transform& transform)
	{
		glm::vec2 emitPlane = {0.0f, 0.0f};
		switch (emitter.shape)
		{
		case ParticleEmitter::EmitterShape::CIRCLE:
			emitPlane.x = RandomFloat(-1.0f, 1.0f);
			emitPlane.y = RandomFloat(-1.0f, 1.0f);

			emitPlane = glm::normalize(emitPlane) * glm::mix(emitter.shapeInnerLevel, 1.0f, RandomFloat());
			break;
		case ParticleEmitter::EmitterShape::RECTANGLE:
			emitPlane.x = RandomFloat(-1.0f, 1.0f);
			emitPlane.y = RandomFloat(-1.0f, 1.0f);

			emitPlane.x = glm::mix(emitter.shapeInnerLevel, 1.0f, glm::abs(emitPlane.x)) * glm::sign(emitPlane.x);
			emitPlane.y = glm::mix(emitter.shapeInnerLevel, 1.0f, glm::abs(emitPlane.y)) * glm::sign(emitPlane.y);

			break;
		default:
			break;
		}

		glm::vec3 position = transform.GetWorldPosition()
			+ transform.GetWorldRight() * emitPlane.x * emitter.sourcShapeeSize.x
			+ transform.GetWorldFront() * emitPlane.y * emitter.sourcShapeeSize.y;
		glm::vec3 destination = transform.GetWorldPosition() + transform.GetWorldUp()
			+ transform.GetWorldRight() * emitPlane.x * emitter.targetShapeSize.x
			+ transform.GetWorldFront() * emitPlane.y * emitter.targetShapeSize.y;

		particle->position = position;
		particle->direction = glm::normalize(destination - position);
		particle->size = RandomFloat(emitter.size.x, emitter.size.y);
		particle->velocity = RandomFloat(emitter.velocity.x, emitter.velocity.y);
		particle->maxLifetime = RandomFloat(emitter.lifetime.x, emitter.lifetime.y);
		particle->currentLifetime = 0.0f;
	}
}

void ParticleEmitterSystem::WorkUpdateQueue(float dt)
{
	GameObject gameObject;

	while (workerUpdateQueue.try_pop(gameObject))
	{
		auto& container = HFEngine::ECS.GetComponent<ParticleContainer>(gameObject);
		auto& emitter = HFEngine::ECS.GetComponent<ParticleEmitter>(gameObject);

		if (container._activeParticles > 0)
			UpdateParticles(container, dt);

		container.lastUpdate = HFEngine::CURRENT_FRAME_NUMBER;

		if (emitter.emitting)
		{
			workerEmitQueue.push(gameObject);

			if (!doEmit) // keep busy if already emitting
				container.business.MakeFree();
		}
		else
		{
			container.business.MakeFree();
		}
	}
}

void ParticleEmitterSystem::WorkEmitQueue(float dt)
{
	GameObject gameObject;
	while (workerEmitQueue.try_pop(gameObject))
	{
		auto& transform = HFEngine::ECS.GetComponent<Transform>(gameObject);
		auto& container = HFEngine::ECS.GetComponent<ParticleContainer>(gameObject);
		auto& emitter = HFEngine::ECS.GetComponent<ParticleEmitter>(gameObject);

		emitter.timeLeftSinceEmit += dt;
		float emitTimeStep = 1.0f / emitter.rate;
		int maxParticlesPerFrame = (int)(dt / emitTimeStep) + 1;
		int spawned = 0;
		while (emitter.timeLeftSinceEmit >= emitTimeStep && spawned < maxParticlesPerFrame)
		{
			Particle* particle = container.GetFreeParticle();
			if (particle == NULL) break;

			EmitParticle(particle, emitter, transform);
			container._activeParticles++;

			emitter.timeLeftSinceEmit -= emitTimeStep;
			spawned++;
		}

		container.business.MakeFree();
	}
}


void ParticleEmitterSystem::Update(float dt)
{
	particleEmitterWorker.WaitForAll(); // make sure we start frame with free worker
	doEmit = false;

	auto it = gameObjects.begin();
	while (it != gameObjects.end())
	{
		auto gameObject = *(it++);
		auto& container = HFEngine::ECS.GetComponent<ParticleContainer>(gameObject);
		auto& renderer = HFEngine::ECS.GetComponent<ParticleRenderer>(gameObject);

		// skip particles that weren't seen on screen
		// there is 1 frame delay (cuz it checks state from previous render frame)
		// but extended AABB should do the trick
		if (renderer.cullingData.lastUpdate+1 < HFEngine::CURRENT_FRAME_NUMBER)
			continue;
		if (!renderer.cullingData.visibleByViewCamera && !renderer.cullingData.visibleByLightCamera)
			continue;

		container.business.MakeBusy();
		workerUpdateQueue.push(gameObject);
	}
	particleEmitterWorker.FillWorkers([this, dt]() {this->WorkUpdateQueue(dt);});
}

void ParticleEmitterSystem::PostUpdate(float dt)
{
	// make sure are emitters are busy and then schedule emitting
	std::queue<GameObject> awaitingEmitters;
	GameObject gameObject;
	while (workerEmitQueue.try_pop(gameObject))
		awaitingEmitters.push(gameObject);

	while (!awaitingEmitters.empty())
	{
		gameObject = awaitingEmitters.front();
		HFEngine::ECS.GetComponent<ParticleContainer>(gameObject).business.MakeBusy();
		workerEmitQueue.push(gameObject);
		awaitingEmitters.pop();
	}

	particleEmitterWorker.FillWorkers([this, dt]() {this->WorkEmitQueue(dt);});
}
