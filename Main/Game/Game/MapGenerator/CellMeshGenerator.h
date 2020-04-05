#pragma once

#include <vector>
#include <glm/glm.hpp>
#include "Config.h"
#include "ConvexPolygon.h"
#include "Resourcing/Mesh.h"

class CellMeshGenerator
{
public:
    ConvexPolygon PolygonBase;
    ConvexPolygon PolygonBaseInner;
    ConvexPolygon PolygonSmooth;
    ConvexPolygon PolygonSmoothInner;

private:
    class IndexedVertex
    {
    public:
        unsigned int index = 0;
        Vertex vert;
        IndexedVertex(glm::vec3 position) { vert.position = position; }
    };
    std::vector<std::shared_ptr<IndexedVertex>> vertices;

    class Triangle
    {
    public:
        std::shared_ptr<IndexedVertex> a, b, c;
        Triangle(std::shared_ptr<IndexedVertex> a,
            std::shared_ptr<IndexedVertex> b,
            std::shared_ptr<IndexedVertex> c)
            : a(a), b(b), c(c) {}
    };
    std::vector<std::shared_ptr<Triangle>> triangles;

    CellMeshConfig config;
    ConvexPolygon originalPolygon;
    ConvexPolygon baseOutline;

    std::vector<ConvexPolygon> outlines;

public:
    CellMeshGenerator(CellMeshConfig& config, ConvexPolygon& polygon)
        : config(config), originalPolygon(polygon),
        PolygonBase(polygon),
        PolygonBaseInner(polygon.ShellScaledBy(config.innerScale))
    {}

    void PrepareOutlines();
    void PrepareBaseMeshStructure();

    void CalculateNormals();
    void GenerateUV();
    void CalculateTangents();

    std::shared_ptr<Mesh> BuildMesh();
};

