#pragma once

#include "ECS/ECSTypes.h"
#include "Config.h"
#include "ConvexPolygon.h"

class CellGenerator
{
private:
	CellMeshConfig meshConfig;
	CellFenceConfig fenceConfig;

	ConvexPolygon originalPolygon;
public:
	CellGenerator(CellMeshConfig& meshConfig, CellFenceConfig& fenceConfig)
		: meshConfig(meshConfig), fenceConfig(fenceConfig) {}

	void Generate(ConvexPolygon& originalPolygon, GameObject cell);

private:
	void GenerateMesh(GameObject cell);
	void GenerateFence(GameObject cell);
};

