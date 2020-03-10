using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using csDelaunay;

public class MapGenerator : MonoBehaviour
{
    public DiagramLayout layout;
    public MapFence fence;

    // This is where we will store the resulting data
    private Dictionary<Vector2f, Site> sites;
    private List<Edge> edges;
    private Rectf bounds;

    private void Start()
    {
        fence.Initialize();


        bounds = layout.GetBounds();
        Voronoi voronoi = new Voronoi(layout.CreatePoints(), bounds);
        voronoi.LloydRelaxation(2);

        sites = voronoi.SitesIndexedByLocation;
        edges = voronoi.Edges;

        GameObject edgesContainer = new GameObject("Edges");
        edgesContainer.transform.parent = transform;
        GameObject verticesContainer = new GameObject("Vertices");
        verticesContainer.transform.parent = transform;

        List<Vector2f> spawnedVertices = new List<Vector2f>();

        foreach (Edge edge in edges)
        {
            if (!edge.Visible())
                continue;

            GameObject edgeGroup = new GameObject(edge.EdgeIndex.ToString());
            edgeGroup.transform.parent = edgesContainer.transform;

            Vector2f leftVertex = edge.ClippedEnds[LR.LEFT];
            Vector2f rightVertex = edge.ClippedEnds[LR.RIGHT];

            fence.CreateEdgeFence(
                    new Vector3(leftVertex.x, 0, leftVertex.y),
                    new Vector3(rightVertex.x, 0, rightVertex.y),
                    edgeGroup
                );


            if (spawnedVertices.Find(v => v.DistanceSquare(leftVertex) < 0.01f).magnitude == 0)
            {
                fence.CreateVertex(new Vector3(leftVertex.x, 0, leftVertex.y), verticesContainer);
                spawnedVertices.Add(leftVertex);
            }

            if (spawnedVertices.Find(v => v.DistanceSquare(rightVertex) < 0.01f).magnitude == 0)
            {
                fence.CreateVertex(new Vector3(rightVertex.x, 0, rightVertex.y), verticesContainer);
                spawnedVertices.Add(rightVertex);
            }

        }
    }
}
