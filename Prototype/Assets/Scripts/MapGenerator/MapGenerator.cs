using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.Events;
using csDelaunay;

public class MapGenerator : MonoBehaviour
{
    public DiagramLayout layout;
    //public MapFence fence;
    public CellMeshConfig cellMeshConfig;
    public CellFenceConfig cellFenceConfig;

    public GameObject bridgePrefab;
    public float minEdgeLengthForBridge = 5.0f;

    // This is where we will store the resulting data
    private Dictionary<Vector2f, Site> sites;
    private List<Edge> edges;
    private Rectf bounds;


    public UnityEvent OnMapGenerated;

    private void Start()
    {
        //fence.Initialize();

        bounds = layout.GetBounds();
        Voronoi voronoi = new Voronoi(layout.CreatePoints(), bounds);
        voronoi.LloydRelaxation(layout.LloydRelaxIteraions);

        sites = voronoi.SitesIndexedByLocation;
        edges = voronoi.Edges;

        GameObject cellContainer = new GameObject("Cells");
        cellContainer.transform.parent = transform;

        // prepare cells
        List<MapCell> cells = new List<MapCell>();

        foreach (var s in sites.Values)
            if (!IsBorderCell(s))
                cells.Add(CreateCell(s, cellContainer));

        // create bridges and connect references
        GameObject bridgeContainer = new GameObject("Bridges");
        bridgeContainer.transform.parent = transform;
        CreateBridges(cells, bridgeContainer);

        // now generate real cells
        foreach (var s in sites.Values)
        {
            if (!IsBorderCell(s))
            {
                MapCell cell = cells.Find(c => c.CellSiteIndex == s.SiteIndex);
                ConvexPolygon cellPolygon = new ConvexPolygon(s.Region(bounds), s.Coord);
                CellGenerator generator = cell.gameObject.AddComponent<CellGenerator>();
                generator.Generate(
                    cellPolygon,
                    cellMeshConfig,
                    cellFenceConfig
                );
            }
        }

        /*
        CreateTerrain();
        CreateEdges();
        CreateVertices();
        */


        StartCoroutine(coTriggerOnMapGenerated());
    }

    IEnumerator coTriggerOnMapGenerated()
    {
        yield return new WaitForEndOfFrame();
        OnMapGenerated.Invoke();
    }

    MapCell CreateCell(Site cell, GameObject parent)
    {
        GameObject cellObject = new GameObject(cell.SiteIndex.ToString());
        cellObject.transform.parent = parent.transform;
        cellObject.transform.position = new Vector3(cell.Coord.x, 0, cell.Coord.y);

        MapCell mCell = cellObject.AddComponent<MapCell>();
        mCell.CellSiteIndex = cell.SiteIndex;
        return mCell;
    }

    void CreateBridges(List<MapCell> cells, GameObject parent)
    {
        System.Func<Site, MapCell> GetCell = (s) =>
        {
            foreach (var cell in cells)
                if (cell.CellSiteIndex == s.SiteIndex)
                    return cell;
            return null;
        };

        foreach (var edge in edges)
        {
            if (!edge.Visible())
                continue;

            Vector2f leftVertex = edge.ClippedEnds[LR.LEFT];
            Vector2f rightVertex = edge.ClippedEnds[LR.RIGHT];

            if (leftVertex.DistanceSquare(rightVertex) < minEdgeLengthForBridge * minEdgeLengthForBridge)
                continue;

            var cellA = GetCell(edge.LeftSite);
            var cellB = GetCell(edge.RightSite);
            if (cellA == null || cellB == null)
                continue;

            Vector3 position = new Vector3(
                    (leftVertex.x + rightVertex.x) * 0.5f,
                    0.0f,
                    (leftVertex.y + rightVertex.y) * 0.5f
                );

            GameObject bridgeObject = Instantiate(bridgePrefab, parent.transform);
            bridgeObject.transform.position = position;
            bridgeObject.transform.right = (new Vector3(rightVertex.x, 0.0f, rightVertex.y) - position).normalized;
            CellBridge bridge = bridgeObject.GetComponent<CellBridge>();

            bridge.CellA = cellA;
            bridge.CellB = cellB;

            cellA.Bridges.Add(new MapCell.BridgeTo() { Bridge = bridge, Cell = cellB });
            cellB.Bridges.Add(new MapCell.BridgeTo() { Bridge = bridge, Cell = cellA });
        }
    }

    /*
    void CreateCell(Site cell, GameObject parent)
    {
        ConvexPolygon cellPolygon = new ConvexPolygon(cell.Region(layout.GetBounds()), cell.Coord);
        cellPolygon = cellPolygon.ScaledBy(cellMeshConfig.mainScale);

        GameObject cellObject = new GameObject(cell.SiteIndex.ToString());
        cellObject.transform.parent = parent.transform;
        cellObject.transform.position = new Vector3(cell.Coord.x, 0, cell.Coord.y);

        MeshRenderer renderer = cellObject.AddComponent<MeshRenderer>();
        renderer.material = new Material(Shader.Find("Lightweight Render Pipeline/Lit"));

        MeshFilter filter = cellObject.AddComponent<MeshFilter>();
        CellMeshGenerator generator = new CellMeshGenerator(cellMeshConfig, cellPolygon);
        generator.PrepareOutlines();
        generator.PrepareBaseMeshStructure();
        filter.mesh = generator.BuildMesh();
    }
    */

    bool IsBorderCell(Site cell)
    {
        foreach (var p in cell.Region(layout.GetBounds()))
            if (p.x == 0 || p.x == layout.width || p.y == 0 || p.y == layout.height)
                return true;
        return false;
    }

    /*
    void CreateTerrain()
    {
        GameObject terrainObject = new GameObject("Terrain");
        terrainObject.transform.parent = transform;

        TerrainData data = new TerrainData();
        data.size = new Vector3(layout.width, 1, layout.height);
        data.baseMapResolution = 64;
        data.terrainLayers = new TerrainLayer[] {
            new TerrainLayer()
        };
        data.terrainLayers[0].tileSize = new Vector2(20, 20);
        float[,,] alphamap = new float[data.alphamapResolution, data.alphamapResolution,1];
        for (int x = 0; x < data.alphamapResolution; x++)
            for (int y = 0; y < data.alphamapResolution; y++)
                alphamap[x, y, 0] = 1.0f;
        data.SetAlphamaps(0, 0, alphamap);

        Terrain terrain = terrainObject.AddComponent<Terrain>();
        terrain.terrainData = data;
        terrain.materialTemplate = new Material(Shader.Find("Lightweight Render Pipeline/Terrain/Lit"));

        terrainObject.AddComponent<TerrainCollider>().terrainData = data;
    }
    void CreateEdges()
    {
        GameObject edgesContainer = new GameObject("Edges");
        edgesContainer.transform.parent = transform;


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
        }
    }

    void CreateVertices()
    {
        GameObject verticesContainer = new GameObject("Vertices");
        verticesContainer.transform.parent = transform;

        List<Vector2f> spawnedVertices = new List<Vector2f>();

        foreach (Edge edge in edges)
        {
            if (!edge.Visible())
                continue;

            Vector2f leftVertex = edge.ClippedEnds[LR.LEFT];
            Vector2f rightVertex = edge.ClippedEnds[LR.RIGHT];

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
    */
}
