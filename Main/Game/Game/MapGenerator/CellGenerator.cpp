#include "CellGenerator.h"
#include "CellMeshGenerator.h"
#include "CellFenceGenerator.h"
#include "ECS/Components/MapLayoutComponents.h"
#include "ECS/Components/MeshRenderer.h"
#include "HFEngine.h"

void CellGenerator::Generate(ConvexPolygon& originalPolygon, GameObject cell)
{
    this->originalPolygon = originalPolygon.ShellScaledBy(meshConfig.mainScale);

    GenerateMesh(cell);
    GenerateFence(cell);
}

void CellGenerator::GenerateMesh(GameObject cell)
{
    CellMeshGenerator generator(meshConfig, originalPolygon);

    generator.PrepareOutlines();
    generator.PrepareBaseMeshStructure();

    generator.CalculateNormals();
    generator.GenerateUV();
    generator.CalculateTangents();

    std::shared_ptr<Mesh> mesh = generator.BuildMesh();


    MeshRenderer renderer;
    renderer.mesh = mesh;
    renderer.material = MaterialManager::CreateEmptyMaterial();
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

    for (int i=0; i<mapCell.Bridges.size(); i++)
    {
        GameObject gateObject = generator.CreateGate(mapCell.Bridges[i].Bridge, gateContainer);

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
