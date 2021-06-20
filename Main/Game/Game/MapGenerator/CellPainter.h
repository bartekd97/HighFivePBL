#pragma once

#include "Resourcing/Material.h"
#include "CellMeshGenerator.h"

class CellPainter
{
public:
	std::shared_ptr<Material> outputMaterial;

private:
	CellMeshGenerator::UVData uvData;
	std::vector<glm::vec2> uvGates;
	std::vector<glm::vec4> uvCircleExcluders;
	std::shared_ptr<Texture> grassDensityTexture;

	GameObject cell;

	CellTerrainConfig config;

public:
	CellPainter(CellTerrainConfig& config)
		: config(config) {}

	void Paint(GameObject cell);

private:
	void PaintMesh();
	void CreateGrass();

	void AddGate(GameObject gateObject);
	void AddCircleExcluder(glm::vec3 worldPosition, float radius);
	void AssignUniforms(std::shared_ptr<Shader> shader);
};

