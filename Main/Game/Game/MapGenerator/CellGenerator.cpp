#include "CellGenerator.h"
#include "CellMeshGenerator.h"
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
    // TODO
}
