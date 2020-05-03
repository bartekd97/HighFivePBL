#pragma once

#include <vector>
#include <cppDelaunay/delaunay/Voronoi.h>
#include "Config.h"
#include "ECS/ECSTypes.h"
#include "ECS/Components/MapLayoutComponents.h"

class MapGenerator
{
public:
    MapGeneratorConig config;

private:
    Delaunay::Rectangle bounds;
    std::vector<Delaunay::Edge*> edges;

public:
    MapGenerator() : bounds(Delaunay::Rectangle(0,0,0,0)) {}

    void Generate();

private:
    bool IsBorderCell(Delaunay::Site* cell);
    GameObject CreateCell(Delaunay::Site* cell, GameObject parent);
    void CreateBridges(std::vector<GameObject> cells, GameObject parent);
};

