#include "GrassSimulatorSystem.h"
#include "HFEngine.h"
#include "ECS/Components/GrassSimulator.h"
#include "ECS/Components/GrassCircleSimulationPrimitive.h"
#include "ECS/Components/Transform.h"
#include "Rendering/PrimitiveRenderer.h"
#include "InputManager.h"
#include "Utility/Utility.h"

namespace {
	const int BINDING_LAST_RESULT = 1;
	const int BINDING_TARGET_RESULT = 2;
}

void GrassSimulatorSystem::Init()
{
	circlePrimitiveShader = ShaderManager::GetShader("GrassCirclePrimitive");

	simulatorShader = ShaderManager::GetShader("GrassSimulator");
	simulatorShader->use();
	simulatorShader->setInt("lastResult", BINDING_LAST_RESULT);
	simulatorShader->setInt("targetResult", BINDING_TARGET_RESULT);
}

void GrassSimulatorSystem::Update(float dt)
{
	static bool enabled = true;
	if (InputManager::GetKeyDown(GLFW_KEY_F9))
		enabled = !enabled;
	if (!enabled)
		return;

	circlePrimitiveShader->use();

	glDepthMask(GL_FALSE);
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE);
	glBlendEquationSeparate(GL_FUNC_ADD, GL_MAX);

	auto it = gameObjects.begin();
	while (it != gameObjects.end())
	{
		auto gameObject = *(it++);
		auto& simulator = HFEngine::ECS.GetComponent<GrassSimulator>(gameObject);

		if (simulator.collectedPrimitives.size() == 0)
			continue;

		simulator.renderTexture->bind();
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		for (auto& primitiveObject : simulator.collectedPrimitives)
		{
			auto& primitive = HFEngine::ECS.GetComponent<GrassCircleSimulationPrimitive>(primitiveObject);
			auto& transform = HFEngine::ECS.GetComponent<Transform>(primitiveObject);

			glm::vec3 position = transform.GetWorldPosition();
			glm::vec3 direction = position - primitive.lastPosition;
			direction.y = 0.0f;

			if (glm::length2(direction) > 0.00001f)
			{
				primitive.lastPosition = position;
				primitive.lastDirection = glm::normalize(glm::vec2(direction.x, direction.z));
				//primitive.lastDirection.y = 1.0f - primitive.lastDirection.y;
			}

			glm::vec4 rectBounds = glm::vec4(
				inverseLerp(position.x - primitive.radius, simulator.worldBounds.x, simulator.worldBounds.z),
				1.0f - inverseLerp(position.z - primitive.radius, simulator.worldBounds.y, simulator.worldBounds.w),
				inverseLerp(position.x + primitive.radius, simulator.worldBounds.x, simulator.worldBounds.z),
				1.0f - inverseLerp(position.z + primitive.radius, simulator.worldBounds.y, simulator.worldBounds.w)
			);

			circlePrimitiveShader->setVector4F("rectBounds", rectBounds);
			circlePrimitiveShader->setVector2F("direction", primitive.lastDirection);
			circlePrimitiveShader->setFloat("targetHeight", primitive.targetHeight);
			circlePrimitiveShader->setFloat("innerRadius", primitive.innerRadius / primitive.radius);
			circlePrimitiveShader->setFloat("ring", primitive.ring / primitive.radius);
			circlePrimitiveShader->setFloat("innerRing", primitive.innerRing / primitive.radius);
			circlePrimitiveShader->setInt("directionMode", (int)primitive.directionMode);

			PrimitiveRenderer::DrawScreenQuad();
		}
	}

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glBlendEquation(GL_FUNC_ADD);
	glDisable(GL_BLEND);
	glDisable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
}


void GrassSimulatorSystem::PostUpdate(float dt)
{
	static bool enabled = true;
	if (InputManager::GetKeyDown(GLFW_KEY_F9))
		enabled = !enabled;
	if (!enabled)
		return;

	simulatorShader->use();
	simulatorShader->setFloat("dt", dt);

	glDepthMask(GL_FALSE);
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);

	auto it = gameObjects.begin();
	while (it != gameObjects.end())
	{
		auto gameObject = *(it++);
		auto& simulator = HFEngine::ECS.GetComponent<GrassSimulator>(gameObject);

		if (simulator.collectedPrimitives.size() == 0)
		{
			simulator.inactivityTime += dt;
			if (simulator.inactivityTime > 2.0f)
				continue;
		}
		else
		{
			simulator.inactivityTime = 0.0f;
		}


		simulator.simulationTexture->bind();
		simulator.simulationTextureBack->getColorAttachement(0)->bind(BINDING_LAST_RESULT);
		simulator.renderTexture->getColorAttachement(0)->bind(BINDING_TARGET_RESULT);
		PrimitiveRenderer::DrawScreenQuad();

		FrameBuffer::BlitColor(simulator.simulationTexture, simulator.simulationTextureBack);
	}

	glDisable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
}