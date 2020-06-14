#pragma once

#include <vector>
#include "MapGenerator/ConvexPolygon.h"
#include "ECS/ECSTypes.h"

class PathfindingGrid;
class MapCell
{
public:
    enum class Type {
        STARTUP,
        NORMAL,
        BOSS
    };
    struct BridgeTo
    {
        GameObject Bridge = NULL_GAMEOBJECT;
        GameObject Cell = NULL_GAMEOBJECT;
        GameObject Gate = NULL_GAMEOBJECT;
    };

    Type CellType;
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

    // don't use this, its only for generation purpose
    ConvexPolygon _BaseDelaunayPolygon;
};


struct CellGate
{
    GameObject Bridge = NULL_GAMEOBJECT;
    GameObject Cell = NULL_GAMEOBJECT;
};


struct CellBridge
{
    GameObject CellA = NULL_GAMEOBJECT;
    GameObject GateA = NULL_GAMEOBJECT;

    GameObject CellB = NULL_GAMEOBJECT;
    GameObject GateB = NULL_GAMEOBJECT;
};


struct CellChild
{
    GameObject cell = NULL_GAMEOBJECT;
};