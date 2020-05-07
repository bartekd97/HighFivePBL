#include <chrono>
#include <glm/gtc/matrix_access.hpp>
#include "HFEngine.h"
#include "MeshRendererSystem.h"
#include "Resourcing/Shader.h"
#include "Resourcing/Material.h"
#include "Resourcing/Mesh.h"

#include "ECS/Components/Transform.h"
#include "ECS/Components/MeshRenderer.h"

void MeshRendererSystem::Init()
{
	toGBufferShader = ShaderManager::GetShader("ToGBuffer");
	toGBufferShader->use();
	MaterialBindingPoint::AssignToShader(toGBufferShader);
}

void MeshRendererSystem::RenderToGBuffer()
{
	//auto start = std::chrono::high_resolution_clock::now();

	toGBufferShader->use();
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	Frustum frustum = HFEngine::MainCamera.GetFrustum();
	ScheduleCulling(frustum);
	FinishCulling();

	HFEngine::MainCamera.Use(toGBufferShader);

	int visCount = 0, invisCount = 0;

	auto currentFrame = HFEngine::CURRENT_FRAME_NUMBER;
	auto renderers = HFEngine::ECS.GetAllComponents<MeshRenderer>();
	for (auto const& renderer : renderers)
	{
		//auto const& renderer = HFEngine::ECS.GetComponent<MeshRenderer>(gameObject);

		if (renderer.cullingData.lastUpdate != currentFrame)
			continue;

		
		if (!renderer.cullingData.visibleByMainCamera)
		{
			//invisCount++;
			continue;
		}
		else
		{
			//visCount++;
		}
		
		toGBufferShader->setMat4("gModel", renderer.cullingData.worldTransform);

		renderer.material->apply(toGBufferShader);
		renderer.mesh->bind();
		renderer.mesh->draw();
	}

	glDisable(GL_CULL_FACE);

	//auto elapsed = std::chrono::high_resolution_clock::now() - start;
	//long us = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
	//LogInfo("RENDERING: Vis: {}    Invis: {}    Elapsed: {} us", visCount, invisCount, us);
}
