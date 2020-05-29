#include <stb_perlin.h>

#include "GravitySystem.h"
#include "HFEngine.h"
#include "../../Physics/Physics.h"

extern GameObjectHierarchy gameObjectHierarchy;

const float gravityAcceleration = 9.81f * 5.0f;
const float minFallingDist = 0.01f;

void GravitySystem::Update(float dt)
{
    float level;
    if (cells.size() == 0) LoadCells();

    for (auto const& gameObject : gameObjects)
    {
        auto& rigidBody = HFEngine::ECS.GetComponent<RigidBody>(gameObject);
        auto& transform = HFEngine::ECS.GetComponent<Transform>(gameObject);

        rigidBody.acceleration.y = gravityAcceleration;

        rigidBody.velocity.y += dt * rigidBody.acceleration.y;
        auto pos = transform.GetWorldPosition();
        auto posYSub = dt * (rigidBody.velocity.y + dt * rigidBody.acceleration.y / 2.0f);

        int closestCell = GetClosestCellIndex(pos);
        /*if (closestCell == -1)
        {
            LogWarning("GravitySystem::Update(): closest cell not found for {}", gameObject);
            continue;
        }*/
        bool shouldFall = true;
        if (closestCell != -1) // TODO: remove it and find out why some objects sometimes have WTF positions causing this
        {
            GameObject bridge = GetBridge(gameObject, closestCell);
            if (bridge != NULL_GAMEOBJECT)
            {
                shouldFall = false;
                level = GetBridgeLevel(pos, bridge);
            }
            else
            {
                glm::vec2 posTemp = glm::vec2(pos.x - cells[closestCell].first.x, pos.z - cells[closestCell].first.z);
                if (HFEngine::ECS.SearchComponent<Collider>(gameObject))
                {
                    auto& collider = HFEngine::ECS.GetComponent<Collider>(gameObject);
                    if (collider.shape == Collider::ColliderShapes::CIRCLE)
                    {
                        auto& circleCollider = HFEngine::ECS.GetComponent<CircleCollider>(gameObject);
                        auto subPos = pos - cells[closestCell].first;
                        if (VECLEN(subPos) > 0.1f)
                        {
                            glm::vec3 normal = glm::normalize(subPos);
                            posTemp.x -= normal.x * circleCollider.radius;
                            posTemp.y -= normal.z * circleCollider.radius;
                        }
                    }
                }
                shouldFall = GetCellYLevel(posTemp, cells[closestCell].second, level);
            }
            
            if (!shouldFall)
            {
                if ((pos.y - posYSub) <= level || (!rigidBody.isFalling && posYSub < minFallingDist))
                {
                    rigidBody.isFalling = false;
                    rigidBody.velocity.y = 0;
                    pos.y = level;
                    transform.SetPosition(pos);
                    continue;
                }
            }
            else
            {
                rigidBody.isFalling = true;
            }
            transform.TranslateSelf(glm::vec3(0.0f, -posYSub, 0.0f));
        }
        
    }
}

bool GravitySystem::GetCellYLevel(glm::vec2& position, MapCell& cell, float& level)
{
    float coRatio = cell.PolygonSmooth.GetEdgeCenterRatio(position);
    if (fabs(coRatio - 1.0f) < std::numeric_limits<float>::epsilon()) return true;

    float innerLevel = glm::clamp((1.0f - coRatio) / (1.0f - config.innerScale), 0.0f, 1.0f);
    float depthLevel = (1.0f - glm::sin(innerLevel * M_PI * 0.5f));

    float noisex = 0.0f, noisey = 0.0f, noisez = 0.0f;
    float noiseLevel = 1.0f - glm::clamp((1.0f - coRatio) / (1.0f - config.noiseCenterRatio), 0.0f, 1.0f);
    if (noiseLevel > 0.0f)
    {
        float pn = stb_perlin_noise3(
            position.x * config.noiseScale,
            position.y * config.noiseScale,
            0.0f,
            0, 0, 0
        );
        pn = pn * noiseLevel * innerLevel;

        glm::vec2 noiseDir = glm::normalize(position);

        noisey = pn * config.noiseForce.y;
    }

    level = noisey - config.depth * depthLevel;

    return false;
}

void GravitySystem::SetCollector(std::shared_ptr<MapCellCollectorSystem> mapCellCollectorSystem)
{
    this->mapCellCollectorSystem = mapCellCollectorSystem;
}

void GravitySystem::LoadCells()
{
    cells.clear();
    for (auto& gameObject : mapCellCollectorSystem->gameObjects)
    {
        auto& transform = HFEngine::ECS.GetComponent<Transform>(gameObject);
        auto& mapCell = HFEngine::ECS.GetComponent<MapCell>(gameObject);

        cells.push_back(std::pair(transform.GetWorldPosition(), mapCell));
    }
}

int GravitySystem::GetClosestCellIndex(glm::vec3& position)
{
    float dist, minDist = std::numeric_limits<float>::max();
    float x, z;
    int cellIndex = -1;
    for (int i = 0; i < cells.size(); i++)
    {
        x = cells[i].first.x - position.x;
        z = cells[i].first.z - position.z;
        dist = sqrt((x * x) + (z * z));
        if (dist < minDist)
        {
            minDist = dist;
            cellIndex = i;
        }
    }
    return cellIndex;
}

GameObject GravitySystem::GetBridge(GameObject gameObject, int closestCell)
{
    for (auto bridge : cells[closestCell].second.Bridges)
    {
        if (Physics::cacheNodes[gameObject].HasTrigger(bridge.Bridge))
        {
            return bridge.Bridge;
        }
    }

    return NULL_GAMEOBJECT;
}

float GravitySystem::GetBridgeLevel(glm::vec3& position, GameObject bridge)
{
    int minIndex = -1, maxIndex = -1;
    auto& bridgeTransform = HFEngine::ECS.GetComponent<Transform>(bridge);
    auto gravityCollider = HFEngine::ECS.GetComponentInChildren<GravityCollider>(bridge).value();
    if (gravityCollider.heights.size() == 0)
    {
        LogWarning("GravitySystem::GetBridgeLevel(): empty gravity collider {}", bridge);
        return 0.0f;
    }
    auto subPos = position - bridgeTransform.GetWorldPosition();
    auto zValue = std::sqrtf(subPos.x * subPos.x + subPos.z * subPos.z);

    int i = 0;
    int heightsSize = gravityCollider.heights.size() - 1;
    while (gravityCollider.heights[i].first <= zValue && i < heightsSize) i++;
    minIndex = i;

    i = gravityCollider.heights.size() - 1;
    while (gravityCollider.heights[i].first >= zValue && i > 0) i--;
    maxIndex = i;

    if (maxIndex == -1 && minIndex == -1)
    {
        LogError("GravitySystem::GetBridgeLevel(): minIndex and maxIndex not estabilished");
        return 0.0f;
    }
    if (maxIndex == minIndex) 
        return gravityCollider.heights[maxIndex].second;
    auto& minPos = gravityCollider.heights[minIndex];
    auto& maxPos = gravityCollider.heights[maxIndex];
    if (maxPos.second == minPos.second) return maxPos.second;

    return ((zValue - minPos.first) / (maxPos.first - minPos.first)) * std::max(minPos.second, maxPos.second);
}
