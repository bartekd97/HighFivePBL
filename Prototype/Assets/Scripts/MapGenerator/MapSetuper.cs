using System.Collections;
using System.Collections.Generic;
using System.Linq;
using UnityEngine;

public class MapSetuper : MonoBehaviour
{
    public int minEnemiesInCell;
    public int maxEnemiesInCell;
    public float centerSpawnRadiusPercentage;
    public GameObject playerObject;

    public GameObject enemyPrefab;

    public void SetupMap()
    {
        MapCell startupCell = GetStartupCell();
        playerObject.transform.position = startupCell.transform.position;
        SetupEnemies();
    }

    MapCell GetStartupCell()
    {
        List<MapCell> tmp = new List<MapCell>(MapCell.All);
        tmp.Sort((a,b) => { return a.transform.position.x.CompareTo(b.transform.position.x); });
        return tmp[0];
    }

    void SetupEnemies()
    {
        List<MapCell> cells = new List<MapCell>(MapCell.All);
        MapCell startupCell = GetStartupCell();
        cells.Where(c => !c.CellSiteIndex.Equals(startupCell.CellSiteIndex)).ToList().ForEach(cell =>
        {
            int enemiesCount = Random.Range(minEnemiesInCell, maxEnemiesInCell);
            float minRadius = Mathf.Infinity;
            for (int i = 0; i < cell.PolygonBase.Points.Length - 1; i++)
            {
                float distance = GetDistanceFromPointToLine(cell.PolygonBase.Points[i], cell.PolygonBase.Points[i + 1], new Vector2(0, 0));
                if (distance < minRadius) minRadius = distance;
            }
            while (enemiesCount > 0)
            {
                Vector2 point = (Random.insideUnitCircle * minRadius * centerSpawnRadiusPercentage);
                InstantiateEnemy(cell.transform.position + new Vector3(point.x, 0, point.y));
                enemiesCount -= 1;
            }
        });
    }

    private GameObject InstantiateEnemy(Vector3 position)
    {
        GameObject enemy = Instantiate(enemyPrefab, position, Quaternion.identity);
        var enemyController = enemy.GetComponent<EnemyController>();
        enemyController.player = playerObject;
        enemyController.charController = playerObject.GetComponent<CharController>();
        return enemy;
    }

    private float GetDistanceFromPointToLine(Vector2 lineA, Vector2 lineB, Vector2 C)
    {
        return Mathf.Abs((C.x - lineA.x) * (-lineB.y + lineA.y) + (C.y - lineA.y) * (lineB.x - lineA.x)) / Mathf.Sqrt(Mathf.Pow(-lineB.y + lineA.y, 2) + Mathf.Pow(lineB.x - lineA.x, 2));
    }
}
