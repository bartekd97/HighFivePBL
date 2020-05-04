
#include <stb_perlin.h>
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
                float pn = stb_perlin_noise3(
                    vpos.x * config.noiseScale,
                    vpos.y * config.noiseScale,
                    0.0f,
                    0,0,0
                    );
                /*
                float pn = Mathf.PerlinNoise(
                    vpos.x * config.noiseScale,
                    vpos.y * config.noiseScale
                    );
                    */
                //pn = 0.5f;
                //pn = (pn * 2.0f - 1.0f);
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

CellMeshGenerator::UVData CellMeshGenerator::GenerateUV()
{
    // TODO: maybe better generation? more proportional, according to Y value?
    glm::vec2 uvmin = { 0,0 }, uvmax = { 0,0 };
    for (auto v : vertices)
    {
        v->vert.uv = { v->vert.position.x, v->vert.position.z };
        uvmin.x = glm::min(uvmin.x, v->vert.uv.x);
        uvmin.y = glm::min(uvmin.y, v->vert.uv.y);
        uvmax.x = glm::max(uvmax.x, v->vert.uv.x);
        uvmax.y = glm::max(uvmax.y, v->vert.uv.y);
    }
    // normalize
    glm::vec2 range = uvmax - uvmin;
    for (auto v : vertices)
    {
        v->vert.uv -= uvmin;
        v->vert.uv /= range;
        v->vert.uv.y = 1.0f - v->vert.uv.y; // flip y for openGL
    }

    // generate output data
    std::vector<glm::vec2> uvPolygonPoints;
    uvPolygonPoints.reserve(PolygonSmooth.Points.size());
    for (auto p : PolygonSmooth.Points)
    {
        glm::vec2 uvp = (p - uvmin) / range;
        uvp.y = 1.0f - uvp.y; // flip y for openGL
        uvPolygonPoints.push_back(uvp);
    }
    
    CellMeshGenerator::UVData data;
    data.uvPolygon = ConvexPolygon(uvPolygonPoints);
    data.uvCenter = (-uvmin) / range;
    data.uvCenter.y = 1.0f - data.uvCenter.y; // flip y for openGL
    data.offset = uvmin;
    data.range = range;
    return data;
}
// tangent calculation code taken from Assimp aiProcess_CalcTangentSpace code
void CellMeshGenerator::CalculateTangents()
{
    Vertex *p1, *p2, *p3;
    glm::vec3 v, w;
    glm::vec3 tangent, bitangent;
    glm::vec3 localTangent, localBitangent;
    for (auto tri : triangles)
    {
        p1 = &tri->a->vert;
        p2 = &tri->b->vert;
        p3 = &tri->c->vert;

        // position differences p1->p2 and p1->p3
        v = p2->position - p1->position, w = p3->position - p1->position;

        // texture offset p1->p2 and p1->p3
        float sx = p2->uv.x - p1->uv.x, sy = p2->uv.y - p1->uv.y;
        float tx = p3->uv.x - p1->uv.x, ty = p3->uv.y - p1->uv.y;
        float dirCorrection = (tx * sy - ty * sx) < 0.0f ? -1.0f : 1.0f;
        // when t1, t2, t3 in same position in UV space, just use default UV direction.
        if (sx * ty == sy * tx) {
            sx = 0.0; sy = 1.0;
            tx = 1.0; ty = 0.0;
        }

        // tangent points in the direction where to positive X axis of the texture coord's would point in model space
        // bitangent's points along the positive Y axis of the texture coord's, respectively
        tangent.x = (w.x * sy - v.x * ty) * dirCorrection;
        tangent.y = (w.y * sy - v.y * ty) * dirCorrection;
        tangent.z = (w.z * sy - v.z * ty) * dirCorrection;
        bitangent.x = (w.x * sx - v.x * tx) * dirCorrection;
        bitangent.y = (w.y * sx - v.y * tx) * dirCorrection;
        bitangent.z = (w.z * sx - v.z * tx) * dirCorrection;

        // store for every vertex of that face
        for (auto p : { p1,p2,p3 })
        {
            // project tangent and bitangent into the plane formed by the vertex' normal
            localTangent = tangent - p->normal * (tangent * p->normal);
            localBitangent = bitangent - p->normal * (bitangent * p->normal);

            localTangent = glm::normalize(localTangent);
            localBitangent = glm::normalize(localBitangent);

            // and write it into the mesh.
            p->tangent = localTangent;
            p->bitangent = localBitangent;
        }
    }
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
