#include "HFEngine.h"
#include "Resourcing/Shader.h"
#include "Rendering/FrameBuffer.h""
#include "Rendering/PrimitiveRenderer.h"
#include "ECS/Components/MapLayoutComponents.h"
#include "ECS/Components/GrassPatchRenderer.h"
#include "ECS/Components/GrassSimulator.h"
#include "ECS/Components/Collider.h"
#include "ECS/Components/CircleCollider.h"
#include "ECS/Components/Transform.h"
#include "Utility/Utility.h"
#include "CellPainter.h"

namespace Painter {
	std::shared_ptr<Shader> GetTerrainPaintShader() {
		return ShaderManager::GetShader("PaintCellTerrain");
	}
	std::shared_ptr<FrameBuffer> GetPainterFrameBuffer() {
		static std::shared_ptr<FrameBuffer> fb;
		if (fb != nullptr)
			return fb;

		const std::vector<FrameBuffer::ColorAttachement> components = {
			// internalFormat, dataFormat, dataType
			{GL_RGB, GL_RGB, GL_UNSIGNED_BYTE},	// albedo
			{GL_RGB, GL_RGB, GL_UNSIGNED_BYTE},	// normal
			{GL_RED, GL_RED, GL_UNSIGNED_BYTE},		// metalness
			{GL_RED, GL_RED, GL_UNSIGNED_BYTE},		// roughness
			{GL_RED, GL_RED, GL_UNSIGNED_BYTE}		// grass density
		};
		fb = FrameBuffer::Create(2048, 2048, components, FrameBuffer::DepthAttachement::DEFAULT);
		return fb;
	}
}

void CellPainter::Paint(GameObject cell)
{
	// setup
	this->cell = cell;

	auto mapCell = HFEngine::ECS.GetComponent<MapCell>(cell);
	uvData = mapCell._uvData;

	uvGates.clear();
	for (auto const& bridge : mapCell.Bridges)
	{
		AddGate(bridge.Gate);
	}

	uvCircleExcluders.clear();
	for (auto colliderObject : HFEngine::ECS.GetGameObjectsWithComponentInChildren<Collider>(cell))
	{
		auto& collider = HFEngine::ECS.GetComponent<Collider>(colliderObject);
		if (collider.type == Collider::ColliderTypes::TRIGGER && collider.shape == Collider::ColliderShapes::CIRCLE)
		{
			auto& circleCollider = HFEngine::ECS.GetComponent<CircleCollider>(colliderObject);
			auto& colliderTransform = HFEngine::ECS.GetComponent<Transform>(colliderObject);
			AddCircleExcluder(colliderTransform.GetWorldPosition(), circleCollider.radius * 1.85f);
		}
	}

	// paint
	PaintMesh();

	// create grass
	CreateGrass();
}

void CellPainter::PaintMesh()
{
	auto shader = Painter::GetTerrainPaintShader();
	auto fb = Painter::GetPainterFrameBuffer();

	shader->use();
	AssignUniforms(shader);

	fb->bind();
	PrimitiveRenderer::DrawScreenQuad();

	auto material = MaterialManager::CreateEmptyMaterial(MaterialType::DEFERRED);
	material->albedoMap = fb->popColorAttachement(0);
	material->normalMap = fb->popColorAttachement(1);
	material->metalnessMap = fb->popColorAttachement(2);
	material->roughnessMap = fb->popColorAttachement(3);
	material->albedoColor = glm::vec3(1.0f);
	material->metalnessValue = 1.0f;
	material->roughnessValue = 1.0f;

	grassDensityTexture = fb->popColorAttachement(4);

	FrameBuffer::BindDefaultScreen();

	outputMaterial = material;
}

void CellPainter::CreateGrass()
{
	const float PATCH_SIZE = 10.0f;
	const float GRASS_SCALE = 1.0f;
	const int DENSITY = 14;
	const int TEXTURE_SIZE = 512;

	auto mapCell = HFEngine::ECS.GetComponent<MapCell>(cell);
	GameObject patchContainer = HFEngine::ECS.CreateGameObject(cell, "GrassPatches");

	glm::vec3 cellPosition = HFEngine::ECS.GetComponent<Transform>(cell).GetWorldPosition();
	glm::vec4 bounds = mapCell.PolygonSmooth.GetBounds();
	glm::vec4 worldBounds = glm::vec4(
		bounds.x + cellPosition.x,
		bounds.y + cellPosition.z,
		bounds.z + cellPosition.x,
		bounds.w + cellPosition.z
	);
	glm::vec2 size = glm::vec2(bounds.z - bounds.x, bounds.w - bounds.y);
	int pX = glm::ceil(size.x / PATCH_SIZE);
	int pY = glm::ceil(size.y / PATCH_SIZE);

	/*
	std::vector<float> texturePixels(TEXTURE_SIZE * TEXTURE_SIZE * 4);
	for (int i = 0; i < texturePixels.size(); i++) texturePixels[i] = 0.0f;
	TextureConfig textureConfig;
	textureConfig.generateMipmaps = false;
	textureConfig.format = GL_RGB16F;
	std::shared_ptr<Texture> simulationTexture = TextureManager::CreateTextureFromRawData(texturePixels.data(), TEXTURE_SIZE, TEXTURE_SIZE, GL_RGBA, textureConfig);
	*/

	const std::vector<FrameBuffer::ColorAttachement> renderComponents = {
		// internalFormat, dataFormat, dataType
		{GL_RGBA16F, GL_RGBA, GL_FLOAT}
	};
	std::shared_ptr<FrameBuffer> renderTexture = FrameBuffer::Create(TEXTURE_SIZE, TEXTURE_SIZE, renderComponents, FrameBuffer::DepthAttachement::DEFAULT);
	std::shared_ptr<FrameBuffer> simulationTexture = FrameBuffer::Create(TEXTURE_SIZE, TEXTURE_SIZE, renderComponents, FrameBuffer::DepthAttachement::DEFAULT);
	std::shared_ptr<FrameBuffer> simulationTextureBack = FrameBuffer::Create(TEXTURE_SIZE, TEXTURE_SIZE, renderComponents, FrameBuffer::DepthAttachement::DEFAULT);

	for (int i = 0; i < pX; i++)
	{
		for (int j = 0; j < pY; j++)
		{
			GameObject patch = HFEngine::ECS.CreateGameObject(patchContainer, "Patch");
			HFEngine::ECS.GetComponent<Transform>(patch).SetPosition(
				glm::vec3(
					bounds.x + (PATCH_SIZE * (float)i) + (PATCH_SIZE * 0.5f),
					0.0f,
					bounds.y + (PATCH_SIZE * (float)j) + (PATCH_SIZE * 0.5f)
				)
			);

			GrassPatchRenderer renderer;
			renderer.worldBounds = worldBounds;
			renderer.patchSize = PATCH_SIZE;
			renderer.grassScale = GRASS_SCALE;
			renderer.density = DENSITY;
			renderer.densityTexture = grassDensityTexture;
			renderer.simulationTexture = simulationTexture;
			HFEngine::ECS.AddComponent<GrassPatchRenderer>(patch, renderer);
		}
	}

	GrassSimulator simulator;
	simulator.worldBounds = worldBounds;
	simulator.simulationTexture = simulationTexture;
	simulator.simulationTextureBack = simulationTextureBack;
	simulator.renderTexture = renderTexture;
	HFEngine::ECS.AddComponent<GrassSimulator>(cell, simulator);
}


void CellPainter::AddGate(GameObject gateObject)
{
	glm::vec3 pos3d = HFEngine::ECS.GetComponent<Transform>(gateObject).GetPosition();
	glm::vec2 pos = { pos3d.x, pos3d.z };
	pos -= uvData.offset;
	pos /= uvData.range;
	pos.y = 1.0f - pos.y;
	uvGates.push_back(pos);
}

void CellPainter::AddCircleExcluder(glm::vec3 worldPosition, float radius)
{
	auto mapCell = HFEngine::ECS.GetComponent<MapCell>(cell);
	glm::vec3 cellPosition = HFEngine::ECS.GetComponent<Transform>(cell).GetWorldPosition();
	glm::vec4 bounds = mapCell.PolygonSmooth.GetBounds();
	glm::vec4 worldBounds = glm::vec4(
		bounds.x + cellPosition.x,
		bounds.y + cellPosition.z,
		bounds.z + cellPosition.x,
		bounds.w + cellPosition.z
	);

	uvCircleExcluders.push_back(glm::vec4(
		inverseLerp(worldPosition.x, worldBounds.x, worldBounds.z),
		1.0f - inverseLerp(worldPosition.z, worldBounds.y, worldBounds.w),
		radius / (worldBounds.z - worldBounds.x),
		radius / (worldBounds.w - worldBounds.y)
	));
}

void CellPainter::AssignUniforms(std::shared_ptr<Shader> shader)
{
	auto mapCell = HFEngine::ECS.GetComponent<MapCell>(cell);

	shader->setInt("GatesCount", uvGates.size());
	for (int i=0; i<uvGates.size(); i++)
		shader->setVector2F(("Gates[" + std::to_string(i) + "]").c_str(), uvGates[i]);

	shader->setInt("CircleExcludersCount", uvCircleExcluders.size());
	for (int i=0; i< uvCircleExcluders.size(); i++)
		shader->setVector4F(("CircleExcluders[" + std::to_string(i) + "]").c_str(), uvCircleExcluders[i]);

	shader->setInt("PolygonPointsCount", uvData.uvPolygon.Points.size());
	for (int i = 0; i < uvData.uvPolygon.Points.size(); i++)
		shader->setVector2F(("PolygonPoints[" + std::to_string(i) + "]").c_str(), uvData.uvPolygon.Points[i]);

	shader->setVector2F("CellCenter", uvData.uvCenter);

	config.grassTexture->bind(1);
	config.roadAlbedoTexture->bind(2);
	config.roadNormalTexture->bind(3);
	config.roadRoughnessTexture->bind(4);
	config.cliffAlbedoTexture->bind(5);
	config.cliffNormalTexture->bind(6);
	config.cliffRoughnessTexture->bind(7);

	shader->setInt("grassTexture", 1);
	shader->setInt("roadAlbedoTexture", 2);
	shader->setInt("roadNormalTexture", 3);
	shader->setInt("roadRoughnessTexture", 4);
	shader->setInt("cliffAlbedoTexture", 5);
	shader->setInt("cliffNormalTexture", 6);
	shader->setInt("cliffRoughnessTexture", 7);

	shader->setFloat("grassTiling", config.grassTiling);
	shader->setFloat("roadTiling", config.roadTiling);
	shader->setFloat("cliffTiling", config.cliffTiling);

	switch (mapCell.CellType)
	{
	case MapCell::Type::STARTUP:
		shader->setFloat("gRoadWidth", 0.024f);
		shader->setFloat("gRoadCenterRadius", 0.12f);
		shader->setFloat("gCliffLevelGrass", 0.89f);
		break;
	case MapCell::Type::NORMAL:
		shader->setFloat("gRoadWidth", 0.015f);
		shader->setFloat("gRoadCenterRadius", 0.08f);
		shader->setFloat("gCliffLevelGrass", 0.89f);
		break;
	case MapCell::Type::BOSS:
		shader->setFloat("gRoadWidth", 0.015f);
		shader->setFloat("gRoadCenterRadius", 0.08f);
		shader->setFloat("gCliffLevelGrass", 0.84f);
		break;
	default:
		break;
	}
}
