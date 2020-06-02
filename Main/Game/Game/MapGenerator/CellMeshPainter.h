#pragma once

#include "Resourcing/Material.h"
#include "CellMeshGenerator.h"

class CellMeshPainter
{
private:
	CellMeshGenerator::UVData uvData;
	std::vector<glm::vec2> uvGates;

	CellTerrainConfig config;

public:
	CellMeshPainter(CellTerrainConfig& config, CellMeshGenerator::UVData& uvData)
		: config(config), uvData(uvData) {}

	void AddGate(GameObject gateObject);
	std::shared_ptr<Material> CreateMaterial();

private:
	void AssignUniforms(std::shared_ptr<Shader> shader);
};

