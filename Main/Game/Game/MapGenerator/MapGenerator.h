#pragma once

#include <vector>
#include <cppDelaunay/delaunay/Voronoi.h>
#include "Config.h"
#include "MapSetuper.h"
#include "ECS/ECSTypes.h"
#include "ECS/Components/MapLayoutComponents.h"

class MapGenerator
{
public:
    MapGeneratorConfig config;

private:
    Delaunay::Rectangle bounds;
    std::vector<Delaunay::Edge*> edges;
    std::unique_ptr<MapSetuper> mapSetuper;

    bool _generated = false;
public:
    MapGenerator() : bounds(Delaunay::Rectangle(0,0,0,0)) {}

    void Generate();

    GameObject GetStartupCell();

    bool _debugLiteMode = false;
private:
    bool IsBorderCell(Delaunay::Site* cell);
    GameObject CreateCell(Delaunay::Site* cell, GameObject parent);
    void CreateBridges(std::vector<GameObject> cells, GameObject parent);
    bool ValidateMapLayout(std::vector<GameObject> cells);
};

