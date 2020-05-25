using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class MapCell : MonoBehaviour
{
    public static List<MapCell> All { get; private set; }
    static MapCell() { All = new List<MapCell>(); }

    public class BridgeTo
    {
        public CellBridge Bridge;
        public MapCell Cell;
        public CellGate Gate;
        public float angle;
    }

    public int SortByAngle(BridgeTo a, BridgeTo b)
    {
        if(a.angle < b.angle)
        {
            return -1;
        }
        else if (a.angle > b.angle)
        {
            return 1;
        }
        return 0;
    }

    public class Zone
    {
        public List<Vector3> Points = new List<Vector3>();
    }

    public List<BridgeTo> Bridges = new List<BridgeTo>();

    public List<Zone> Zones = new List<Zone>();

    public List<GameObject> Enemies = new List<GameObject>();

    public List<GameObject> Monuments = new List<GameObject>();

    public List<Vector3> generationPoints = new List<Vector3>();



    [HideInInspector]
    public int CellSiteIndex;


    // probably those polygon wont be needed, maybe one or two
    // it needs testing
    // some visualisation of them would be cool
    // (maybe with onDrawGizmo in unity scene?)
    //
    // Those polygons are in local space! (with 0,0 at center, not real cell position in x,y)
    public ConvexPolygon PolygonBase;
    public ConvexPolygon PolygonBaseInner;
    public ConvexPolygon PolygonSmooth;
    public ConvexPolygon PolygonSmoothInner;


    private void Awake()
    {
        All.Add(this);

    }
    private void OnDestroy()
    {
        All.Remove(this);
    }
}
