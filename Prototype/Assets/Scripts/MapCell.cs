using System.Collections;
using System.Collections.Generic;
using System.Diagnostics;
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

    public class Grid
    {
        float startX;
        float startY;
        private int width;
        private int height;
        public PathNode[,] gridArray;
        public Grid(int width, int height, float startX, float startY)
        {
            this.width = width;
            this.height = height;
            this.startX = startX;
            this.startY = startY;

            gridArray = new PathNode[width, height];
            for (int i = 0; i < 50; i++)
            {
                for (int j = 0; j < 50; j++)
                {
                    gridArray[i, j] = new PathNode();
                    gridArray[i,j].x = startX + (float)i;
                    gridArray[i,j].y = startY + (float)j;
                    gridArray[i, j].indX = i;
                    gridArray[i, j].indY = j;
                    //if(cell.PolygonSmoothInner.GetEdgeCenterRatio(new Vector2(gridArray[i, j].x, gridArray[i, j].y)) < 0.75)
                    //{
                    //    gridArray[i, j].isAvailable = false;
                    //}
                }
            }
        }

        public Vector3 GetPosition(int width, int height)
        {
            return new Vector3(gridArray[width, height].x , 0 , gridArray[width, height].y);
        }

        public PathNode GetNode(float startX, float startY)
        {
            Vector3 startVector = new Vector3(startX, 0, startY);
            PathNode closestNode = gridArray[0, 0];
            for (int i = 0; i < 50; i++)
            {
                for (int j = 0; j < 50; j++)
                {
                    if (Vector3.Distance(startVector, new Vector3(closestNode.x, 0, closestNode.y)) >
                        Vector3.Distance(startVector, new Vector3(gridArray[i, j].x, 0, gridArray[i, j].y)))
                    {
                        closestNode = gridArray[i, j];
                    }
                }
            }
            return closestNode;
        }
    }

    public class PathNode
    {
        public float x;
        public float y;
        public int indX;
        public int indY;

        public int gCost;
        public int hCost;
        public int fCost;

        public bool isAvailable = true;

        public PathNode cameFromNode;

        public void CalculateFCost()
        {
            fCost = gCost + hCost;
        }

        public override string ToString()
        {
            return x + "," + y;
        }
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
    public List<Vector3> pathPoints = new List<Vector3>();
    public Grid grid;



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
