#include <iostream>

#include "PhysicsSystem.h"
#include "HFEngine.h"

#include "../Components.h"

#define clamp(x,min,max) x > min ? (x < max ? x : max) : min
#define veclen(v) std::sqrtf(v.x*v.x + v.y*v.y);

void PhysicsSystem::Init()
{
	step = 0.05f;
}

/*
 * TODO LIST:
 * -refactor
 * -what about transform scale?
 * -what about moving dynamic colliders into another colliders (no collision detection when moved inside loops)?
 * -what about rigidBody requirement?
 */
void PhysicsSystem::Update(float dt)
{
    bool collided, localCollided;
	for (auto const& gameObject : gameObjects)
	{
		auto& rigidBody = HFEngine::ECS.GetComponent<RigidBody>(gameObject);
		auto& transform = HFEngine::ECS.GetComponent<Transform>(gameObject);

		glm::vec3 displacement;
		if (rigidBody.moved)
		{
			displacement = rigidBody.movePosition - transform.GetPosition();
		}
		else
		{
			displacement = rigidBody.velocity * dt;
		}

        if (!HFEngine::ECS.SearchComponent<Collider>(gameObject))
        {
            transform.SetPosition(transform.GetPosition() + displacement);
            continue;
        }

		float length = sqrt((displacement.x * displacement.x) + (displacement.z * displacement.z));

        if (islessequal(length, step)) continue;
        int steps = std::max(1, (int)std::round(length / step));
		glm::vec3 moveStep = displacement / (float)steps;
        glm::vec3 tempPosition = transform.GetPosition();
        glm::vec3 sepVector(0.0f);
        glm::vec3 oldVelocity = rigidBody.velocity;

        auto& collider = HFEngine::ECS.GetComponent<Collider>(gameObject);

        for (int s = 0; s < steps; s++)
        {
            tempPosition += moveStep;
            transform.SetPosition(tempPosition);
            
            collided = false;
            // get gameObjects from colliderCollectingSystem
            for (auto const& otherObject : colliderCollectorSystem->gameObjects)
            {
                if (gameObject != otherObject)
                {
                    auto& otherTransform = HFEngine::ECS.GetComponent<Transform>(otherObject);
                    auto const& otherCollider = HFEngine::ECS.GetComponent<Collider>(otherObject);
                    auto const& otherPosition = otherTransform.GetPosition();

                    localCollided = false;

                    if (collider.shape == Collider::ColliderShapes::CIRCLE && otherCollider.shape == Collider::ColliderShapes::CIRCLE)
                    {
                        if (DetectCollision(tempPosition, HFEngine::ECS.GetComponent<CircleCollider>(gameObject), otherPosition, HFEngine::ECS.GetComponent<CircleCollider>(otherObject), sepVector))
                        {
                            collided = true;
                            localCollided = true;
                        }
                    } else if (collider.shape == Collider::ColliderShapes::CIRCLE && otherCollider.shape == Collider::ColliderShapes::BOX)
                    {
                        if (DetectCollision(tempPosition, HFEngine::ECS.GetComponent<CircleCollider>(gameObject), otherPosition, otherTransform.GetRotationEuler().y, HFEngine::ECS.GetComponent<BoxCollider>(otherObject), sepVector))
                        {
                            collided = true;
                            localCollided = true;
                        }
                    }

                    if (localCollided)
                    {
                        if (otherCollider.type == Collider::ColliderTypes::DYNAMIC)
                        {
                            sepVector *= 0.5f; // mass?
                            //auto& otherRb = HFEngine::ECS.GetComponent<RigidBody>(otherObject);
                            otherTransform.SetPosition(otherPosition - sepVector); // rb move?
                            //otherRb.velocity.x -= sepVector.x / dt;
                            //otherRb.velocity.z -= sepVector.y / dt;
                        }

                        tempPosition += sepVector;
                        rigidBody.velocity.x += sepVector.x / dt;
                        rigidBody.velocity.z += sepVector.y / dt;
                    }
                }
            }
            if (collided) break;
        }
        transform.SetPosition(tempPosition);
        //if (rigidBody.velocity.x * oldVelocity.x < 0) rigidBody.velocity.x = 0.0f;
        //if (rigidBody.velocity.z * oldVelocity.z < 0) rigidBody.velocity.z = 0.0f;
        rigidBody.moved = false;
	}
}

bool PhysicsSystem::DetectCollision(const glm::vec3& pos1, const CircleCollider& c1, const glm::vec3& pos2, const CircleCollider& c2, glm::vec3& sepVector)
{
    float r = c1.radius + c2.radius;
    glm::vec2 c1c2(pos1.x - pos2.x, pos1.z - pos2.z);

    float dist = veclen(c1c2);
    if (dist >= r) return false;

    sepVector.x = (c1c2.x / dist) * (r - dist);
    sepVector.z = (c1c2.y / dist) * (r - dist);

    return true;
}

bool PhysicsSystem::DetectCollision(const glm::vec3& pos1, const CircleCollider& c1, const glm::vec3& pos2, const float& rotation2, const BoxCollider& c2, glm::vec3& sepVector)
{
    // TODO: VERY VERY DYRTI
    float tbx1, tby1, tbx2, tby2;
    //float bx1, by1, bx2, by2;
    tbx1 = -c2.width / 2.0f;
    tby1 = -c2.height / 2.0f;
    tbx2 = c2.width / 2.0f;
    tby2 = c2.height / 2.0f;

    /*bx1 = (tbx1 * cos(rotation2)) - (tby1 * sin(rotation2)) + pos2.x;
    by1 = (tbx1 * sin(rotation2)) + (tby1 * cos(rotation2)) + pos2.z;
    bx2 = (tbx2 * cos(rotation2)) - (tby2 * sin(rotation2)) + pos2.x;
    by2 = (tbx2 * sin(rotation2)) + (tby2 * cos(rotation2)) + pos2.z;*/

    glm::vec2 circlePoints[2] = {
        { pos1.x - c1.radius, pos1.y - c1.radius },
        { pos1.x + c1.radius, pos1.y + c1.radius }
    };
    glm::vec2 boxPoints[4] = {
        { (tbx1 * cos(rotation2)) - (tby1 * sin(rotation2)) + pos2.x, (tbx1 * sin(rotation2)) + (tby1 * cos(rotation2)) + pos2.z },
        { (tbx2 * cos(rotation2)) - (tby1 * sin(rotation2)) + pos2.x, (tbx2 * sin(rotation2)) + (tby1 * cos(rotation2)) + pos2.z },
        { (tbx2 * cos(rotation2)) - (tby2 * sin(rotation2)) + pos2.x, (tbx2 * sin(rotation2)) + (tby2 * cos(rotation2)) + pos2.z },
        { (tbx1 * cos(rotation2)) - (tby2 * sin(rotation2)) + pos2.x, (tbx1 * sin(rotation2)) + (tby2 * cos(rotation2)) + pos2.z }
    };
    glm::vec2 circleCenter(pos1.x, pos1.z);
    glm::vec2 tmpVec = boxPoints[0] - circleCenter;
    float tmpLen;

    int index = 0;
    float min_distance = veclen(tmpVec);
    for (int i = 1; i < 4; i++)
    {
        tmpVec = boxPoints[i] - circleCenter;
        tmpLen = veclen(tmpVec);
        if (tmpLen < min_distance)
        {
            min_distance = tmpLen;
            index = i;
        }
    }

    glm::vec2 axis[3] = {
        { boxPoints[1].x - boxPoints[0].x, boxPoints[1].y - boxPoints[0].y },
        { boxPoints[1].x - boxPoints[2].x, boxPoints[1].y - boxPoints[2].y },
        { boxPoints[index].x - circleCenter.x, boxPoints[index].y - circleCenter.y }
    };

    float box_min = 0.f, box_max = 0.f;
    float circle_min = 0.f, circle_max = 0.f;

    float smallestOverlap = std::numeric_limits<float>::max();
    glm::vec2 MTVAxis(0.0f);

    for (int i = 0; i < 3; i++)
    {
        box_min = glm::dot(boxPoints[0], axis[i]);
        box_max = box_min;

        for (int j = 0; j < 4; j++)
        {
            float projection = glm::dot(boxPoints[j], axis[i]);
            if (projection < box_min) box_min = projection;
            if (projection > box_max) box_max = projection;
        }

        float angle = atan(axis[i].y / axis[i].x);
        glm::vec2 pointPerimeters[2] = {
            { circleCenter.x - c1.radius * cos(angle), circleCenter.y - c1.radius * sin(angle) },
            { circleCenter.x + c1.radius * cos(angle), circleCenter.y + c1.radius * sin(angle) }
        };

        float circle_1_projection = glm::dot(pointPerimeters[0], axis[i]);
        float circle_2_projection = glm::dot(pointPerimeters[1], axis[i]);

        if (circle_1_projection < circle_2_projection)
        {
            circle_min = circle_1_projection;
            circle_max = circle_2_projection;
        }
        else
        {
            circle_min = circle_2_projection;
            circle_max = circle_1_projection;
        }

        auto min1 = std::min(circle_max, box_max);
        auto max1 = std::max(circle_min, box_min);
        auto diff = min1 - max1;
        auto overlap = std::max(0.0f, diff);
        //if (overlap == 0) return false;

        if (!((circle_min <= box_max) && (circle_max >= box_min)))
            return false;

        if (overlap < smallestOverlap)
        {
            smallestOverlap = overlap;
            MTVAxis = axis[i];
        }
    }

    auto p2top1 = glm::vec2(pos2.x - pos1.x, pos2.z - pos1.z);
    if (glm::dot(MTVAxis, p2top1) >= 0)
    {
        MTVAxis *= -1.0f;
    }
    sepVector.x = MTVAxis.x * smallestOverlap;
    sepVector.z = MTVAxis.y * smallestOverlap;

    return true;
}

/*bool PhysicsSystem::DetectCollision(const glm::vec3& pos1, const CircleCollider& c1, const glm::vec3& pos2, const float& rotation2, const BoxCollider& c2, glm::vec3& sepVector)
{
    float tbx1, tby1, tbx2, tby2;
    float bx1, by1, bx2, by2;
    tbx1 = -c2.width / 2.0f;
    tby1 = -c2.height / 2.0f;
    tbx2 = c2.width / 2.0f;
    tby2 = c2.height / 2.0f;

    float rrotation2 = glm::radians(rotation2);

    bx1 = (tbx1 * cos(rotation2)) - (tby1 * sin(rotation2)) + pos2.x;
    by1 = (tbx1 * sin(rotation2)) + (tby1 * cos(rotation2)) + pos2.z;
    bx2 = (tbx2 * cos(rotation2)) - (tby2 * sin(rotation2)) + pos2.x;
    by2 = (tbx2 * sin(rotation2)) + (tby2 * cos(rotation2)) + pos2.z;

    //float closestX = clamp(pos1.x, bx1, bx2);
    //float closestY = clamp(pos1.z, by1, by2);

    // Calculate the distance between the circle's center and this closest point
    //float distanceX = pos1.x - closestX;
    //float distanceY = pos1.z - closestY;

    glm::vec2 f = { clamp(pos1.x, bx1, bx2), clamp(pos1.z, by1, by2) };
    glm::vec2 cf = { pos1.x - f.x, pos1.z - f.y };
    float dist = veclen(cf);
    if (dist >= c1.radius)  return false;

    if (dist < 0.001f)
    {
        float left = pos1.x - bx1 + c1.radius, right = bx2 - pos1.x + c1.radius;
        float top = pos1.y - by1 + c1.radius, bottom = by2 - pos1.y + c1.radius;

        left < right ? sepVector.x = -left : sepVector.x = right;
        top < bottom ? sepVector.z = -top : sepVector.y = bottom;
        if (abs(sepVector.x) < abs(sepVector.z))
            sepVector.z = 0;
        else if (abs(sepVector.x) > abs(sepVector.z))
            sepVector.x = 0;
    }
    else
    {
        sepVector.x = (cf.x / dist) * (c1.radius - dist);
        sepVector.z = (cf.y / dist) * (c1.radius - dist);
    }

    //var penetrationDepth = circle.r - dist;
    //var penetrationVector = dist.normalise().mult(penetrationDepth);

    /*auto penDepth = c1.radius - dist;
    auto penVec = glm::normalize(cf) * dist;
    sepVector.x = penVec.x;
    sepVector.z = penVec.y;*/

    //return true;

    // If the distance is less than the circle's radius, an intersection occurs
    /*float distanceSquared = (distanceX * distanceX) + (distanceY * distanceY);
    if (distanceSquared < (c1.radius * c1.radius))
    {
        sepVector.x = distanceX;
        sepVector.z = distanceY;

        return true;
    }
    return false;*/
//}

void PhysicsSystem::SetCollector(std::shared_ptr<ColliderCollectorSystem> colliderCollectorSystem)
{
    this->colliderCollectorSystem = colliderCollectorSystem;
}
