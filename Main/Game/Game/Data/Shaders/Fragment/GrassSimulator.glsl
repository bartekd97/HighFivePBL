#version 330 core
out vec4 Result;

const float PI = 3.14159265359;

in vec2 TexCoords;

uniform sampler2D targetResult;
uniform sampler2D lastResult;
uniform float dt;


float ch(float from, float to, float speed)
{
    float delta = speed * dt;
    float diff = to - from;
    if (abs(diff) < delta)
        return diff;
    else
        return delta * sign(diff);
}


float GetBendDownSpeed(float currentLevel, float targetLevel)
{
    return 5.0f * (targetLevel - currentLevel) + 1.0f;
}
float GetBendUpSpeed(float currentLevel, float targetLevel)
{
    float begin = 0.25f * pow(currentLevel, 0.125);
    return begin * min((currentLevel - targetLevel) * 10.0f, 1);
}


float GetDirectionInertia(float currentLevel)
{
    return sqrt(currentLevel);
}
float GetDirectionChangeSpeed(float currentLevel, float levelChangeness)
{
    return mix(1.0f, abs(levelChangeness), currentLevel) * 10.0f;
}


float GetDirectionAngle(vec2 dir, float def)
{
    if (length(dir) < 0.000001f)
        return def;
    else
        return atan(dir.x, dir.y);
}


void main()
{
	vec4 current = texture(lastResult, TexCoords);
    vec4 target = texture(targetResult, TexCoords);


    float levelChangenessSpeed;
    if (target.w > current.w)
    {
        levelChangenessSpeed = GetBendDownSpeed(current.w, target.w);
    }
    else
    {
        levelChangenessSpeed = GetBendUpSpeed(current.w, target.w);
    }

    float levelChangeness = ch(current.w, target.w, levelChangenessSpeed);

    float newLevel = current.w + levelChangeness;

    float fromDirAngle = GetDirectionAngle(current.xy, 0);
    float toDirAngle = GetDirectionAngle(target.xy, fromDirAngle);

    if (levelChangeness < 0) // bending up, so first go to original pose
    {
        toDirAngle = fromDirAngle;
    }
    else if (current.w < 0.01f) // we are bending from original pose, so change dir immediately
    {
        fromDirAngle = toDirAngle;
    }

    float angleDiff = toDirAngle - fromDirAngle;
    if (angleDiff > PI)
    {
        toDirAngle -= PI*2;
    }
    else if (angleDiff < -PI)
    {
        fromDirAngle -= PI*2;
    } 

    toDirAngle = mix(toDirAngle, fromDirAngle, GetDirectionInertia(current.w));
    float angleChangeness = ch(fromDirAngle, toDirAngle, GetDirectionChangeSpeed(current.w, levelChangeness));


    vec2 newDirection = vec2(
        sin(fromDirAngle + angleChangeness),
        cos(fromDirAngle + angleChangeness)
    );

    // if grass is basically up, lets zero direction to allow proper interpolation
    newDirection *= pow(newLevel,2);


    Result = vec4(newDirection.x, newDirection.y, 0, newLevel);
}