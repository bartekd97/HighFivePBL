using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class CellMeshGenerator
{
    class Vertex
    {
        public int index;
        public Vector3 position;
        public Vertex(Vector3 position)
        {
            this.position = position;
        }
        public Vertex(float x, float y, float z) : this(new Vector3(x,y,z))
        {}
    }
    List<Vertex> vertices;

    class Triangle
    {
        public Vertex a, b, c;
        public Triangle(Vertex a, Vertex b, Vertex c)
        {
            this.a = a;
            this.b = b;
            this.c = c;
        }
    }
    List<Triangle> triangles;


    CellMeshConfig config;
    ConvexPolygon originalPolygon;
    ConvexPolygon baseOutline;

    List<ConvexPolygon> outlines;

    public CellMeshGenerator(CellMeshConfig config, ConvexPolygon polygon)
    {
        this.config = config;
        this.originalPolygon = polygon;
    }

    public void PrepareOutlines()
    {
        baseOutline = originalPolygon;
        for (int i = 0; i < config.vertexBevelSteps; i++)
            baseOutline = baseOutline.BeveledVerticesBy(config.vertexBevel);

        baseOutline = baseOutline.CreateCircular(config.circularSegments);

        outlines = new List<ConvexPolygon>();
        int outlinesCount = config.outlineSplits + 1; // + outer
        float scale = 1.0f;
        float scaleFactor = config.nextOutlineScale;
        for (int i = 0; i < outlinesCount; i++)
        {
            outlines.Add(
                baseOutline.ScaledBy(scale)
            );

            scale *= scaleFactor;
            scaleFactor *= config.nextOutlineScaleMultiplier;
        }
    }

    public void PrepareBaseMeshStructure()
    {
        vertices = new List<Vertex>();
        triangles = new List<Triangle>();

        for (int i = 0; i < outlines.Count; i++)
        {
            //float innerLevel = (float)i / (float)(outlines.Count - 1);
            //float depthLevel = (1.0f - Mathf.Sin(innerLevel * Mathf.PI * 0.5f));
            //float depthLevel = 1.0f - Mathf.Sqrt(innerLevel);
            //float depthLevel = 1.0f - (1 + (--innerLevel) * innerLevel * innerLevel * innerLevel * innerLevel);
            foreach (var vpos in outlines[i].Points)
            {
                float coRatio = baseOutline.GetEdgeCenterRatio(vpos);

                float innerLevel = Mathf.Clamp01((1.0f - coRatio) / (1.0f - config.innerScale));
                float depthLevel = (1.0f - Mathf.Sin(innerLevel * Mathf.PI * 0.5f));

                float noisex = 0.0f, noisey = 0.0f, noisez = 0.0f;
                float noiseLevel = 1.0f - Mathf.Clamp01((1.0f - coRatio) / (1.0f - config.noiseCenterRatio));
                if (noiseLevel > 0.0f)
                {
                    float pn = Mathf.PerlinNoise(
                        vpos.x * config.noiseScale,
                        vpos.y * config.noiseScale
                        );
                    pn = (pn * 2.0f - 1.0f);
                    pn = pn * noiseLevel * innerLevel;

                    Vector2 noiseDir = vpos.normalized;
                    noisex = noiseDir.x * pn * config.noiseForce.x;
                    noisez = noiseDir.y * pn * config.noiseForce.x;

                    noisey = pn * config.noiseForce.y;
                }

                vertices.Add(new Vertex(
                        noisex + vpos.x,
                        noisey + (i == 0 ? -config.depthMax : -config.depth) * depthLevel,
                        noisez + vpos.y
                    ));
            }
        }

        Vertex a, b, c;
        // make outline triangles
        int outlineLength = outlines[0].Points.Length;
        for (int i = 0, nexti; i < outlines.Count - 1; i++)
        {
            nexti = i + 1;
            for (int j = 0, nextj; j < outlineLength; j++)
            {
                nextj = j == outlineLength - 1 ? 0 : j + 1;
                a = vertices[j + i * outlineLength];
                b = vertices[j + nexti * outlineLength];
                c = vertices[nextj + i * outlineLength];
                triangles.Add(new Triangle(a,b,c));

                a = vertices[nextj + nexti * outlineLength];
                triangles.Add(new Triangle(b, a, c));
            }
        }

        // make middle triangles
        Vertex centerVertex = new Vertex(0, 0, 0);
        vertices.Add(centerVertex);

        for (int j = 0, nextj; j < outlineLength; j++)
        {
            nextj = j == outlineLength - 1 ? 0 : j + 1;
            a = vertices[j + (outlines.Count - 1) * outlineLength];
            b = centerVertex;
            c = vertices[nextj + (outlines.Count - 1) * outlineLength];
            triangles.Add(new Triangle(a, b, c));
        }
    }

    public Mesh BuildMesh()
    {
        // update vertex indices
        for (int i = 0; i < vertices.Count; i++)
            vertices[i].index = i;

        // create structs
        Vector3[] _vertices = new Vector3[vertices.Count];
        int[] _triangles = new int[triangles.Count * 3];

        // fill them
        for (int i = 0; i < vertices.Count; i++)
            _vertices[i] = vertices[i].position;
        for (int i = 0; i < triangles.Count; i++)
        {
            _triangles[i * 3] = triangles[i].a.index;
            _triangles[i * 3 + 1] = triangles[i].b.index;
            _triangles[i * 3 + 2] = triangles[i].c.index;
        }

        // make mesh
        Mesh mesh = new Mesh();
        mesh.vertices = _vertices;
        mesh.triangles = _triangles;
        mesh.RecalculateNormals();
        return mesh;
    }
}
