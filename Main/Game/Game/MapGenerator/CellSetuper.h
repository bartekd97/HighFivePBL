#pragma once

#include "ECS/ECSTypes.h"
#include "Config.h"

class CellSetuper
{
public:
	enum class Type {
		STARTUP,
		NORMAL,
		BOSS
	};

private:
	CellStructuresConfig structuresConfig;
public:
	const GameObject cell;
	const Type type;

public:
	CellSetuper(CellStructuresConfig& structuresConfig, GameObject cell, Type type)
		: structuresConfig(structuresConfig), cell(cell), type(type) {}

	void Setup();
};

