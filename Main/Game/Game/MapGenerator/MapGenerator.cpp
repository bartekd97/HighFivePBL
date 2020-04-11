#include <random>
#include <memory>
#include <set>
#include <cppDelaunay/delaunay/Edge.h>
#include <glm/glm.hpp>
#include "MapGenerator.h"
#include "CellGenerator.h"
#include "ext/LloydRelaxation.h"
#include "HFEngine.h"
#include "ECS/Components/Transform.h"
#include "ECS/Helpers/TransformHelper.h"

namespace {
	std::vector<Delaunay::Point*> GeneratePoints(DiagramLayout layout)
	{
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<> dis(0.0,1.0);

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
	bounds = GetBounds(layout);
	auto points = GeneratePoints(layout);

	auto voronoi = DelaunayExt::VoronoiWithLloydRelaxation(points, bounds, layout.LloydRelaxIteraions);
    edges = voronoi->edges();

    std::set<Delaunay::Site*> sites;
    for (auto edge : edges)
    {
        sites.insert(edge->rightSite());
        sites.insert(edge->leftSite());
    }

    GameObject MapObject = HFEngine::ECS.CreateGameObject("Map");

    // prepare cells
    GameObject cellContainer = HFEngine::ECS.CreateGameObject(MapObject, "Cells");
    std::vector<GameObject> cells;
    for (auto s : sites)
        if (!IsBorderCell(s))
            cells.push_back(CreateCell(s, cellContainer));

    // create bridges and connect references
    GameObject bridgeContainer = HFEngine::ECS.CreateGameObject(MapObject, "Bridges");
    CreateBridges(cells, bridgeContainer);

    TransformHelper::UpdateFromRoot(MapObject);

    // now generate real cells
    for (auto s : sites)
    {
        if (!IsBorderCell(s))
        {
            GameObject cell = *std::find_if(cells.begin(), cells.end(), [s](GameObject go) {
                return HFEngine::ECS.GetComponent<MapCell>(go).CellSiteIndex == s->index();
                });
            ConvexPolygon cellPolygon = CreateLocalPolygon(s, bounds);
            CellGenerator generator(cellMeshConfig, cellFenceConfig);
            generator.Generate(cellPolygon, cell);
        }
    }

}



bool MapGenerator::IsBorderCell(Delaunay::Site* cell)
{
    for (auto p : cell->region(bounds))
        if (p->x == 0 || p->x == layout.width || p->y == 0 || p->y == layout.height)
            return true;
    return false;
}

GameObject MapGenerator::CreateCell(Delaunay::Site* cell, GameObject parent)
{
    GameObject cellObject = HFEngine::ECS.CreateGameObject(parent, std::to_string(cell->index()));
    HFEngine::ECS.GetComponent<Transform>(cellObject).SetPosition({cell->coord()->x, 0.0f, cell->coord()->y});

    MapCell mCell;
    mCell.CellSiteIndex = cell->index();
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

        if (Delaunay::Point::distance(leftVertex, rightVertex) < minEdgeLengthForBridge)
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

        /*
        GameObject bridgeObject = Instantiate(bridgePrefab, parent.transform);
        bridgeObject.transform.position = position;
        bridgeObject.transform.right = (new Vector3(rightVertex.x, 0.0f, rightVertex.y) - position).normalized;
        */
        GameObject bridgeObject = HFEngine::ECS.CreateGameObject(parent);
        float rotation = rad2deg(glm::atan(rightVertex->x - position.x, rightVertex->y - position.z)) + 90; // + 90 for right
        HFEngine::ECS.GetComponent<Transform>(bridgeObject).SetPosition(position);
        HFEngine::ECS.GetComponent<Transform>(bridgeObject).SetRotation({ 0.0f, rotation, 0.0f });

        CellBridge bridge;
        bridge.CellA = cellA;
        bridge.CellB = cellB;
        HFEngine::ECS.AddComponent<CellBridge>(bridgeObject, bridge);

        HFEngine::ECS.GetComponent<MapCell>(cellA).Bridges.push_back({ bridgeObject, cellB, (GameObject)0 });
        HFEngine::ECS.GetComponent<MapCell>(cellB).Bridges.push_back({ bridgeObject, cellA, (GameObject)0 });
    }
}