#pragma once

#include <vector>
#include <unordered_set>
#include <memory>
#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>
#include "ECS/ECSTypes.h"

struct GhostCrossing;
struct GhostLine;

struct GhostCrossing
{
    glm::vec2 position;
    std::shared_ptr<GhostLine> a;
    std::shared_ptr<GhostLine> b;

    /*
    inline bool Equals(const std::shared_ptr<GhostCrossing>& other)
    {
        return glm::distance2(position, other->position) < 0.001f;
    }
    */
    /*
    struct Equaliter
    {
        inline bool operator()(const std::shared_ptr<GhostCrossing>& lhs, const std::shared_ptr<GhostCrossing>& rhs) const
        {
            return lhs->Equals(rhs);
        }
    };
    */
};

struct GhostLine
{
    std::vector<glm::vec2> points;
    std::unordered_set<
        std::shared_ptr<GhostCrossing>//,
        //std::hash<std::shared_ptr<GhostCrossing>>,
        //GhostCrossing::Equaliter

    > crosings;
    std::vector<GameObject> ghosts;
};