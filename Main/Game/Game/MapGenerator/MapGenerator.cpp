#include <random>
#include <memory>
#include <set>
#include <cppDelaunay/delaunay/Edge.h>
#include <glm/glm.hpp>
#include "MapGenerator.h"
#include "CellGenerator.h"
#include "CellSetuper.h"
#include "ext/LloydRelaxation.h"
#include "HFEngine.h"
#include "ECS/Components/Transform.h"

namespace {
	std::vector<Delaunay::Point*> GeneratePoints(DiagramLayout layout)
	{
        static std::random_device rd;
        static std::mt19937 gen(rd());
        static std::uniform_real_distribution<> dis(0.0,1.0);

		std::vector<Delaunay::Point*> points;
        float wPart = layout.width / (float)layout.columns;
        float hPart = layout.height / (float)layout.rows;

        for (int x = 0; x < layout.columns; x++)
        {
            for (int y = 0; y < layout.rows; y++)
            {
                points.push_back(
                    Delaunay::Point::create(
                        glm::mix(wPart * (float)x, wPart * (float)(x + 1), (float)dis(gen)),
                        glm::mix(hPart * (float)y, hPart * (float)(y + 1), (float)dis(gen))
                        )
                    );
            }
        }
        return points;
	}
	Delaunay::Rectangle GetBounds(DiagramLayout layout)
	{
		return Delaunay::Rectangle(0, 0, layout.width, layout.height);
	}
    ConvexPolygon CreateLocalPolygon(Delaunay::Site* cell, Delaunay::Rectangle bounds)
    {
        auto center = cell->coord();
        std::vector<glm::vec2> points;
        for (auto p : cell->region(bounds))
            points.push_back({ p->x - center->x, p->y - center->y });
        return ConvexPolygon(points);
    }
}

void MapGenerator::Generate()
{
    assert(!_generated && "Map already generated");

    bounds = GetBounds(config.layout);
    std::unique_ptr<Delaunay::Voronoi> voronoi;
    std::set<Delaunay::Site*> sites;
    std::vector<GameObject> cells;
    GameObject MapObject;
    GameObject cellContainer;
    GameObject bridgeContainer;

    while (true)
    {
        auto points = GeneratePoints(config.layout);

        voronoi = DelaunayExt::VoronoiWithLloydRelaxation(points, bounds, config.layout.LloydRelaxIteraions);
        edges = voronoi->edges();

        for (auto edge : edges)
        {
            sites.insert(edge->rightSite());
            sites.insert(edge->leftSite());
        }

        MapObject = HFEngine::ECS.CreateGameObject("Map");

        // prepare cells
        cellContainer = HFEngine::ECS.CreateGameObject(MapObject, "Cells");
        for (auto s : sites)
            if (!IsBorderCell(s))
                cells.push_back(CreateCell(s, cellContainer));

        // create bridges and connect references
        bridgeContainer = HFEngine::ECS.CreateGameObject(MapObject, "Bridges");
        CreateBridges(cells, bridgeContainer);

        if (ValidateMapLayout(cells))
        {
            break; // all is fine, continue...
        }
        else
        {
            // somethins is wrong with map layout
            // clear it and start process again
            sites.clear();
            cells.clear();
            HFEngine::ECS.DestroyGameObject(MapObject); // map root object, it will recusively destroy everything below
            voronoi.reset();
        }
    }

    // now generate real cells
    // MOVED TO MapSetuper::Setup


    // and setup those cells
    mapSetuper = std::make_unique<MapSetuper>(config, cells);
    mapSetuper->_debugLiteMode = _debugLiteMode;
    mapSetuper->Setup();

    _generated = true;
}

GameObject MapGenerator::GetStartupCell()
{
    assert(_generated && "Map not generated yet");
    return mapSetuper->GetStartupCell();
}



bool MapGenerator::IsBorderCell(Delaunay::Site* cell)
{
    for (auto p : cell->region(bounds))
        if (p->x == 0 || p->x == config.layout.width || p->y == 0 || p->y == config.layout.height)
            return true;
    return false;
}

GameObject MapGenerator::CreateCell(Delaunay::Site* cell, GameObject parent)
{
    GameObject cellObject = HFEngine::ECS.CreateGameObject(parent, std::to_string(cell->index()));
    HFEngine::ECS.GetComponent<Transform>(cellObject).SetPosition({cell->coord()->x, 0.0f, cell->coord()->y});

    MapCell mCell;
    mCell.CellSiteIndex = cell->index();
    mCell._BaseDelaunayPolygon = CreateLocalPolygon(cell, bounds);
    HFEngine::ECS.AddComponent<MapCell>(cellObject, mCell);
    return cellObject;
}

void MapGenerator::CreateBridges(std::vector<GameObject> cells, GameObject parent)
{
    auto GetCell = [cells](Delaunay::Site* s) {
        for (GameObject cell : cells)
            if (HFEngine::ECS.GetComponent<MapCell>(cell).CellSiteIndex == s->index())
                return cell;
        return (GameObject)0;
    };

    for (auto edge : edges)
    {
        if (!edge->visible())
            continue;

        Delaunay::Point* leftVertex = edge->clippedEnds()[Delaunay::LR::LEFT];
        Delaunay::Point* rightVertex = edge->clippedEnds()[Delaunay::LR::RIGHT];

        if (Delaunay::Point::distance(leftVertex, rightVertex) < config.minEdgeLengthForBridge)
            continue;

        GameObject cellA = GetCell(edge->leftSite());
        GameObject cellB = GetCell(edge->rightSite());
        if (cellA == (GameObject)0 || cellB == (GameObject)0)
            continue;

        glm::vec3 position = glm::vec3(
            (leftVertex->x + rightVertex->x) * 0.5f,
            0.0f,
            (leftVertex->y + rightVertex->y) * 0.5f
            );

        float rotation = rad2deg(glm::atan(rightVertex->x - position.x, rightVertex->y - position.z)) + 90; // + 90 for right
        GameObject bridgeObject = config.bridgePrefab->Instantiate(parent, position, { 0.0f, rotation, 0.0f });

        CellBridge bridge;
        bridge.CellA = cellA;
        bridge.CellB = cellB;
        HFEngine::ECS.AddComponent<CellBridge>(bridgeObject, bridge);

        HFEngine::ECS.GetComponent<MapCell>(cellA).Bridges.push_back({ bridgeObject, cellB, (GameObject)0 });
        HFEngine::ECS.GetComponent<MapCell>(cellB).Bridges.push_back({ bridgeObject, cellA, (GameObject)0 });
    }
}


bool MapGenerator::ValidateMapLayout(std::vector<GameObject> cells)
{
    int cellsWithOneBridgeCount = 0;
    for (auto cell : cells)
    {
        MapCell mc = HFEngine::ECS.GetComponent<MapCell>(cell);
        if (mc.Bridges.size() == 0)
        {
            return false; // invalid cell with no bridges, throw away whole map...
        }
        else if (mc.Bridges.size() == 1)
        {
            cellsWithOneBridgeCount++;
        }
    }

    // there must be at least one cell with only one bridge
    if (cellsWithOneBridgeCount >= 1)
        return true;
    else
        return false;
}
