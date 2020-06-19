#include <vector>
#include "PointLightRendererSystem.h"
#include "ECS/Components/Transform.h"
#include "ECS/Components/PointLightRenderer.h"
#include "Rendering/RenderPipeline.h"
#include "Rendering/PrimitiveRenderer.h"
#include "Resourcing/Mesh.h"
#include "Utility/Utility.h"
#include "HFEngine.h"

void PointLightRendererSystem::Init()
{
	std::vector<Vertex> sphereVertices;
    std::vector<unsigned int> sphereIndices;
    AABBStruct sphereAABB = {
        {-1.0f,-1.0f,-1.0f},
        {1.0f,1.0f,1.0f}
    };

	const int sectorCount = 24.0f;
	const int stackCount = 12.0f;
    const float radius = 1.0f;

	float sectorStep = 2.0f * M_PI / sectorCount;
	float stackStep = M_PI / stackCount;
	float sectorAngle, stackAngle;

    Vertex v;
    float xy, z;
    int k1, k2;
    for (int i = 0; i <= stackCount; ++i)
    {
        k1 = i * (sectorCount + 1);     // beginning of current stack
        k2 = k1 + sectorCount + 1;      // beginning of next stack

        stackAngle = M_PI / 2 - i * stackStep;        // starting from pi/2 to -pi/2
        xy = radius * cosf(stackAngle);             // r * cos(u)
        z = radius * sinf(stackAngle);              // r * sin(u)

        // add (sectorCount+1) vertices per stack
        // the first and last vertices have same position and normal, but different tex coords
        for (int j = 0; j <= sectorCount; ++j, ++k1, ++k2)
        {
            sectorAngle = j * sectorStep;           // starting from 0 to 2pi

            // vertex position (x, y, z)
            v.position.x = xy * cosf(sectorAngle);             // r * cos(u) * cos(v)
            v.position.y = xy * sinf(sectorAngle);             // r * cos(u) * sin(v)
            v.position.z = z;
            sphereVertices.push_back(v);

            // 2 triangles per sector excluding first and last stacks
             // k1 => k2 => k1+1
            if (i != 0)
            {
                sphereIndices.push_back(k1);
                sphereIndices.push_back(k2);
                sphereIndices.push_back(k1 + 1);
            }

            // k1+1 => k2 => k2+1
            if (i != (stackCount - 1))
            {
                sphereIndices.push_back(k1 + 1);
                sphereIndices.push_back(k2);
                sphereIndices.push_back(k2 + 1);
            }
        }
    }


    sphereMesh = ModelManager::CreateMesh(sphereVertices, sphereIndices, sphereAABB);
    pointLightShader = ShaderManager::GetShader("PointLight");
    pointLightShader->use();
    pointLightShader->setInt("gPosition", (int)RenderPipeline::GBufferBindingPoint::POSITION);
    pointLightShader->setInt("gNormal", (int)RenderPipeline::GBufferBindingPoint::NORMAL);
    pointLightShader->setInt("gAlbedoFade", (int)RenderPipeline::GBufferBindingPoint::ALBEDO_FADE);
    pointLightShader->setInt("gMetalnessRoughnessShadow", (int)RenderPipeline::GBufferBindingPoint::METALNESS_ROUGHNESS_SHADOW);
}

unsigned int PointLightRendererSystem::Render(Camera& viewCamera, glm::vec2 viewportSize)
{
    unsigned int rendered = 0;

    pointLightShader->use();
    pointLightShader->setVector2F("viewportSize", viewportSize);
    viewCamera.Use(pointLightShader);

    glDepthMask(GL_FALSE);
    glEnable(GL_CULL_FACE); // TODO: make it better when intersecting camera frustum
    glCullFace(GL_BACK);
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE); // make it additive

    sphereMesh->bind();
    auto currentFrame = HFEngine::CURRENT_FRAME_NUMBER;
    auto it = gameObjects.begin();
    while (it != gameObjects.end())
    {
        auto gameObject = *(it++);
        auto& transform = HFEngine::ECS.GetComponent<Transform>(gameObject);
        auto& renderer = HFEngine::ECS.GetComponent<PointLightRenderer>(gameObject);

        if (renderer.cullingData.lastUpdate != currentFrame)
            continue;
        if (!renderer.cullingData.visibleByViewCamera)
            continue;
        if (renderer.light.intensity < 0.001f || renderer.light.radius < 0.001f)
            continue;

        renderer.light.position = transform.GetWorldPosition();
        renderer.light.Apply(pointLightShader, viewCamera);

        glm::mat4 model = glm::translate(glm::mat4(1.0f), renderer.light.position)
            * glm::scale(glm::mat4(1.0f), glm::vec3(renderer.light.radius));
        pointLightShader->setMat4("gModel", model);
        sphereMesh->draw();
        rendered++;
    }

    glDisable(GL_BLEND);
    glDisable(GL_CULL_FACE);
    glDepthMask(GL_TRUE);

    return rendered;
}
