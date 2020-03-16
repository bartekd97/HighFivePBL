using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class MapCell : MonoBehaviour
{
    public class BridgeTo
    {
        public CellBridge Bridge;
        public MapCell Cell;
    }
    public List<BridgeTo> Bridges = new List<BridgeTo>();

    [HideInInspector]
    public int CellSiteIndex;


    public ConvexPolygon PolygonBase;
    public ConvexPolygon PolygonBaseInner;
    public ConvexPolygon PolygonSmooth;
    public ConvexPolygon PolygonSmoothInner;
}
