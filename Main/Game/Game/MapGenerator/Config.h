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
    // configured in MapGeneratorConfig

    CellFenceEntity gateEntity;
    CellFenceEntity fragmentEntity;
    CellFenceEntity connectorEntity;
    float gateDistance = 9.0f;
    int fragmentCount = 2;
    float innerLevelFenceLocation = 0.87f;
    bool forceTryFillGaps = false;

    CellFenceEntity invisHoleFence;

    CellFenceConfig()
    {
        invisHoleFence.SetPrefab("Fences/InvisHoleFence");
    }
};


struct CellFenceFireConfig
{
    std::shared_ptr<Prefab> ragnarosFire;
    std::shared_ptr<Prefab> necromancerFire;

    float fireFenceOffset = 1.5f;

    CellFenceFireConfig()
    {
        ragnarosFire = PrefabManager::GetPrefab("Fences/Fire/Ragnaros");
        necromancerFire = PrefabManager::GetPrefab("Fences/Fire/Necromancer");
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



struct CellTutorialConfig
{
    std::shared_ptr<Prefab> WASD;
    std::shared_ptr<Prefab> SpaceKey;
    std::shared_ptr<Prefab> LMBKey;
    std::shared_ptr<Prefab> GhostPlayground;
    std::shared_ptr<Prefab> EnemyToy;

    CellTutorialConfig()
    {
        WASD = PrefabManager::GetPrefab("Tutorial/WASD");
        SpaceKey = PrefabManager::GetPrefab("Tutorial/SpaceKey");
        LMBKey = PrefabManager::GetPrefab("Tutorial/LMBKey");
        GhostPlayground = PrefabManager::GetPrefab("Tutorial/GhostPlayground");
        EnemyToy = PrefabManager::GetPrefab("Tutorial/EnemyToy");
    }
};


struct CellSetupConfig
{
    CellFenceFireConfig cellFenceFireConfig;
    CellTutorialConfig cellTutorialConfig;

    int gridSize = 60;
    int gridStep = 2;
    glm::vec2 gridInnerLevel = { 0.20f, 0.75f }; // min, max
    float gridInnerRadius = 6.0f;
    float gridMinRoadDistance = 4.0f;

    float enemiesCountFactor = 2.2f; //2.2f; // how many enemies per 100 points in zone grid

    std::shared_ptr<Prefab> mainStatuePrefab;
    std::vector<std::shared_ptr<Prefab>> obstaclePrefabs;
    std::vector<std::shared_ptr<Prefab>> smallPrefabs;
    std::vector<std::shared_ptr<Prefab>> mediumPrefabs;
    std::vector<std::shared_ptr<Prefab>> largePrefabs;


    std::vector<std::shared_ptr<Prefab>> enemyPrefabs;


    CellSetupConfig()
    {
        // TODO: make it with cleaner way, with possibility to use different configs for different cells
        mainStatuePrefab = PrefabManager::GetPrefab("Statues/Goth");

        obstaclePrefabs.push_back(PrefabManager::GetPrefab("Obstacles/Mud1"));
        obstaclePrefabs.push_back(PrefabManager::GetPrefab("Obstacles/Mud2"));
        obstaclePrefabs.push_back(PrefabManager::GetPrefab("Obstacles/Mud3"));
        obstaclePrefabs.push_back(PrefabManager::GetPrefab("Obstacles/Mud4"));
        obstaclePrefabs.push_back(PrefabManager::GetPrefab("Obstacles/Mud5"));
        obstaclePrefabs.push_back(PrefabManager::GetPrefab("Obstacles/Fire1"));
        obstaclePrefabs.push_back(PrefabManager::GetPrefab("Obstacles/Fire2"));
        obstaclePrefabs.push_back(PrefabManager::GetPrefab("Obstacles/Fire3"));
        obstaclePrefabs.push_back(PrefabManager::GetPrefab("Obstacles/Fire4"));
        obstaclePrefabs.push_back(PrefabManager::GetPrefab("Obstacles/Fire5"));
        obstaclePrefabs.push_back(PrefabManager::GetPrefab("Obstacles/Gas1"));
        obstaclePrefabs.push_back(PrefabManager::GetPrefab("Obstacles/Gas2"));
        obstaclePrefabs.push_back(PrefabManager::GetPrefab("Obstacles/Gas3"));
        obstaclePrefabs.push_back(PrefabManager::GetPrefab("Obstacles/Gas4"));
        obstaclePrefabs.push_back(PrefabManager::GetPrefab("Obstacles/Gas5"));

        smallPrefabs.push_back(PrefabManager::GetPrefab("Structures/pomnik1"));
        smallPrefabs.push_back(PrefabManager::GetPrefab("Structures/pomnik2"));
        smallPrefabs.push_back(PrefabManager::GetPrefab("Structures/pomnik3"));
        smallPrefabs.push_back(PrefabManager::GetPrefab("Structures/pomnik4"));
        smallPrefabs.push_back(PrefabManager::GetPrefab("Structures/tombstone_celtic1"));
        smallPrefabs.push_back(PrefabManager::GetPrefab("Structures/tombstone_celtic2"));
        smallPrefabs.push_back(PrefabManager::GetPrefab("Structures/littleFenceFirst"));
        largePrefabs.push_back(PrefabManager::GetPrefab("Structures/chapel"));
        smallPrefabs.push_back(PrefabManager::GetPrefab("Structures/STR1"));
        mediumPrefabs.push_back(PrefabManager::GetPrefab("Structures/STR2"));
        largePrefabs.push_back(PrefabManager::GetPrefab("Structures/STR3"));
        mediumPrefabs.push_back(PrefabManager::GetPrefab("Structures/STR6"));
        mediumPrefabs.push_back(PrefabManager::GetPrefab("Structures/STR7"));
        mediumPrefabs.push_back(PrefabManager::GetPrefab("Structures/STR8"));
        mediumPrefabs.push_back(PrefabManager::GetPrefab("Structures/STR10"));
        smallPrefabs.push_back(PrefabManager::GetPrefab("Structures/STR11"));
        smallPrefabs.push_back(PrefabManager::GetPrefab("Structures/STR12"));
        mediumPrefabs.push_back(PrefabManager::GetPrefab("Structures/STR13"));
        smallPrefabs.push_back(PrefabManager::GetPrefab("Structures/STR14"));
        smallPrefabs.push_back(PrefabManager::GetPrefab("Structures/STR15"));


        enemyPrefabs.push_back(PrefabManager::GetPrefab("Enemies/Axer"));
        enemyPrefabs.push_back(PrefabManager::GetPrefab("Enemies/Flyer"));
    }
};




struct MapGeneratorConfig
{
    DiagramLayout layout;
    CellMeshConfig cellMeshConfig;

    CellFenceConfig cellRegularFenceConfig;
    CellFenceConfig cellBossFenceConfig;

    CellTerrainConfig cellTerrainConfig;

    CellSetupConfig cellStructuresConfig;

    std::shared_ptr<Prefab> bridgePrefab;
    float minEdgeLengthForBridge = 24.0f;

    MapGeneratorConfig()
    {
        // TODO: make it with cleaner way, with possibility to use different configs for different cells
        bridgePrefab = PrefabManager::GetPrefab("Bridges/Bridge1");

        // setup regular fence
        cellRegularFenceConfig.gateEntity.SetPrefab("Fences/PrototypeSet/Gate");
        cellRegularFenceConfig.fragmentEntity.SetPrefab("Fences/PrototypeSet/HighFence");
        cellRegularFenceConfig.connectorEntity.SetPrefab("Fences/PrototypeSet/ConnectColumn");

        // setup boss fence
        cellBossFenceConfig.fragmentCount = 1;
        cellBossFenceConfig.forceTryFillGaps = true;
        cellBossFenceConfig.gateEntity.SetPrefab("Fences/SecondSet/Gate");
        cellBossFenceConfig.fragmentEntity.SetPrefab("Fences/SecondSet/HighFence");
        cellBossFenceConfig.connectorEntity.SetPrefab("Fences/SecondSet/ConnectColumn");
    }
};