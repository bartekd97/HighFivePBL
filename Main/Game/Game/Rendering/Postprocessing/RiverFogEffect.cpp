#include <stb_perlin.h>
#include "RiverFogEffect.h"
#include "Resourcing/Mesh.h"
#include "Resourcing/Shader.h"
#include "HFEngine.h"
#include "Event/Events.h"
#include "Event/EventManager.h"

void RiverFogEffect::LateUpdateNoise(Event& event)
{
	float dt = event.GetParam<float>(Events::General::DELTA_TIME);

	noiseOffsets[0] += noiseSpeeds[0] * dt;
	noiseOffsets[1] += noiseSpeeds[1] * dt;
	noiseOffsets[2] += noiseSpeeds[2] * dt;
}

void RiverFogEffect::Init()
{
	std::vector<Vertex> planeVertices = {
		{{-1.0f, 0.0f, -1.0f}, {0.0f, 0.0f}},
		{{1.0f, 0.0f, -1.0f}, {1.0f, 0.0f}},
		{{1.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
		{{-1.0f, 0.0f, 1.0f}, {0.0f, 1.0f}}
	};
	std::vector<unsigned> planeIndices = {
		0, 1, 2,
		0, 2, 3
	};
	AABBStruct planeAABB = { {-1.0f, 0.0f, -1.0f}, {1.0f, 0.0f, 1.0f} };

	fogPlane = ModelManager::CreateMesh(planeVertices, planeIndices, planeAABB);


	std::vector<unsigned char> noisePixels;
	noisePixels.reserve(512 * 512 * 3);
	float fx, fy;
	float n1, n2, n3;
	int x, y;
	for (x = 0, fx = 0.0f; x < 512; x++, fx += (1.0f / 512.0f))
	{
		for (y = 0, fy = 0.0f; y < 512; y++, fy += (1.0f / 512.0f))
		{
			n1 = (stb_perlin_noise3(fx * 4.0f, fy * 4.0f, 0.0f, 4, 4, 0) + 1.0f) * 127.0f;
			n2 = (stb_perlin_noise3(fx * 8.0f, fy * 8.0f, 0.0f, 8, 8, 0) + 1.0f) * 127.0f;
			n3 = (stb_perlin_noise3(fx * 16.0f, fy * 16.0f, 0.0f, 16, 16, 0) + 1.0f) * 127.0f;
			noisePixels.push_back(static_cast<unsigned char>(n1));
			noisePixels.push_back(static_cast<unsigned char>(n2));
			noisePixels.push_back(static_cast<unsigned char>(n3));
		}
	}
	TextureConfig nConfig;
	noise = TextureManager::CreateTextureFromRawData(noisePixels.data(), 512, 512, GL_RGB, nConfig);


	shader = ShaderManager::GetShader("PPRiverFog");
	shader->use();
	shader->setInt("depthMap", 0);
	shader->setInt("noiseMap", 1);

	EventManager::AddListener(METHOD_LISTENER(Events::General::POST_UPDATE, RiverFogEffect::LateUpdateNoise));
}

bool RiverFogEffect::PreForwardProcess(
	std::shared_ptr<FrameBuffer> source,
	std::shared_ptr<FrameBuffer> destination,
	RenderPipeline::GBufferStruct& gbuffer)
{
	Camera& viewCamera = HFEngine::MainCamera;

	glm::vec3 mainCameraPosition = viewCamera.GetPosition();
	glm::vec3 mainCameraDirection = viewCamera.GetViewDiorection();

	float stepsToFog = -((mainCameraPosition.y - FogHeightLevel) / mainCameraDirection.y);
	glm::vec3 fogPos = mainCameraPosition + (mainCameraDirection * stepsToFog);
	glm::vec2 camSize = viewCamera.GetSize() * 0.55f;
	float viewSize = glm::max(camSize.x, camSize.y) * viewCamera.GetScale();
	glm::mat4 model = glm::translate(glm::mat4(1.0), fogPos) * glm::scale(glm::mat4(1.0), glm::vec3(viewSize));

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	shader->use();
	shader->setMat4("gModel", model);
	shader->setMat4("gView", viewCamera.GetViewMatrix());
	shader->setMat4("gProjection", viewCamera.GetProjectionMatrix());
	shader->setVector2F("viewportSize", {source->width, source->height});
	shader->setVector2F("noiseOffsets[0]", noiseOffsets[0]);
	shader->setVector2F("noiseOffsets[1]", noiseOffsets[1]);
	shader->setVector2F("noiseOffsets[2]", noiseOffsets[2]);
	gbuffer.depth->bind(0);
	noise->bind(1);
	fogPlane->bind();
	fogPlane->draw();
	glDisable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);

	return false;
}
