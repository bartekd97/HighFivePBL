#version 330 core
layout (location = 0) out vec3 gAlbedo;
layout (location = 1) out float gMetalness;
layout (location = 2) out float gRoughness;

const float PI = 3.14159265359;

uniform int GatesCount = 0;
uniform vec2 Gates[8];
uniform int PolygonPointsCount = 0;
uniform vec2 PolygonPoints[64];
uniform vec2 CellCenter;

in vec2 TexCoords;

uniform sampler2D grassTexture;
uniform sampler2D roadTexture;
uniform sampler2D cliffTexture;
uniform float grassTiling;
uniform float roadTiling;
uniform float cliffTiling;

uniform float gRoadWidth = 0.01f;
uniform float gRoadSmoothingWidth = 0.035f;
uniform float gRoadCenterRadius = 0.06f;

uniform float gCliffLevel = 0.9f;
uniform float gCliffSmoothing = 0.07f;

float DistToLine(vec2 pt1, vec2 pt2, vec2 testPt);
bool IsPointInside(vec2 point);
float GetEdgeCenterRatio(vec2 point);
float GetDistanceToRoadGate(vec2 point);

float GetRoadFactor()
{
    float cFactor, rFactor;

    float cDist = length(TexCoords - CellCenter);
    if (cDist < gRoadCenterRadius)
        cFactor = 0.0f;
    else if (cDist < (gRoadCenterRadius + gRoadSmoothingWidth))
    {
        float diff = cDist - gRoadCenterRadius;
        cFactor = pow(diff / gRoadSmoothingWidth, 2);
    }
    else
        cFactor = 1.0f;

    float rDist = GetDistanceToRoadGate(TexCoords);
    if (rDist < gRoadWidth)
        rFactor = 0.0f;
    else if (rDist < (gRoadWidth + gRoadSmoothingWidth))
    {
        float diff = rDist - gRoadWidth;
        rFactor = pow(diff / gRoadSmoothingWidth, 2);
    }
    else
        rFactor = 1.0f;

    return 1.0f - min(cFactor, rFactor);
}

float GetCliffFactor()
{
    float factor;
    float innerLevel = GetEdgeCenterRatio(TexCoords - CellCenter);
    if (innerLevel > gCliffLevel)
        factor = 0.0f;
    else if (innerLevel > (gCliffLevel - gCliffSmoothing))
    {
        float diff = gCliffLevel - innerLevel;
        factor = pow(diff / gCliffSmoothing, 2);
    }
    else
        factor = 1.0f;

    return 1.0f - factor;
}



void main()
{
    vec3 grass = texture(grassTexture, TexCoords * grassTiling).rgb;
    vec3 road = texture(roadTexture, TexCoords * roadTiling).rgb;
    vec3 cliff = texture(cliffTexture, TexCoords * cliffTiling).rgb;

    float roadFactor = GetRoadFactor();
    float cliffFactor = GetCliffFactor();

    vec3 albedo;
    albedo = mix(grass, road, roadFactor);
    albedo = mix(albedo, cliff, cliffFactor);

    gAlbedo = albedo;
    gMetalness = mix(0.1f, 0.3f, cliffFactor);
    gRoughness = mix(0.65f, 0.45f, roadFactor);
}





float DistToLine(vec2 pt1, vec2 pt2, vec2 testPt)
{
    vec2 lineDir = pt2 - pt1;
    vec2 perpDir = vec2(lineDir.y, -lineDir.x);
    vec2 dirToPt1 = pt1 - testPt;
    return abs(dot(normalize(perpDir), dirToPt1));
}


bool IsPointInside(vec2 point)
{
    //n>2 Keep track of cross product sign changes
    float pos = 0;
    float neg = 0;

    for (int i = 0; i < PolygonPointsCount; i++)
    {
        //If point is in the polygon
        if (PolygonPoints[i] == point)
            return true;

        //Form a segment between the i'th point
        float x1 = PolygonPoints[i].x;
        float y1 = PolygonPoints[i].y;

        //And the i+1'th, or if i is the last, with the first point
        int i2 = i < PolygonPointsCount - 1 ? i + 1 : 0;

        float x2 = PolygonPoints[i2].x;
        float y2 = PolygonPoints[i2].y;

        float x = point.x;
        float y = point.y;

        //Compute the cross product
        float d = (x - x1) * (y2 - y1) - (y - y1) * (x2 - x1);

        if (d > 0) pos++;
        if (d < 0) neg++;

        //If the sign changes, then point is outside
        if (pos > 0 && neg > 0)
            return false;
    }

    //If no change in direction, then on same side of all segments, and thus inside
    return true;
}


float GetEdgeCenterRatio(vec2 point)
{

    if (!IsPointInside(CellCenter + point))
        return 1.0f;

    if (length(point) < 0.01f)
        return 0.0f;
        

    vec2 direction = normalize(point) * 0.1f;
    vec2 edgePoint = point + direction;
    for (int i = 0; i < 10; i++)
        if (IsPointInside(CellCenter + edgePoint))
            edgePoint += direction;

    direction *= 0.5f;
    edgePoint -= direction;
    for (int j = 0; j < 5; j++)
    {
        direction *= 0.5f;
        if (IsPointInside(CellCenter + edgePoint))
            edgePoint += direction;
        else
            edgePoint -= direction;
    }

    return length(point) / length(edgePoint);
}

float GetDistanceToRoadGate(vec2 point)
{
    if (length(point) < 0.01f)
        return 0.0f;
    
    float minDistance = 1.0f;
    for (int i = 0; i < GatesCount; i++)
    {
        vec2 gate = Gates[i];
        if (dot(normalize(CellCenter - point), normalize(CellCenter - gate)) > 0.0f)
        {
            float dist = DistToLine(CellCenter, gate, point);
            if (dist < minDistance)
                minDistance = dist;
        }
    }

    return minDistance;
}