#pragma once

#include <vector>
#include "MapGenerator/ConvexPolygon.h"
#include "ECS/ECSTypes.h"

class PathfindingGrid;
class MapCell
{
public:
    struct BridgeTo
    {
        GameObject Bridge;
        GameObject Cell;
        GameObject Gate;
    };

    std::vector<BridgeTo> Bridges;
    GameObject EnemyContainer = NULL_GAMEOBJECT;

    std::shared_ptr<PathfindingGrid> PathFindingGrid;
    //std::vector<GameObject> Enemies;
    //std::vector<GameObject> Monuments;

    int CellSiteIndex = 0;


    // probably those polygon wont be needed, maybe one or two
    // it needs testing
    // some visualisation of them would be cool
    // (maybe with onDrawGizmo in unity scene?)
    //
    // Those polygons are in local space! (with 0,0 at center, not real cell position in x,y)
    ConvexPolygon PolygonBase;
    ConvexPolygon PolygonBaseInner;
    ConvexPolygon PolygonSmooth;
    ConvexPolygon PolygonSmoothInner;
};


struct CellGate
{
    GameObject Bridge;
    GameObject Cell;
};


struct CellBridge
{
    GameObject CellA;
    GameObject GateA;

    GameObject CellB;
    GameObject GateB;
};


struct CellChild
{
    GameObject cell;
};