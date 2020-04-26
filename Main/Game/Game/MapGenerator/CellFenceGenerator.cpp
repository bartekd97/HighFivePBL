#include <glm/gtx/vector_angle.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include "CellFenceGenerator.h"
#include "HFEngine.h"
#include "ECS/Components.h"

GameObject CellFenceGenerator::CreateGate(GameObject bridge, GameObject parent, Transform& parentTransform)
{
    glm::vec3 offsetPosition =
        HFEngine::ECS.GetComponent<Transform>(bridge).GetWorldPosition() -
        parentTransform.GetWorldPosition();
        //HFEngine::ECS.GetComponent<Transform>(parent).GetWorldPosition();

    glm::vec2 bridgePosition = glm::vec2(
        offsetPosition.x,
        offsetPosition.z
        );
    glm::vec2 frontDirection = glm::normalize(glm::vec2(
        HFEngine::ECS.GetComponent<Transform>(bridge).GetWorldFront().x,
        HFEngine::ECS.GetComponent<Transform>(bridge).GetWorldFront().z
        ));
    glm::vec2 gateDirection = glm::normalize(glm::vec2(
        HFEngine::ECS.GetComponent<Transform>(bridge).GetWorldRight().x,
        HFEngine::ECS.GetComponent<Transform>(bridge).GetWorldRight().z
        ));

    if (glm::length(bridgePosition) < glm::length(bridgePosition + frontDirection))
    {
        frontDirection = -frontDirection;
        gateDirection = -gateDirection;
    }

    glm::vec2 gatePosition = bridgePosition + frontDirection * config.gateDistance;

    GameObject element = HFEngine::ECS.CreateGameObject(parent);
    /*
    element.transform.parent = parent.transform;
    element.transform.localPosition = new glm::vec3(
        gatePosition.x,
        0.0f,
        gatePosition.y
        );
    element.transform.right = new glm::vec3(
        gateDirection.x,
        0.0f,
        gateDirection.y
        );
        */

    auto gate = std::make_shared<Gate>();
    gate->position = gatePosition;
    gate->direction = gateDirection;
    gates.push_back(gate);

    return element;
}

void CellFenceGenerator::PrepareBrokenCurves()
{
    std::sort(gates.begin(), gates.end(), [](std::shared_ptr<Gate> a, std::shared_ptr<Gate> b) {
        float angleA = glm::orientedAngle({ 0,1 }, glm::normalize(a->position));
        float angleB = glm::orientedAngle({ 0,1 }, glm::normalize(b->position));
        return angleA < angleB;
        });
    //std::reverse(gates.begin(), gates.end());

    std::vector<glm::vec2> outline;
    outline.insert(outline.end(), polygon.Points.begin(), polygon.Points.end());
    outline.insert(outline.end(), polygon.Points.begin(), polygon.Points.end()); // make it overlap for easier indexing

    std::shared_ptr<Gate> fromGate, toGate;
    glm::vec2 startPoint, endPoint;
    for (int i = 0; i < gates.size(); i++)
    {
        fromGate = gates[i];
        toGate = gates[i == gates.size() - 1 ? 0 : i + 1];

        // repalced startPoint with endPoint due to different transform space
        startPoint = fromGate->position - fromGate->direction * config.gateEntity.length * 0.5f;
        endPoint = toGate->position + toGate->direction * config.gateEntity.length * 0.5f;

        int fromIndex = 0, toIndex = 0;
        for (int j = 0; j < polygon.Points.size(); j++)
        {
            if (glm::length(startPoint - outline[j])
                < glm::length(startPoint - outline[fromIndex]))
                fromIndex = j;
            if (glm::length(endPoint - outline[j])
                < glm::length(endPoint - outline[toIndex]))
                toIndex = j;
        }

        if (toIndex < fromIndex)
            toIndex += polygon.Points.size();

        if (toIndex <= 2)
        {
            fromIndex += polygon.Points.size();
            toIndex += polygon.Points.size();
        }


        if (glm::dot(
        glm::normalize(outline[fromIndex + 1] - outline[fromIndex]),
            glm::normalize(startPoint - outline[fromIndex])
            ) > -0.2f)
            fromIndex++;
        if (glm::dot(
            glm::normalize(outline[toIndex - 1] - outline[toIndex]),
            glm::normalize(endPoint - outline[toIndex])
            ) > -0.2f)
            toIndex--;

        if (glm::dot(
            glm::normalize(outline[fromIndex + 1] - outline[fromIndex]),
            glm::normalize(startPoint - outline[fromIndex])
            ) > -0.2f)
            fromIndex++;
        if (glm::dot(
            glm::normalize(outline[toIndex - 1] - outline[toIndex]),
            glm::normalize(endPoint - outline[toIndex])
            ) > -0.2f)
            toIndex--;


        std::vector<glm::vec2> curve;
        curve.push_back(startPoint);
        for (int k = fromIndex; k <= toIndex; k++)
            curve.push_back(outline[k]);
        curve.push_back(endPoint);

        std::vector<glm::vec2> reversedCurve;
        reversedCurve.insert(reversedCurve.end(), curve.begin(), curve.end());
        std::reverse(reversedCurve.begin(), reversedCurve.end());

        auto brokenCurve = std::make_shared<BrokenCurve>();
        brokenCurve->points = curve;
        brokenCurve->reversedPoints = reversedCurve;
        brokenCurves.push_back(brokenCurve);
    }
}

void CellFenceGenerator::CreateSections()
{
    float lengthForNextSegment = config.fragmentEntity.length * config.fragmentCount + config.connectorEntity.length;
    float minHoleGap = lengthForNextSegment + config.connectorEntity.length;

    std::vector<glm::vec2>* targetPoints;
    std::vector<std::shared_ptr<Segment>>* targetSegments;
    int* targetPointI;

    glm::vec2 nextPosition;
    glm::vec2 direction;
    std::shared_ptr<Segment> lastSegment;

    for (auto curve : brokenCurves)
    {
        int i = 0;
        int created = 0;
        float holeDistance;
        while ((holeDistance = curve->HoleDistance()) > minHoleGap || i < 2)
        {
            if (i % 2 == 0)
            {
                targetPoints = &curve->points;
                targetSegments = &curve->forawrdSegments;
                targetPointI = &curve->targetPointI;
            }
            else
            {
                targetPoints = &curve->reversedPoints;
                targetSegments = &curve->backwardSegments;
                targetPointI = &curve->targetReversedPointI;
            }

            nextPosition = targetPoints->at(0);
            if (targetSegments->size() > 0)
            {
                lastSegment = targetSegments->at(targetSegments->size() - 1);
                nextPosition = lastSegment->position + lastSegment->direction * lengthForNextSegment;
            }

            direction = targetPoints->at(*targetPointI) - nextPosition;
            if (glm::length(direction) >= lengthForNextSegment)
            {
                auto segment = std::make_shared<Segment>();
                segment->position = nextPosition;
                segment->direction = glm::normalize(direction);
                segment->fragmentCount = config.fragmentCount;
                segment->fragmentLength = config.fragmentEntity.length;
                targetSegments->push_back(segment);
                created++;
            }
            else if (*targetPointI < targetPoints->size() - 1)
            {
                *targetPointI = *targetPointI + 1;
            }

            i++;
        }
    }
}

void CellFenceGenerator::TryFillGapsInSections()
{
    for (int i=0; i<brokenCurves.size(); i++)
    {
        int fi = brokenCurves[i]->forawrdSegments.size() - 1;
        int bi = brokenCurves[i]->backwardSegments.size() - 1;
        brokenCurves[i]->filler = TryFillGap(
            brokenCurves[i]->forawrdSegments[fi],
            brokenCurves[i]->backwardSegments[bi]
            );
    }
}
std::vector<std::shared_ptr<CellFenceGenerator::FenceObject>>
    CellFenceGenerator::TryFillGap(
        std::shared_ptr<CellFenceGenerator::Segment> forward,
        std::shared_ptr<CellFenceGenerator::Segment> backward
        )
{
    std::vector<std::shared_ptr<CellFenceGenerator::FenceObject>> filler;

    CellFenceEntity* fragment = &config.fragmentEntity;
    CellFenceEntity* connector = &config.connectorEntity;
    float conLen2 = connector->length * 2.0f;

    glm::vec2 forwardDirFrom = forward->direction;
    glm::vec2 forwardDirTo = glm::normalize(backward->position - forward->position);
    glm::vec2 backwarddDirFrom = backward->direction;
    glm::vec2 backwardDirTo = glm::normalize(forward->position - backward->position);

    for (float f = 1.0f; f >= 0.0f; f -= 0.2f)
    {
        forward->direction = glm::normalize(glm::mix(
            forwardDirFrom, forwardDirTo, f
            ));
        backward->direction = glm::normalize(glm::mix(
            backwarddDirFrom, backwardDirTo, f
            ));

        float gap = glm::length(forward->endPosition() - backward->endPosition());
        glm::vec2 dir = glm::normalize(backward->endPosition() - forward->endPosition());

        // maybe just 1 connector?
        if (connector->InFillRange(gap))
        {
            filler.push_back(std::make_shared<FenceObject>(
                forward->endPosition() + dir * gap * 0.5f,
                dir,
                connector
                ));
            return filler;
        }

        // maybe 2 connectors + fragment?
        if (fragment->InFillRange(gap - conLen2))
        {
            float fragLen = gap - conLen2;
            filler.push_back(std::make_shared<FenceObject>(
                    forward->endPosition() + dir * connector->length * 0.5f,
                    dir,
                    connector
                ));
            filler.push_back(std::make_shared<FenceObject>(
                    forward->endPosition() + dir * (connector->length + fragLen * 0.5f),
                    dir,
                    fragment
                ));
            filler.push_back(std::make_shared<FenceObject>(
                    forward->endPosition() + dir * (connector->length * 1.5f + fragLen),
                    dir,
                    connector
                ));
            return filler;
        }

        // maybe 2 connectors + 2 fragments?
        if (fragment->InFillRange(gap - conLen2, 2))
        {
            float fragsLen = gap - conLen2;
            float fragHalfLen = (fragsLen - fragment->length) * 0.5f;
            filler.push_back(std::make_shared<FenceObject>(
                    forward->endPosition() + dir * connector->length * 0.5f,
                    dir,
                    connector
                ));
            filler.push_back(std::make_shared<FenceObject>(
                    forward->endPosition() + dir * (connector->length + fragHalfLen),
                    dir,
                    fragment
                ));
            filler.push_back(std::make_shared<FenceObject>(
                    forward->endPosition() + dir * (connector->length + fragHalfLen + fragment->length),
                    dir,
                    fragment
                ));
            filler.push_back(std::make_shared<FenceObject>(
                    forward->endPosition() + dir * (connector->length * 1.5f + fragsLen),
                    dir,
                    connector
                ));
            return filler;
        }


        if (f < 0.00001f)
        {
            // couldn't find fill?
            // remove one fragment from one segment and try again
            if (backward->fragmentCount == 2)
            {
                backward->fragmentCount = 1;
                f = 1.2f;
            }
            // couldnt find then? try with three fragments instead
            else if (backward->fragmentCount == 1)
            {
                backward->fragmentCount = 3;
                f = 1.2f;
            }
            // couldnt find even then? reverse change and cancel finding :c
            else if (backward->fragmentCount == 3)
            {
                // leave it "random"
                if (((int)(glm::length(backward->position))) % 2 == 0)
                {
                    forward->fragmentCount = 2;
                    backward->fragmentCount = 3;
                }
                else
                {
                    forward->fragmentCount = 3;
                    backward->fragmentCount = 2;
                }
                break;
            }
        }
    }

    return filler;
}

void CellFenceGenerator::MakeHoles()
{
    // TODO - make it better
    for (auto curve : brokenCurves)
    {
        if (curve->filler.size() == 0)
        {
            curve->forawrdSegments[curve->forawrdSegments.size() - 1]->fragmentCount = 0;
            curve->backwardSegments[curve->backwardSegments.size() - 1]->fragmentCount = 0;
        }
    }
}

void CellFenceGenerator::PrepareObjects()
{
    for (auto curve : brokenCurves)
    {
        curve->objects.clear();
        PrepareObjectsForSection(curve->forawrdSegments, curve->objects, false);
        PrepareObjectsForSection(curve->backwardSegments, curve->objects, true);
        if (curve->filler.size() == 0)
            curve->objects.insert(curve->objects.end(), curve->filler.begin(), curve->filler.end());
    }
}
void CellFenceGenerator::PrepareObjectsForSection(std::vector<std::shared_ptr<Segment>>& segments, std::vector<std::shared_ptr<FenceObject>>& target, bool reversed)
{
    for (int i = 0; i < segments.size(); i++)
    {
        if (i > 0)
        {
            glm::vec2 connectorFrom = segments[i - 1]->endPosition();
            glm::vec2 connectorTo = segments[i]->position;
            glm::vec2 connectorDirection = glm::normalize(connectorTo - connectorFrom);
            target.push_back(std::make_shared<FenceObject>(
                    (connectorFrom + connectorTo) * 0.5f,
                    reversed ? -connectorDirection : connectorDirection,
                    &config.connectorEntity
                ));
        }

        float dist = 0.0f;
        for (int j = 0; j < segments[i]->fragmentCount; j++)
        {
            float eDist = dist + config.fragmentEntity.length * 0.5f;
            target.push_back(std::make_shared<FenceObject>(
                    segments[i]->position + segments[i]->direction * eDist,
                    reversed ? -segments[i]->direction : segments[i]->direction,
                    &config.fragmentEntity
                ));
            dist += config.fragmentEntity.length;
        }
    }
}

void CellFenceGenerator::BuildSections(GameObject parent)
{
    //if (parent > 400) return;
    GameObject element, container;
    for (int i = 0; i < brokenCurves.size(); i++)
    {
        container = HFEngine::ECS.CreateGameObject(parent, std::to_string(i));

        for (auto o : brokenCurves[i]->objects)
        {
            float rotation = rad2deg(glm::atan(o->direction.x, o->direction.y)) + 90; // + 90 for right
            element = o->entity->prefab->Instantiate(
                parent,
                { o->position.x, 0.0f, o->position.y },
                { 0.0f, rotation, 0.0f }
               );
        }
    }
}
