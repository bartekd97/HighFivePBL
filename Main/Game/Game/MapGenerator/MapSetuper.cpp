#include "MapSetuper.h"
#include "ECS/Components/MapLayoutComponents.h"
#include "ECS/Components/Transform.h"
#include "HFEngine.h"

#define MAKE_SETUPER(cell, type) std::make_shared<CellSetuper>(config.cellStructuresConfig, cell, type);

void MapSetuper::Setup()
{
    GameObject startupCell = GetStartupCell();
    GameObject bossCell = CalculateBossCell(startupCell);

    cellSetupers[startupCell] = MAKE_SETUPER(startupCell,   CellSetuper::Type::STARTUP);
    cellSetupers[bossCell] =    MAKE_SETUPER(bossCell,      CellSetuper::Type::BOSS);

    for (auto cell : cells)
    {
        if (!cellSetupers.contains(cell))
        {
            cellSetupers[cell] = MAKE_SETUPER(cell,         CellSetuper::Type::NORMAL);
        }
    }


    for (auto& cs : cellSetupers)
    {
        cs.second->Setup();
    }
}


GameObject MapSetuper::GetStartupCell()
{
    for (auto cell : cells)
    {
        MapCell mc = HFEngine::ECS.GetComponent<MapCell>(cell);
        if (mc.Bridges.size() == 1)
        {
            return cell;
        }
    }
    assert(false && "No one-bridge startup cell");
    return cells[0]; // fallback
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
