#include "CellMeshGenerator.h"
#include "Utility/Utility.h"

void CellMeshGenerator::PrepareOutlines()
{
    baseOutline = originalPolygon;
    for (int i = 0; i < config.vertexBevelSteps; i++)
        baseOutline = baseOutline.BeveledVerticesBy(config.vertexBevel);

    baseOutline = baseOutline.CreateCircular(config.circularSegments);

    PolygonSmooth = baseOutline;
    PolygonSmoothInner = baseOutline.ShellScaledBy(config.innerScale);

    outlines.clear();
    int outlinesCount = config.outlineSplits + 1; // + outer
    float scale = 1.0f;
    float scaleFactor = config.nextOutlineScale;
    for (int i = 0; i < outlinesCount; i++)
    {
        outlines.push_back(
            baseOutline.ShellScaledBy(scale)
            );

        scale *= scaleFactor;
        scaleFactor *= config.nextOutlineScaleMultiplier;
    }
}

void CellMeshGenerator::PrepareBaseMeshStructure()
{
    vertices.clear();
    triangles.clear();

    for (int i = 0; i < outlines.size(); i++)
    {
        //float innerLevel = (float)i / (float)(outlines.Count - 1);
        //float depthLevel = (1.0f - Mathf.Sin(innerLevel * Mathf.PI * 0.5f));
        //float depthLevel = 1.0f - Mathf.Sqrt(innerLevel);
        //float depthLevel = 1.0f - (1 + (--innerLevel) * innerLevel * innerLevel * innerLevel * innerLevel);
        for (auto vpos : outlines[i].Points)
        {
            float coRatio = baseOutline.GetEdgeCenterRatio(vpos);

            float innerLevel = glm::clamp((1.0f - coRatio) / (1.0f - config.innerScale), 0.0f, 1.0f);
            float depthLevel = (1.0f - glm::sin(innerLevel * M_PI * 0.5f));

            float noisex = 0.0f, noisey = 0.0f, noisez = 0.0f;
            float noiseLevel = 1.0f - glm::clamp((1.0f - coRatio) / (1.0f - config.noiseCenterRatio), 0.0f, 1.0f);
            if (noiseLevel > 0.0f)
            {
                /*
                float pn = Mathf.PerlinNoise(
                    vpos.x * config.noiseScale,
                    vpos.y * config.noiseScale
                    );
                    */
                float pn = 0.5f;
                pn = (pn * 2.0f - 1.0f);
                pn = pn * noiseLevel * innerLevel;

                glm::vec2 noiseDir = glm::normalize(vpos);
                noisex = noiseDir.x * pn * config.noiseForce.x;
                noisez = noiseDir.y * pn * config.noiseForce.x;

                noisey = pn * config.noiseForce.y;
            }

            vertices.push_back(std::make_shared<IndexedVertex>(glm::vec3(
                noisex + vpos.x,
                noisey + (i == 0 ? -config.depthMax : -config.depth) * depthLevel,
                noisez + vpos.y )
                ));
        }
    }

    // make outline triangles
    std::shared_ptr<IndexedVertex> a,b,c;
    int outlineLength = outlines[0].Points.size();
    for (int i = 0, nexti; i < outlines.size() - 1; i++)
    {
        nexti = i + 1;
        for (int j = 0, nextj; j < outlineLength; j++)
        {
            nextj = j == outlineLength - 1 ? 0 : j + 1;
            a = vertices[j + i * outlineLength];
            b = vertices[j + nexti * outlineLength];
            c = vertices[nextj + i * outlineLength];
            triangles.push_back(std::make_shared<Triangle>(a, b, c));

            a = vertices[nextj + nexti * outlineLength];
            triangles.push_back(std::make_shared<Triangle>(b, a, c));
        }
    }

    // make middle triangles
    auto centerVertex = std::make_shared<IndexedVertex>(glm::vec3(0,0,0));
    vertices.push_back(centerVertex);

    for (int j = 0, nextj; j < outlineLength; j++)
    {
        nextj = j == outlineLength - 1 ? 0 : j + 1;
        a = vertices[j + (outlines.size() - 1) * outlineLength];
        b = centerVertex;
        c = vertices[nextj + (outlines.size() - 1) * outlineLength];
        triangles.push_back(std::make_shared<Triangle>(a, b, c));
    }
}

void CellMeshGenerator::CalculateNormals()
{
    glm::vec3 v1, v2, v3, norm;
    for (auto tri : triangles)
    {
        v1 = tri->a->vert.position;
        v2 = tri->b->vert.position;
        v3 = tri->c->vert.position;
        norm = glm::normalize(glm::cross((v2 - v1), (v3 - v1)));

        tri->a->vert.normal += norm;
        tri->b->vert.normal += norm;
        tri->c->vert.normal += norm;
    }

    for (auto v : vertices)
    {
        v->vert.normal = glm::normalize(v->vert.normal);
    }
}

void CellMeshGenerator::GenerateUV()
{
    // TODO
}
void CellMeshGenerator::CalculateTangents()
{
    // TODO
    // it requries normals & UV
}

std::shared_ptr<Mesh> CellMeshGenerator::BuildMesh()
{
    std::vector<Vertex> _vertices;
    std::vector<unsigned int> _indices;

    unsigned int vI = 0;
    for (auto v : vertices)
    {
        v->index = vI++;
        _vertices.push_back(v->vert);
    }

    for (auto tri : triangles)
    {
        _indices.push_back(tri->a->index);
        _indices.push_back(tri->b->index);
        _indices.push_back(tri->c->index);
    }

    return ModelManager::CreateMesh(_vertices, _indices);
}
