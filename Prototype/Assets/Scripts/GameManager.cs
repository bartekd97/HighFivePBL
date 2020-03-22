using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class GameManager : MonoBehaviour
{
    public static GameManager Instance { get; private set; }

    public MapCell currentCell;
    public GameObject playerObject;
    //List<MapCell> cells;

    //bool isCellClear;

    private void Awake()
    {
        Instance = this;
    }

    public void SetCurrentCell(MapCell cell)
    {
        currentCell = cell;
        if (!IsCellClear(currentCell))
        {
            CloseGates(currentCell);
        }
    }

    bool IsCellClear(MapCell cell)
    {
        bool isClear = true;
        foreach (GameObject e in cell.Enemies)
        {
            if (!e.Equals(null))
            {
                isClear = false;
                break;
            }
        }
        return isClear;
    }

    void Update()
    {
        if (currentCell != null)
        {
            //SetCurrentCell();

            /*
            if (currentCell.Enemies.Count == 0)
            {
                isCellClear = true;
            }

            foreach (GameObject e in currentCell.Enemies)
            {
                isCellClear = true;
                if (!e.Equals(null))
                {
                    isCellClear = false;
                    break;
                }
            }
            */

            if (IsCellClear(currentCell))
            {
                //ActivateBridges();
                OpenGates(currentCell);
            }

        }

    }

    void OpenGates(MapCell cell)
    {
        foreach (MapCell.BridgeTo b in cell.Bridges)
        {
            b.Gate.Open();
        }
    }

    void CloseGates(MapCell cell)
    {
        foreach (MapCell.BridgeTo b in cell.Bridges)
        {
            b.Gate.Close();
        }
    }

    /*
    // Start is called before the first frame update
    public void GameSetup()
    {
        cells = new List<MapCell>(MapCell.All);
    }


    void SetCurrentCell()
    {
        MapCell oldCurrentCell = currentCell;
        foreach (MapCell c in cells)
        {
            float cx = c.transform.position.x - playerObject.transform.position.x;
            float cz = c.transform.position.z - playerObject.transform.position.z; 
            if (c.PolygonSmoothInner.GetEdgeCenterRatio(new Vector2(cx, cz)) < 0.8)
            {
                currentCell = c;
                if (currentCell != oldCurrentCell)
                {
                    DisableBridges();
                }
                break;
            }
        }
    }

    void DisableBridges()
    {
        foreach(MapCell.BridgeTo b in  currentCell.Bridges)
        {
            b.Bridge.gameObject.SetActive(false);
        }
    }

    void ActivateBridges()
    {
        foreach (MapCell.BridgeTo b in currentCell.Bridges)
        {
            b.Bridge.gameObject.SetActive(true);
        }
    }
    */

    //private float GetDistanceFromPointToLine(Vector3 p,  Vector3 c)
    //{
    //    return Mathf.Abs((c.x - p.x)  + (c.z- p.z)  / Mathf.Sqrt(Mathf.Pow(-lineB.y + lineA.y, 2) + Mathf.Pow(lineB.x - lineA.x, 2));
    //}
}
