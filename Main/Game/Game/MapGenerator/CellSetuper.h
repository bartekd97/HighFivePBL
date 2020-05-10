#pragma once

#include "ECS/ECSTypes.h"
#include "Config.h"

class CellSetuper
{
private:
	CellStructuresConfig structuresConfig;

public:
	CellSetuper(CellStructuresConfig& structuresConfig)
		: structuresConfig(structuresConfig) {}

	void Setup(GameObject cell);
};

