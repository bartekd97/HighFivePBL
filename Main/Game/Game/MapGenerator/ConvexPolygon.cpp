#include <algorithm>
#include "ConvexPolygon.h"
#include "Utility/Utility.h"

void ConvexPolygon::CalculatePointAngles()
{
    pointAngles.reserve(Points.size());
    for (auto& p : Points)
        pointAngles.push_back({ p, glm::atan(p.x, p.y) });

    std::sort(pointAngles.begin(), pointAngles.end(), [](PointAngle& a, PointAngle& b) {
        return a.angle < b.angle;
        });
}

ConvexPolygon ConvexPolygon::CreateCircular(int segments, int precision)
{
    std::vector<glm::vec2> cpoints;
    cpoints.reserve(segments);

    glm::vec2 point, direction;
    for (int i = 0; i < segments; i++)
    {
        direction = glm::vec2(
            glm::cos(((float)i / (float)segments) * M_PI * 2.0f),
            glm::sin(((float)i / (float)segments) * M_PI * 2.0f)
            );

        point = direction;
        while (IsPointInside(point))
            point += direction;


        direction *= 0.5f;
        point -= direction;
        for (int j = 0; j < precision; j++)
        {
            direction *= 0.5f;
            if (IsPointInside(point))
                point += direction;
            else
                point -= direction;
        }


        cpoints.push_back(point);
    }

    return ConvexPolygon(cpoints);
}

// https://stackoverflow.com/questions/1119627/how-to-test-if-a-point-is-inside-of-a-convex-polygon-in-2d-integer-coordinates
bool ConvexPolygon::IsPointInside(glm::vec2 point)
{
    //Check if a triangle or higher n-gon
    assert(Points.size() >= 3);

    //n>2 Keep track of cross product sign changes
    float pos = 0;
    float neg = 0;

    int i, i2;
    float x1, y1, x2, y2;
    for (int i = 0; i < Points.size(); i++)
    {
        //If point is in the polygon
        if (Points[i] == point)
            return true;

        //Form a segment between the i'th point
        x1 = Points[i].x;
        y1 = Points[i].y;

        //And the i+1'th, or if i is the last, with the first point
        i2 = i < Points.size() - 1 ? i + 1 : 0;

        x2 = Points[i2].x;
        y2 = Points[i2].y;

        //Compute the cross product
        auto d = (point.x - x1) * (y2 - y1) - (point.y - y1) * (x2 - x1);

        if (d > 0) pos++;
        if (d < 0) neg++;

        //If the sign changes, then point is outside
        if (pos > 0 && neg > 0)
            return false;
    }

    //If no change in direction, then on same side of all segments, and thus inside
    return true;
}

float ConvexPolygon::GetEdgeCenterRatio(glm::vec2 point, int precision)
{
    if (!IsPointInside(point))
        return 1.0f;

    float plen = glm::length(point);
    if (plen < 0.01f)
        return 0.0f;

    float angle = glm::atan(point.x, point.y);

    glm::vec2 pA = pointAngles[pointAngles.size() - 1].point;
    glm::vec2 pB = pointAngles[0].point;
    for (int i = 1; i < pointAngles.size(); i++)
        if (pointAngles[i - 1].angle <= angle && angle <= pointAngles[i].angle)
        {
            pA = pointAngles[i - 1].point;
            pB = pointAngles[i].point;
        }



    float edgeDist = Utility::GetDistanceBetweenPointAndSegment(point, pA, pB);

    return plen / (plen + edgeDist);
}

glm::vec4 ConvexPolygon::GetBounds()
{
    glm::vec4 bounds(0);
    for (int i = 0; i < Points.size(); i++)
    {
        bounds.x = glm::min(Points[i].x, bounds.x);
        bounds.y = glm::min(Points[i].y, bounds.y);
        bounds.z = glm::max(Points[i].x, bounds.z);
        bounds.w = glm::max(Points[i].y, bounds.w);
    }
    return bounds;
}

ConvexPolygon ConvexPolygon::ShellScaledBy(float mult)
{
    float sumlen = 0.0f;
    for (int i = 0; i < Points.size(); i++)
        sumlen += glm::length(Points[i]);
    float moveBy = (1.0f - mult) * (sumlen / (float)Points.size());

    std::vector<glm::vec2> points;
    points.reserve(Points.size());
    for (int i = 0; i < Points.size(); i++)
        points.push_back(Points[i] - glm::normalize(Points[i]) * moveBy);
    return ConvexPolygon(points);
}

ConvexPolygon ConvexPolygon::ScaledBy(float mult)
{
    std::vector<glm::vec2> points;
    points.reserve(Points.size());
    for (int i = 0; i < Points.size(); i++)
        points.push_back(Points[i] * mult);
    return ConvexPolygon(points);
}

ConvexPolygon ConvexPolygon::BeveledVerticesBy(float mult)
{
    std::vector<glm::vec2> points;

    glm::vec2 previous, current, next;
    for (int i = 0; i < Points.size(); i++)
    {
        previous = Points[i > 0 ? i - 1 : Points.size() - 1];
        current = Points[i];
        next = Points[i < (Points.size() - 1) ? i + 1 : 0];

        points.push_back(glm::mix(current, previous, mult));
        points.push_back(glm::mix(current, next, mult));
    }

    return ConvexPolygon(points);
}
