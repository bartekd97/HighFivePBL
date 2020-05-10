#include "HFEngine.h"
#include "Resourcing/Shader.h"
#include "Rendering/FrameBuffer.h""
#include "Rendering/PrimitiveRenderer.h"
#include "CellMeshPainter.h"

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
			{GL_RED, GL_RED, GL_UNSIGNED_BYTE},		// metalness
			{GL_RED, GL_RED, GL_UNSIGNED_BYTE}		// roughness
		};
		fb = FrameBuffer::Create(1024, 1024, components, FrameBuffer::DepthAttachement::DEFAULT);
		return fb;
	}
}


void CellMeshPainter::AddGate(GameObject gateObject)
{
	glm::vec3 pos3d = HFEngine::ECS.GetComponent<Transform>(gateObject).GetPosition();
	glm::vec2 pos = { pos3d.x, pos3d.z };
	pos -= uvData.offset;
	pos /= uvData.range;
	pos.y = 1.0f - pos.y;
	uvGates.push_back(pos);
}

std::shared_ptr<Material> CellMeshPainter::CreateMaterial()
{
	auto shader = Painter::GetTerrainPaintShader();
	auto fb = Painter::GetPainterFrameBuffer();

	shader->use();
	AssignUniforms(shader);

	fb->bind();
	PrimitiveRenderer::DrawScreenQuad();

	auto material = MaterialManager::CreateEmptyMaterial();
	material->albedoMap = fb->popColorAttachement(0);
	material->metalnessMap = fb->popColorAttachement(1);
	material->roughnessMap = fb->popColorAttachement(2);
	material->albedoColor = glm::vec3(1.0f);
	material->metalnessValue = 1.0f;
	material->roughnessValue = 1.0f;

	FrameBuffer::BindDefaultScreen();

	return material;
}

void CellMeshPainter::AssignUniforms(std::shared_ptr<Shader> shader)
{
	shader->setInt("GatesCount", uvGates.size());
	for (int i=0; i<uvGates.size(); i++)
		shader->setVector2F(("Gates[" + std::to_string(i) + "]").c_str(), uvGates[i]);

	shader->setInt("PolygonPointsCount", uvData.uvPolygon.Points.size());
	for (int i = 0; i < uvData.uvPolygon.Points.size(); i++)
		shader->setVector2F(("PolygonPoints[" + std::to_string(i) + "]").c_str(), uvData.uvPolygon.Points[i]);

	shader->setVector2F("CellCenter", uvData.uvCenter);

	config.grassTexture->bind(1);
	config.roadTexture->bind(2);
	config.cliffTexture->bind(3);
	shader->setInt("grassTexture", 1);
	shader->setInt("roadTexture", 2);
	shader->setInt("cliffTexture", 3);
	shader->setFloat("grassTiling", config.grassTiling);
	shader->setFloat("roadTiling", config.roadTiling);
	shader->setFloat("cliffTiling", config.cliffTiling);
}
