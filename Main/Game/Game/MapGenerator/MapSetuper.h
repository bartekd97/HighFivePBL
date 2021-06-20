#pragma once

#include <vector>
#include <map>
#include <memory>
#include "CellSetuper.h"
#include "ECS/ECSTypes.h"
#include "Config.h"

class MapSetuper
{
private:
	MapGeneratorConfig config;
	std::vector<GameObject> cells;
	std::map<GameObject, std::shared_ptr<CellSetuper>> cellSetupers;

public:
	MapSetuper(MapGeneratorConfig& config, std::vector<GameObject>& cells)
		: config(config), cells(cells) {}

	void Setup();

	GameObject GetStartupCell();

	bool _debugLiteMode = false;
private:
	void GenerateCell(GameObject cell);
	void PaintCell(GameObject cell);

	GameObject CalculateBossCell(GameObject startupCell);
};

