#pragma once

#include <vector>
#include <glm/glm.hpp>
#include "Config.h"
#include "ConvexPolygon.h"
#include "ECS/Components/Transform.h"

class CellFenceGenerator
{
private:
    class FenceObject
    {
    public:
        glm::vec2 position;
        glm::vec2 direction;
        CellFenceEntity* entity;
        FenceObject(glm::vec2 position, glm::vec2 direction, CellFenceEntity* entity)
            : position(position), direction(direction), entity(entity)
        {}
    };

    class Gate
    {
    public:
        glm::vec2 position;
        glm::vec2 direction;
    };
    std::vector<std::shared_ptr<Gate>> gates;

    class Segment
    {
    public:
        glm::vec2 position;
        glm::vec2 direction;
        int fragmentCount;
        float fragmentLength;

        inline glm::vec2 endPosition() {
            return position + direction * fragmentLength * (float)fragmentCount;
        }
    };
    class BrokenCurve
    {
    public:
        std::vector<glm::vec2> points;
        std::vector<glm::vec2> reversedPoints;

        std::vector<std::shared_ptr<Segment>> forawrdSegments;
        std::vector<std::shared_ptr<Segment>> backwardSegments;

        int targetPointI = 1;
        int targetReversedPointI = 1;

        std::vector<std::shared_ptr<FenceObject>> objects;
        std::vector<std::shared_ptr<FenceObject>> filler;

        inline float HoleDistance()
        {
            glm::vec2 from = points[0];
            if (forawrdSegments.size() > 0)
                from = forawrdSegments[forawrdSegments.size() - 1]->endPosition();

            glm::vec2 to = reversedPoints[0];
            if (backwardSegments.size() > 0)
                to = backwardSegments[backwardSegments.size() - 1]->endPosition();

            return glm::distance(from, to);
        }
    };
    std::vector<std::shared_ptr<BrokenCurve>> brokenCurves;

    CellFenceConfig config;
    ConvexPolygon polygon;

public:
    CellFenceGenerator(CellFenceConfig& config, ConvexPolygon& polygon)
        : config(config), polygon(polygon)
    {}

    GameObject CreateGate(GameObject bridge, GameObject parent, Transform& parentTransform);
    void PrepareBrokenCurves();
    void CreateSections();
    void TryFillGapsInSections();
    void MakeHoles();
    void PrepareObjects();

    void BuildSections(GameObject parent);

private:
    std::vector<std::shared_ptr<CellFenceGenerator::FenceObject>>
        TryFillGap(
            std::shared_ptr<CellFenceGenerator::Segment> forward,
            std::shared_ptr<CellFenceGenerator::Segment> backward
            );
    void PrepareObjectsForSection(std::vector<std::shared_ptr<Segment>>& segments, std::vector<std::shared_ptr<FenceObject>>& target, bool reversed);
};

