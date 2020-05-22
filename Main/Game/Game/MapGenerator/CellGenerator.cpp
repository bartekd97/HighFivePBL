#include "CellGenerator.h"
#include "CellMeshGenerator.h"
#include "CellFenceGenerator.h"
#include "CellMeshPainter.h"
#include "ECS/Components/MapLayoutComponents.h"
#include "ECS/Components/MeshRenderer.h"
#include "HFEngine.h"

void CellGenerator::Generate(ConvexPolygon& originalPolygon, GameObject cell)
{
    this->originalPolygon = originalPolygon.ShellScaledBy(meshConfig.mainScale);

    GenerateMesh(cell);
    GenerateFence(cell);
    PaintMesh(cell);
}

void CellGenerator::GenerateMesh(GameObject cell)
{
    CellMeshGenerator generator(meshConfig, originalPolygon);

    generator.PrepareOutlines();
    generator.PrepareBaseMeshStructure();

    generator.CalculateNormals();
    uvData = generator.GenerateUV();
    generator.CalculateTangents();

    std::shared_ptr<Mesh> mesh = generator.BuildMesh();


    MeshRenderer renderer;
    renderer.mesh = mesh;
    renderer.material = MaterialManager::BLANK_MATERIAL;
    HFEngine::ECS.AddComponent<MeshRenderer>(cell, renderer);

    // assign polygon structures
    HFEngine::ECS.GetComponent<MapCell>(cell).PolygonBase = generator.PolygonBase;
    HFEngine::ECS.GetComponent<MapCell>(cell).PolygonBaseInner = generator.PolygonBaseInner;
    HFEngine::ECS.GetComponent<MapCell>(cell).PolygonSmooth = generator.PolygonSmooth;
    HFEngine::ECS.GetComponent<MapCell>(cell).PolygonSmoothInner = generator.PolygonSmoothInner;
}

void CellGenerator::GenerateFence(GameObject cell)
{
    auto fencePolygon = HFEngine::ECS.GetComponent<MapCell>(cell).PolygonSmoothInner
        .ShellScaledBy(fenceConfig.innerLevelFenceLocation);
    CellFenceGenerator generator(fenceConfig, fencePolygon);

    GameObject gateContainer = HFEngine::ECS.CreateGameObject(cell, "Gates");
    MapCell& mapCell = HFEngine::ECS.GetComponent<MapCell>(cell);

    gateObjects.clear();
    for (int i=0; i<mapCell.Bridges.size(); i++)
    {
        GameObject gateObject = generator.CreateGate(mapCell.Bridges[i].Bridge, gateContainer);
        gateObjects.push_back(gateObject);

        CellGate gate;
        gate.Cell = cell;
        gate.Bridge = mapCell.Bridges[i].Bridge;
        HFEngine::ECS.AddComponent<CellGate>(gateObject, gate);

        mapCell.Bridges[i].Gate = gateObject;
        CellBridge& bridge = HFEngine::ECS.GetComponent<CellBridge>(mapCell.Bridges[i].Bridge);

        if (cell == bridge.CellA)
            bridge.GateA = gateObject;
        else if (cell == bridge.CellB)
            bridge.GateB = gateObject;
    }

    GameObject fenceContainer = HFEngine::ECS.CreateGameObject(cell, "Fence");

    generator.PrepareBrokenCurves();
    generator.CreateSections();
    generator.TryFillGapsInSections();
    generator.MakeHoles();
    generator.PrepareObjects();
    generator.BuildSections(fenceContainer);
}

void CellGenerator::PaintMesh(GameObject cell)
{
    CellMeshPainter painter(terrainConfig, uvData);
    for (auto go : gateObjects)
        painter.AddGate(go);

    HFEngine::ECS.GetComponent<MeshRenderer>(cell).material = painter.CreateMaterial();
}