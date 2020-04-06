#pragma once
#include "glm/glm.hpp"

struct DiagramLayout
{
    float width = 240;
    float height = 320;

    int columns = 5;
    int rows = 7;

    int LloydRelaxIteraions = 1;
};

struct CellMeshConfig
{
    float mainScale = 0.97f;
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


// TODO: finish fence config
struct CellFenceEntity
{
    //GameObject gameObject;
    float length;
    float minForFill;
    float maxForFill;

    inline bool InFillRange(float gap)
    {
        return gap >= minForFill && gap <= maxForFill;
    }
    inline bool InFillRange(float gap, int segments)
    {
        return InFillRange(gap - (segments - 1) * length);
    }
};
struct CellFenceConfig
{
    CellFenceEntity gateEntity;
    CellFenceEntity fragmentEntity;
    CellFenceEntity connectorEntity;
    float gateDistance = 5.0f;
    int fragmentCount = 2;
    float innerLevelFenceLocation = 0.87f;
};