#include <random>
#include "ECS/Components/ParticleContainer.h"
#include "ECS/Components/ParticleEmitter.h"
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
		for (auto& particle : container.particles)
		{
			if (particle.isExpired()) continue;
			particle.position += particle.direction * particle.velocity * dt;
			particle.currentLifetime += dt;
		}
	}

	void EmitParticle(Particle* particle, ParticleEmitter& emitter, Transform& transform)
	{
		glm::vec2 emitPlane;
		switch (emitter.shape)
		{
		case ParticleEmitter::EmitterShape::CIRCLE:
			emitPlane.x = RandomFloat(-1.0f, 1.0f);
			emitPlane.y = RandomFloat(-1.0f, 1.0f);
			emitPlane = glm::normalize(emitPlane) * RandomFloat();
			break;
		case ParticleEmitter::EmitterShape::RECTANGLE:
			emitPlane.x = RandomFloat(-1.0f, 1.0f);
			emitPlane.y = RandomFloat(-1.0f, 1.0f);
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

void ParticleEmitterSystem::Update(float dt)
{
	auto it = gameObjects.begin();
	while (it != gameObjects.end())
	{
		auto gameObject = *(it++);

		auto& transform = HFEngine::ECS.GetComponent<Transform>(gameObject);
		auto& container = HFEngine::ECS.GetComponent<ParticleContainer>(gameObject);
		auto& emitter = HFEngine::ECS.GetComponent<ParticleEmitter>(gameObject);

		UpdateParticles(container, dt);
		container.lastUpdate = HFEngine::CURRENT_FRAME_NUMBER;

		if (!emitter.emitting) return;
		emitter.timeLeftSinceEmit += dt;
		float emitTimeStep = 1.0f / emitter.rate;
		int maxParticlesPerFrame = (int)(dt / emitTimeStep) + 1;
		int spawned = 0;
		while (emitter.timeLeftSinceEmit >= emitTimeStep && spawned < maxParticlesPerFrame)
		{
			Particle* particle = container.getFreeParticle();
			if (particle == NULL) break;

			EmitParticle(particle, emitter, transform);

			emitter.timeLeftSinceEmit -= emitTimeStep;
			spawned++;
		}
	}
}