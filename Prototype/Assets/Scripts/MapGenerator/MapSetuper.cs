using System.Collections;
using System.Collections.Generic;
using System.Linq;
using UnityEngine;

public class MapSetuper : MonoBehaviour
{
    public int minEnemiesInCell;
    public int maxEnemiesInCell;
    public int maxMonumentsCounter = 3;
    public float centerSpawnRadiusPercentage;
    public int maxBossSpawnCellDistance;
    public GameObject playerObject;
    public Material bossCellMaterial;

    public GameObject enemyPrefab;
    public GameObject bossPrefab;
    public GameObject monumentPrefab;

    public void SetupMap()
    {
        MapCell startupCell = GetStartupCell();
        playerObject.transform.position = startupCell.transform.position;
        SetupEnemies();
        SetupMonuments();
        GameManager.Instance.SetCurrentCell(startupCell);
    }

    MapCell GetStartupCell()
    {
        List<MapCell> tmp = new List<MapCell>(MapCell.All);
        tmp.Sort((a,b) => { return a.transform.position.x.CompareTo(b.transform.position.x); });
        return tmp[0];
    }

    MapCell GetRandomBossCell()
    {
        List<MapCell> tmp = new List<MapCell>(MapCell.All);
        tmp.Sort((a, b) => { return b.transform.position.x.CompareTo(a.transform.position.x); });
        return tmp[Mathf.Min(Random.Range(0, maxBossSpawnCellDistance), tmp.Count - 1)];
    }

    void SetupEnemies()
    {
        List<MapCell> cells = new List<MapCell>(MapCell.All);
        MapCell startupCell = GetStartupCell();
        MapCell bossCell = GetRandomBossCell();
        cells
            .Where(c => !c.CellSiteIndex.Equals(startupCell.CellSiteIndex))
            .Where(c => !c.CellSiteIndex.Equals(bossCell.CellSiteIndex))
            .ToList()
            .ForEach(cell => SpawnEnemiesInCell(cell));

        GameObject boss = InstantiateBoss(bossCell.transform.position);
        bossCell.Enemies.Add(boss);

        bossCell.GetComponentInChildren<MeshRenderer>().material = bossCellMaterial;
    }

    private void SpawnEnemiesInCell(MapCell cell)
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
            Vector2 circlePoint = (Random.insideUnitCircle * minRadius * centerSpawnRadiusPercentage);
            Vector3 finalPoint = cell.transform.position + new Vector3(circlePoint.x, 0, circlePoint.y);
            if (Physics.OverlapBox(finalPoint + new Vector3(0, enemyPrefab.GetComponent<BoxCollider>().size.y, 0), enemyPrefab.GetComponent<BoxCollider>().size / 2.0f).Length == 0)
            {
                GameObject enemy = InstantiateEnemy(finalPoint, cell);
                cell.Enemies.Add(enemy);
                enemiesCount -= 1;
            }
        }
    }

    private GameObject InstantiateEnemy(Vector3 position, MapCell cell)
    {
        GameObject enemy = Instantiate(enemyPrefab, position, Quaternion.identity);
        var enemyController = enemy.GetComponent<EnemyController>();
        enemyController.player = playerObject;
        enemyController.charController = playerObject.GetComponent<CharController>();
        return enemy;
    }

    private GameObject InstantiateBoss(Vector3 position)
    {
        GameObject boss = Instantiate(bossPrefab, position, Quaternion.identity);
        var bossController = boss.GetComponent<EnemyController>();
        bossController.player = playerObject;
        bossController.charController = playerObject.GetComponent<CharController>();
        return boss;
    }

    private float GetDistanceFromPointToLine(Vector2 lineA, Vector2 lineB, Vector2 C)
    {
        return Mathf.Abs((C.x - lineA.x) * (-lineB.y + lineA.y) + (C.y - lineA.y) * (lineB.x - lineA.x)) / Mathf.Sqrt(Mathf.Pow(-lineB.y + lineA.y, 2) + Mathf.Pow(lineB.x - lineA.x, 2));
    }

    private void GenerateMonumentInCell (MapCell cell)
    {
        Vector3 monumentPosition = cell.transform.position;
        GameObject monument = Instantiate(monumentPrefab, cell.transform.position, Quaternion.identity);
        //var monumentController = monument.GetComponent<MonumentController>();

        cell.Monuments.Add(monument);
    }

    void SetupMonuments()
    {
        List<MapCell> cells = new List<MapCell>(MapCell.All);
        
        MapCell startupCell = GetStartupCell();
        MapCell bossCell = GetRandomBossCell();

        //Debug.Log("start:" + startupCell.CellSiteIndex);
        //Debug.Log("boss:" + bossCell.CellSiteIndex);
        cells.ToList();
        
        cells.Sort((p, q) => p.CellSiteIndex.CompareTo(q.CellSiteIndex));

        cells.Remove(startupCell);
        cells.Remove(bossCell);

        int[] cellIndexes = new int[cells.Count];

        for (int i = 0; i < cells.Count - 1; i++)
        {
            cellIndexes[i] = cells[i].CellSiteIndex;
        }

        for (int i = 0; i < maxMonumentsCounter; i++)
        {
            int index = Random.Range(0, cellIndexes.Length - 1);
            int x = cellIndexes[index];


            for (int j = 0; j < cells.Count - 1; j++)
            {
                if (cells[j].CellSiteIndex == x)
                {
                    GenerateMonumentInCell(cells[j]);
                }
            }
            cellIndexes = RemoveIndices(cellIndexes, index);
        }
    }

    private int[] RemoveIndices(int[] IndicesArray, int RemoveAt)
    {
        int[] newIndicesArray = new int[IndicesArray.Length - 1];

        int i = 0;
        int j = 0;
        while (i < IndicesArray.Length)
        {
            if (i != RemoveAt)
            {
                newIndicesArray[j] = IndicesArray[i];
                j++;
            }

            i++;
        }

        return newIndicesArray;
    }

}
