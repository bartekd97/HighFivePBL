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

    // it's called between Awake and Start
    public void Generate(ConvexPolygon originalPolygon, CellMeshConfig config)
    {
        originalPolygon = originalPolygon.ScaledBy(config.mainScale);

        GameObject meshObject = new GameObject(cell.CellSiteIndex.ToString());
        meshObject.transform.parent = transform;
        meshObject.transform.position = transform.position;

        MeshRenderer renderer = meshObject.AddComponent<MeshRenderer>();
        renderer.material = new Material(Shader.Find("Lightweight Render Pipeline/Lit"));

        CellMeshGenerator generator = new CellMeshGenerator(config, originalPolygon);
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
}
