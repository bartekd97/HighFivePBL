#include "ConvexPolygon.h"
#include "Utility/Utility.h"

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

    for (int i = 0; i < Points.size(); i++)
    {
        //If point is in the polygon
        if (Points[i] == point)
            return true;

        //Form a segment between the i'th point
        auto x1 = Points[i].x;
        auto y1 = Points[i].y;

        //And the i+1'th, or if i is the last, with the first point
        auto i2 = i < Points.size() - 1 ? i + 1 : 0;

        auto x2 = Points[i2].x;
        auto y2 = Points[i2].y;

        auto x = point.x;
        auto y = point.y;

        //Compute the cross product
        auto d = (x - x1) * (y2 - y1) - (y - y1) * (x2 - x1);

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

    if (glm::length(point) < 0.01f)
        return 0.0f;

    glm::vec2 direction = glm::normalize(point);
    glm::vec2 edgePoint = point + direction;
    while (IsPointInside(edgePoint))
        edgePoint += direction;

    direction *= 0.5f;
    edgePoint -= direction;
    for (int j = 0; j < precision; j++)
    {
        direction *= 0.5f;
        if (IsPointInside(edgePoint))
            edgePoint += direction;
        else
            edgePoint -= direction;
    }

    return glm::length(point) / glm::length(edgePoint);
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
