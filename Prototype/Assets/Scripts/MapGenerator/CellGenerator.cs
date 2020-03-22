using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class CellGenerator : MonoBehaviour
{
    MapCell cell;

    private void Awake()
    {
        cell = GetComponent<MapCell>();
    }

    private void Start()
    {
        Destroy(this); // we dont need generator anymore
    }

    ConvexPolygon originalPolygon;
    CellMeshConfig meshConfig;
    CellFenceConfig fenceConfig;

    // it's called between Awake and Start
    public void Generate(
        ConvexPolygon originalPolygon,
        CellMeshConfig meshConfig,
        CellFenceConfig fenceConfig
        )
    {
        this.originalPolygon = originalPolygon.ScaledBy(meshConfig.mainScale);
        this.meshConfig = meshConfig;
        this.fenceConfig = fenceConfig;

        GenerateMesh();
        GenerateFence();
    }

    void GenerateMesh()
    {
        GameObject meshObject = new GameObject(cell.CellSiteIndex.ToString());
        meshObject.transform.parent = transform;
        meshObject.transform.position = transform.position;

        MeshRenderer renderer = meshObject.AddComponent<MeshRenderer>();
        renderer.material = new Material(Shader.Find("Lightweight Render Pipeline/Lit"));

        CellMeshGenerator generator = new CellMeshGenerator(meshConfig, originalPolygon);
        generator.PrepareOutlines();
        generator.PrepareBaseMeshStructure();
        Mesh mesh = generator.BuildMesh();

        MeshFilter filter = meshObject.AddComponent<MeshFilter>();
        filter.sharedMesh = mesh;
        MeshCollider collider = meshObject.AddComponent<MeshCollider>();
        collider.sharedMesh = mesh;

        // assign polygon structures
        cell.PolygonBase = generator.PolygonBase;
        cell.PolygonBaseInner = generator.PolygonBaseInner;
        cell.PolygonSmooth = generator.PolygonSmooth;
        cell.PolygonSmoothInner = generator.PolygonSmoothInner;
    }

    void GenerateFence()
    {
        CellFenceGenerator generator = new CellFenceGenerator(
            fenceConfig,
            cell.PolygonSmoothInner.ScaledBy(fenceConfig.innerLevelFenceLocation)
        );


        GameObject gateContainer = new GameObject("Gates");
        gateContainer.transform.parent = transform;
        gateContainer.transform.position = transform.position;

        foreach (MapCell.BridgeTo bridgeTo in cell.Bridges)
        {
            GameObject gateObject = generator.CreateGate(bridgeTo.Bridge.gameObject, gateContainer);
            CellGate gate = gateObject.GetComponent<CellGate>();
            gate.Cell = cell;
            gate.Bridge = bridgeTo.Bridge;
            bridgeTo.Gate = gate;
            if (cell == bridgeTo.Bridge.CellA)
                bridgeTo.Bridge.GateA = gate;
            else if (cell == bridgeTo.Bridge.CellB)
                bridgeTo.Bridge.GateB = gate;
        }

        GameObject fenceContainer = new GameObject("Fence");
        fenceContainer.transform.parent = transform;
        fenceContainer.transform.position = transform.position;

        generator.PrepareBrokenCurves();
        generator.CreateSections();
        generator.TryFillGapsInSections();
        generator.PrepareObjects();
        generator.BuildSections(fenceContainer);

        //generator.PrepareOutlineSections();
        //generator.PrepareObjects();
        //generator.Build(fenceContainer);
    }
}
