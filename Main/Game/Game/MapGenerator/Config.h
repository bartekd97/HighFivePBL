#pragma once
#include <glm/glm.hpp>
#include "Resourcing/Prefab.h"
#include "Resourcing/Texture.h"
#include "Utility/PropertyReader.h"

struct DiagramLayout
{
    float width = 300;
    float height = 400;

    int columns = 5;
    int rows = 7;

    int LloydRelaxIteraions = 1;
};

struct CellMeshConfig
{
    float mainScale = 0.94f;
    float depth = 5.0f;
    float depthMax = 10.0f;
    float innerScale = 0.92f;
    int outlineSplits = 6;
    float nextOutlineScale = 0.999f;
    float nextOutlineScaleMultiplier = 0.97f;
    float vertexBevel = 0.1f;
    int vertexBevelSteps = 3;
    int circularSegments = 32;
    float noiseCenterRatio = 0.9f;
    float noiseScale = 0.1f;
    glm::vec2 noiseForce = { 5, 5 };
};


struct CellFenceEntity
{
    std::shared_ptr<Prefab> prefab;
    float length;
    float minForFill;
    float maxForFill;

    CellFenceEntity()
    {
        prefab = PrefabManager::GENERIC_PREFAB;
    }
    inline void SetPrefab(std::string name)
    {
        prefab = PrefabManager::GetPrefab(name);
        prefab->Properties().GetFloat("length", length, 1.0f);
        prefab->Properties().GetFloat("minForFill", minForFill, 0.0f);
        prefab->Properties().GetFloat("maxForFill", maxForFill, length);
    }

    inline bool InFillRange(float gap)
    {
        return gap >= minForFill && gap <= maxForFill;
    }
    inline bool InFillRange(float gap, int segments)
    {
        return InFillRange(gap - ((float)((segments - 1)) * length));
    }
};
struct CellFenceConfig
{
    CellFenceEntity gateEntity;
    CellFenceEntity fragmentEntity;
    CellFenceEntity connectorEntity;
    float gateDistance = 6.5f;
    int fragmentCount = 2;
    float innerLevelFenceLocation = 0.87f;

    CellFenceConfig() 
    {
        // TODO: make it with cleaner way, with possibility to use different configs for different cells
        gateEntity.SetPrefab("Fences/PrototypeSet/Gate");
        fragmentEntity.SetPrefab("Fences/PrototypeSet/HighFence");
        connectorEntity.SetPrefab("Fences/PrototypeSet/ConnectColumn");
    }
};

struct CellTerrainConfig
{
    std::shared_ptr<Texture> grassTexture;
    float grassTiling = 1.0f;
    std::shared_ptr<Texture> roadTexture;
    float roadTiling = 4.0f;
    std::shared_ptr<Texture> cliffTexture;
    float cliffTiling = 1.0f;

    CellTerrainConfig()
    {
        // TODO: make it with cleaner way, with possibility to use different configs for different cells
        grassTexture = TextureManager::GetTexture("MapGenerator/Terrain", "Grass");
        roadTexture = TextureManager::GetTexture("MapGenerator/Terrain", "Road");
        cliffTexture = TextureManager::GetTexture("MapGenerator/Terrain", "Cliff");
    }
};

struct MapGeneratorConig
{
    DiagramLayout layout;
    CellMeshConfig cellMeshConfig;
    CellFenceConfig cellFenceConfig;
    CellTerrainConfig cellTerrainConfig;

    std::shared_ptr<Prefab> bridgePrefab;
    float minEdgeLengthForBridge = 20.0f;

    MapGeneratorConig()
    {
        // TODO: make it with cleaner way, with possibility to use different configs for different cells
        bridgePrefab = PrefabManager::GetPrefab("Bridges/Bridge1");
    }
};