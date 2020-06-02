#pragma once

#include "ECS/ECSTypes.h"
#include "Config.h"
#include "ConvexPolygon.h"
#include "CellMeshGenerator.h"

class CellGenerator
{
private:
	CellMeshConfig meshConfig;
	CellFenceConfig fenceConfig;
	CellTerrainConfig terrainConfig;

	ConvexPolygon originalPolygon;
	CellMeshGenerator::UVData uvData;
	std::vector<GameObject> gateObjects;
public:
	CellGenerator(CellMeshConfig& meshConfig, CellFenceConfig& fenceConfig, CellTerrainConfig& terrainConfig)
		: meshConfig(meshConfig), fenceConfig(fenceConfig), terrainConfig(terrainConfig) {}

	void Generate(ConvexPolygon& originalPolygon, GameObject cell);

private:
	void GenerateMesh(GameObject cell);
	void GenerateFence(GameObject cell);
	void PaintMesh(GameObject cell);
};

