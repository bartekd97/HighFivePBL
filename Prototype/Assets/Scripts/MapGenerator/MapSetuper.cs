using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class MapSetuper : MonoBehaviour
{
    public GameObject playerObject;

    public void SetupMap()
    {
        MapCell startupCell = GetStartupCell();
        playerObject.transform.position = startupCell.transform.position;
    }

    MapCell GetStartupCell()
    {
        List<MapCell> tmp = new List<MapCell>(MapCell.All);
        tmp.Sort((a,b) => { return a.transform.position.x.CompareTo(b.transform.position.x); });
        return tmp[0];
    }
}
