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

	struct Zone {
		std::vector<glm::vec2> points;
		glm::vec2 center;
	};
	struct ZoneRoad {
		glm::vec2 gatePosition;
		float angle;
	};

private:
	CellStructuresConfig structuresConfig;
public:
	const GameObject cell;
	const Type type;


private:
	std::vector<Zone> zones;

public:
	CellSetuper(CellStructuresConfig& structuresConfig, GameObject cell, Type type)
		: structuresConfig(structuresConfig), cell(cell), type(type) {}

	void Setup();

private:
	void MakeZones();
};

