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
    }
    public List<BridgeTo> Bridges = new List<BridgeTo>();

    public List<GameObject> Enemies = new List<GameObject>();


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
