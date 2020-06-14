#include "MapSetuper.h"
#include "ECS/Components/MapLayoutComponents.h"
#include "ECS/Components/Transform.h"
#include "HFEngine.h"
#include "CellGenerator.h"

#define MAKE_SETUPER(cell, type) HFEngine::ECS.GetComponent<MapCell>(cell).CellType = type; cellSetupers[cell] = std::make_shared<CellSetuper>(config.cellStructuresConfig, cell);

void MapSetuper::Setup()
{
    GameObject startupCell = GetStartupCell();
    GameObject bossCell = CalculateBossCell(startupCell);

    MAKE_SETUPER(startupCell,   MapCell::Type::STARTUP);
    MAKE_SETUPER(bossCell,      MapCell::Type::BOSS);

    for (auto cell : cells)
    {
        if (!cellSetupers.contains(cell))
        {
            MAKE_SETUPER(cell,  MapCell::Type::NORMAL);
        }
    }

    if (_debugLiteMode)
    {
        GameObject secondCell = HFEngine::ECS.GetComponent<MapCell>(startupCell).Bridges[0].Cell;

        GenerateCell(startupCell);
        GenerateCell(secondCell);

        cellSetupers[startupCell]->_debugLiteMode = true;
        cellSetupers[secondCell]->_debugLiteMode = true;
        cellSetupers[startupCell]->Setup();
        cellSetupers[secondCell]->Setup();

        // fix enemy container null gameobject
        for (auto cell : cells)
        {
            MapCell& mc = HFEngine::ECS.GetComponent<MapCell>(cell);
            if (mc.EnemyContainer == NULL_GAMEOBJECT)
                mc.EnemyContainer = HFEngine::ECS.CreateGameObject("Enemies");
        }

        return;
    }

    // first, generate cells
    for (auto cell : cells)
    {
        GenerateCell(cell);
    }

    // then setup
    for (auto& cs : cellSetupers)
    {
        cs.second->Setup();
    }
}


GameObject MapSetuper::GetStartupCell()
{
    for (auto cell : cells)
    {
        MapCell& mc = HFEngine::ECS.GetComponent<MapCell>(cell);
        if (mc.Bridges.size() == 1)
        {
            return cell;
        }
    }
    assert(false && "No one-bridge startup cell");
    return cells[0]; // fallback
}

void MapSetuper::GenerateCell(GameObject cell)
{
    auto cellType = HFEngine::ECS.GetComponent<MapCell>(cell).CellType;
    ConvexPolygon& cellPolygon = HFEngine::ECS.GetComponent<MapCell>(cell)._BaseDelaunayPolygon;
    CellGenerator generator(
        config.cellMeshConfig,
        cellType == MapCell::Type::BOSS ? config.cellBossFenceConfig : config.cellRegularFenceConfig,
        config.cellTerrainConfig
    );
    generator.Generate(cellPolygon, cell);
}

GameObject MapSetuper::CalculateBossCell(GameObject startupCell)
{
    using CD = std::pair<GameObject, float>; // cell and it's distance do startup cell
    std::vector<CD> cds;
    glm::vec3 startupPos = HFEngine::ECS.GetComponent<Transform>(startupCell).GetWorldPosition();
    glm::vec3 cellPos;
    for (auto cell : cells)
    {
        cellPos = HFEngine::ECS.GetComponent<Transform>(cell).GetWorldPosition();
        cds.push_back({cell, glm::distance2(startupPos, cellPos)});
    }
    std::sort(cds.begin(), cds.end(), [](CD a, CD b) {
        return a.second > b.second;
    });
    return cds[0].first; // boss cell is the farest cell from startup cell
}
