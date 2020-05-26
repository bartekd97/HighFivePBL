using System.Collections;
using System.Collections.Generic;
using System.Collections.Specialized;
using System.Diagnostics;
using System.Linq;
using UnityEngine;

public class MapSetuper : MonoBehaviour
{
    public int minEnemiesInCell;
    public int maxEnemiesInCell;
    public int maxMonumentsCounter = 3;
    public int minObstaclesInCell = 0;
    public int maxObstaclesInCell = 4;
    public float centerSpawnRadiusPercentage;
    public int maxBossSpawnCellDistance;
    public GameObject playerObject;
    public Material bossCellMaterial;

    public GameObject[] structures;
    public GameObject[] obstacles;

    public GameObject enemyPrefab;
    public GameObject bossPrefab;
    public GameObject monumentPrefab;
    public GameObject mudPrefab;
    public GameObject fogPrefab;
    public GameObject toxicFogPrefab;
    public GameObject firePrefab;

    public GameObject pointPrefab;
    public GameObject pointPrefab2;
    public GameObject pointPrefab3;




    public void SetupMap()
    {
        MapCell startupCell = GetStartupCell();
        playerObject.transform.position = startupCell.transform.position;
        SetupMonuments();
        SetupObstacles();
        SetupEnemies();
        GameManager.Instance.SetCurrentCell(startupCell);
    }

    MapCell GetStartupCell()
    {
        List<MapCell> tmp = new List<MapCell>(MapCell.All);
        tmp.Sort((a, b) => { return a.transform.position.x.CompareTo(b.transform.position.x); });
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
        boss.GetComponent<EnemyController>().cellNumber = bossCell.CellSiteIndex;
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
                enemy.GetComponent<EnemyController>().cellNumber = cell.CellSiteIndex;

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

    private void GenerateMonumentInCell(MapCell cell)
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

    private void GenerateStructure(MapCell cell)
    {
        int structureNumber = Random.Range(0, structures.Length);

        int pointNumber = Random.Range(0, cell.Zones[0].Points.Count);
        Vector3 finalPoint = cell.Zones[0].Points[pointNumber];

        int iter_available = 200;

        while (Physics.OverlapBox(finalPoint, structures[structureNumber].GetComponent<BoxCollider>().size / 2.0f).Length > 1
            && iter_available > 0)
        {
            pointNumber = Random.Range(0, cell.Zones[0].Points.Count);
            finalPoint = cell.Zones[0].Points[pointNumber];
            iter_available = iter_available - 1;
        }

        if (Physics.OverlapBox(finalPoint, structures[structureNumber].GetComponent<BoxCollider>().size / 2.0f).Length <= 1)
        {
            Instantiate(structures[structureNumber], finalPoint, Quaternion.identity);
        }
        else
        {
            Instantiate(pointPrefab, finalPoint, Quaternion.identity);
            UnityEngine.Debug.Log(structures[structureNumber]);
            UnityEngine.Debug.Log(Physics.OverlapBox(finalPoint + new Vector3(0, structures[structureNumber].GetComponent<BoxCollider>().size.y, 0), structures[structureNumber].GetComponent<BoxCollider>().size));
        }
    }

    private void GenerateObstacle(MapCell cell, int i)
    {
        int obstacleType = Random.Range(0, obstacles.Length);

        int pointNumber = Random.Range(0, cell.Zones[i].Points.Count);
        Vector3 finalPoint = cell.Zones[i].Points[pointNumber];

        int iter_available = 200;

        while (Physics.OverlapBox(finalPoint, obstacles[obstacleType].GetComponent<BoxCollider>().size / 2.0f).Length > 1
            && iter_available > 0)
        {
            pointNumber = Random.Range(0, cell.Zones[i].Points.Count);
            finalPoint = cell.Zones[i].Points[pointNumber];
            iter_available = iter_available - 1;
        }

        if (Physics.OverlapBox(finalPoint, obstacles[obstacleType].GetComponent<BoxCollider>().size / 2.0f).Length <= 1)
        {
            Instantiate(obstacles[obstacleType], new Vector3(finalPoint.x, obstacles[obstacleType].transform.position.y, finalPoint.z), Quaternion.identity);
        }
        else
        {
            Instantiate(pointPrefab, finalPoint, Quaternion.identity);
            UnityEngine.Debug.Log(obstacles[obstacleType]);
            UnityEngine.Debug.Log(Physics.OverlapBox(finalPoint + new Vector3(0, obstacles[obstacleType].GetComponent<BoxCollider>().size.y, 0), obstacles[obstacleType].GetComponent<BoxCollider>().size));
        }
    }

    private void SpawnObstaclesInCell(MapCell cell)
    {
        //int obstaclesCount = Random.Range(minObstaclesInCell, maxObstaclesInCell);
        int obstacleType;

        SpawnPoints(cell);


        // Generate structure
        GenerateStructure(cell);

        if (cell.Zones[0].Points.Count > 400)
        {
            GenerateStructure(cell);
        }


        // Generate obstacles
        for (int i = 1; i < cell.Zones.Count; i++)
        {
            GenerateObstacle(cell, i);

            if (cell.Zones[i].Points.Count > 400)
            {
                GenerateObstacle(cell, i);
            }
        }




        //for (int i = 0; i < obstaclesCount; i++)
        //{
        //    float minRadius = Mathf.Infinity;
        //    for (int j = 0 ; j < cell.PolygonBase.Points.Length - 1; j++)
        //    {
        //        float distance = GetDistanceFromPointToLine(cell.PolygonBase.Points[j], cell.PolygonBase.Points[j + 1], new Vector2(0, 0));
        //        if (distance < minRadius) minRadius = distance;
        //    }
        //    obstacleType = Random.Range(0, 4);
        //    if (obstacleType == 0)
        //    {
        //        Vector2 circlePoint = (Random.insideUnitCircle * minRadius * centerSpawnRadiusPercentage);
        //        Vector3 finalPoint = cell.transform.position + new Vector3(circlePoint.x, 1f, circlePoint.y);
        //        int n = 0;
        //        int test = Physics.OverlapBox(finalPoint + new Vector3(0, fogPrefab.GetComponent<BoxCollider>().size.y, 0), fogPrefab.GetComponent<BoxCollider>().size / 2.0f).Length;
        //        while (Physics.OverlapBox(finalPoint + new Vector3(0, fogPrefab.GetComponent<BoxCollider>().size.y, 0), fogPrefab.GetComponent<BoxCollider>().size / 2.0f).Length > 1)
        //        {
        //            circlePoint = (Random.insideUnitCircle * minRadius * centerSpawnRadiusPercentage);
        //            finalPoint = cell.transform.position + new Vector3(circlePoint.x, 1f, circlePoint.y);
        //            n++;
        //        }
        //        Instantiate(fogPrefab, finalPoint, Quaternion.identity);
        //    }
        //    else if (obstacleType == 1)
        //    {
        //        Vector2 circlePoint = (Random.insideUnitCircle * minRadius * centerSpawnRadiusPercentage);
        //        Vector3 finalPoint = cell.transform.position + new Vector3(circlePoint.x, 1f, circlePoint.y);
        //        while (Physics.OverlapBox(finalPoint + new Vector3(0, toxicFogPrefab.GetComponent<BoxCollider>().size.y, 0), toxicFogPrefab.GetComponent<BoxCollider>().size / 2.0f).Length > 1)
        //        {
        //            circlePoint = (Random.insideUnitCircle * minRadius * centerSpawnRadiusPercentage);
        //            finalPoint = cell.transform.position + new Vector3(circlePoint.x, 1f, circlePoint.y);
        //        }
        //        Instantiate(toxicFogPrefab, finalPoint, Quaternion.identity);
        //    }
        //    else if (obstacleType == 2)
        //    {
        //        Vector2 circlePoint = (Random.insideUnitCircle * minRadius * centerSpawnRadiusPercentage);
        //        Vector3 finalPoint = cell.transform.position + new Vector3(circlePoint.x, 0.05f, circlePoint.y);
        //        while (Physics.OverlapBox(finalPoint + new Vector3(0, firePrefab.GetComponent<BoxCollider>().size.y, 0), firePrefab.GetComponent<BoxCollider>().size / 2.0f).Length > 1)
        //        {
        //            circlePoint = (Random.insideUnitCircle * minRadius * centerSpawnRadiusPercentage);
        //            finalPoint = cell.transform.position + new Vector3(circlePoint.x, 0.05f, circlePoint.y);
        //        }
        //        Instantiate(firePrefab, finalPoint, Quaternion.identity);
        //    }
        //    else
        //    {
        //        Vector2 circlePoint = (Random.insideUnitCircle * minRadius * centerSpawnRadiusPercentage);
        //        Vector3 finalPoint = cell.transform.position + new Vector3(circlePoint.x, 0.05f, circlePoint.y);
        //        while (Physics.OverlapBox(finalPoint + new Vector3(0, mudPrefab.GetComponent<BoxCollider>().size.y, 0), mudPrefab.GetComponent<BoxCollider>().size / 2.0f).Length > 1)
        //        {
        //            circlePoint = (Random.insideUnitCircle * minRadius * centerSpawnRadiusPercentage);
        //            finalPoint = cell.transform.position + new Vector3(circlePoint.x, 0.05f, circlePoint.y);
        //        }
        //        Instantiate(mudPrefab, finalPoint, Quaternion.identity);
        //    }
        //}


        //float minRadius = Mathf.Infinity;
        //for (int i = 0; i < cell.PolygonBase.Points.Length - 1; i++)
        //{
        //    float distance = GetDistanceFromPointToLine(cell.PolygonBase.Points[i], cell.PolygonBase.Points[i + 1], new Vector2(0, 0));
        //    if (distance < minRadius) minRadius = distance;
        //}
        //while (obstaclesCount > 0)
        //{
        //    Vector2 circlePoint = (Random.insideUnitCircle * minRadius * centerSpawnRadiusPercentage);
        //    Vector3 finalPoint = cell.transform.position + new Vector3(circlePoint.x, 1, circlePoint.y);
        //    if (Physics.OverlapBox(finalPoint + new Vector3(0, 2, 0),  2.0f).Length == 0)
        //    {
        //        Instantiate(toxicFogPrefab, finalPoint, Quaternion.identity);
        //        //GameObject enemy = InstantiateEnemy(finalPoint, cell);
        //        //cell.Enemies.Add(enemy);
        //        obstaclesCount -= 1;
        //    }
        //}
    }

    private void SpawnStructuresInCell(MapCell cell)
    {
        int structureNumber = Random.Range(0, structures.Length);
        float minRadius = Mathf.Infinity;
        for (int j = 0; j < cell.PolygonBase.Points.Length - 1; j++)
        {
            float distance = GetDistanceFromPointToLine(cell.PolygonBase.Points[j], cell.PolygonBase.Points[j + 1], new Vector2(0, 0));
            if (distance < minRadius) minRadius = distance;
        }
        Vector2 circlePoint = (Random.insideUnitCircle * minRadius * centerSpawnRadiusPercentage);
        Vector3 finalPoint = cell.transform.position + new Vector3(circlePoint.x, 0.00f, circlePoint.y);

        int test = Physics.OverlapBox(finalPoint + new Vector3(0, structures[structureNumber].GetComponent<BoxCollider>().size.y / 2.0f, 0), structures[structureNumber].GetComponent<BoxCollider>().size / 2.0f).Length;
        Vector3 test2 = structures[structureNumber].GetComponent<BoxCollider>().size;

        while (Physics.OverlapBox(finalPoint + new Vector3(0, structures[structureNumber].GetComponent<BoxCollider>().size.y, 0), structures[structureNumber].GetComponent<BoxCollider>().size / 2.0f).Length > 1)
        {
            circlePoint = (Random.insideUnitCircle * minRadius * centerSpawnRadiusPercentage);
            finalPoint = cell.transform.position + new Vector3(circlePoint.x, 0f, circlePoint.y);
        }
        //Instantiate(structures[structureNumber], finalPoint, Quaternion.identity);
    }

    void SetupObstacles()
    {
        List<MapCell> cells = new List<MapCell>(MapCell.All);
        //MapCell startupCell = GetStartupCell();
        // MapCell bossCell = GetRandomBossCell();
        cells
            //   .Where(c => !c.CellSiteIndex.Equals(startupCell.CellSiteIndex))
            //    .Where(c => !c.CellSiteIndex.Equals(bossCell.CellSiteIndex))
            .ToList()
            .ForEach(cell => SpawnStructuresInCell(cell));
        cells
        //    .Where(c => !c.CellSiteIndex.Equals(startupCell.CellSiteIndex))
            .ToList()
            .ForEach(cell => SpawnObstaclesInCell(cell));
    }



    private float getDistanceBetweenPointAndSegment(float pointX, float pointY, float x1, float y1, float x2, float y2)
    {
        float A = pointX - x1;
        float B = pointY - y1;
        float C = x2 - x1;
        float D = y2 - y1;

        float point = A * C + B * D;
        float lenSquare = C * C + D * D;
        float parameter = point / lenSquare;

        float shortestX;
        float shortestY;

        if (parameter < 0)
        {
            shortestX = x1;
            shortestY = y1;
        }
        else if (parameter > 1)
        {
            shortestX = x2;
            shortestY = y2;
        }
        else
        {
            shortestX = x1 + parameter * C;
            shortestY = y1 + parameter * D;
        }

        float distance = Mathf.Sqrt((pointX - shortestX) * (pointX - shortestX) + (pointY - shortestY) * (pointY - shortestY));
        return distance;
    }

    private void SpawnPoints(MapCell cell)
    {
        // Sort bridge points by angle
        foreach (MapCell.BridgeTo bridge in cell.Bridges)
        {
            float bridgeX = bridge.Gate.transform.position.x;
            float bridgeY = bridge.Gate.transform.position.z;
            bridge.angle = Mathf.Atan2(bridge.Gate.transform.position.x - cell.transform.position.x, bridge.Gate.transform.position.z - cell.transform.position.z);
        }
        cell.Bridges.Sort(cell.SortByAngle);


        //Generate points
        float startX = cell.transform.position.x - 25;
        float startY = cell.transform.position.z - 25;

        for (float i = 0; i < 50; i = i + 1)
        {
            for (float j = 0; j < 50; j = j + 1)
            {
                if (cell.PolygonSmoothInner.GetEdgeCenterRatio(new Vector2(i - 25, j - 25)) < 0.75 && cell.PolygonSmoothInner.GetEdgeCenterRatio(new Vector2(i - 25, j - 25)) > 0.2)
                {
                    cell.generationPoints.Add(new Vector3(startX + i, 0, startY + j));
                }
            }
        }


        // Delete points on roads
        float centerX = cell.transform.position.x;
        float centerY = cell.transform.position.z;

        foreach (MapCell.BridgeTo bridge in cell.Bridges)
        {
            float bridgeX = bridge.Gate.transform.position.x;
            float bridgeY = bridge.Gate.transform.position.z;
            for (int i = 0; i < cell.generationPoints.Count; i++)
            {
                if (getDistanceBetweenPointAndSegment(cell.generationPoints[i].x, cell.generationPoints[i].z,
                    centerX, centerY, bridgeX, bridgeY) < 3.0f)
                {
                    cell.generationPoints.RemoveAt(i);
                    i = i - 1;
                }
            }
        }

        // Group point in zones
        for (int i = 0; i < cell.Bridges.Count; i++)
        {
            cell.Zones.Add(new MapCell.Zone());
            for (int j = 0; j < cell.generationPoints.Count; j++)
            {
                if (i == cell.Bridges.Count - 1)
                {
                    cell.Zones[i].Points.Add(cell.generationPoints[j]);
                    cell.generationPoints.RemoveAt(j);
                    j = j - 1;

                }
                else
                {
                    if (Mathf.Atan2(cell.generationPoints[j].x - cell.transform.position.x, cell.generationPoints[j].z - cell.transform.position.z) > cell.Bridges[i].angle
                        && Mathf.Atan2(cell.generationPoints[j].x - cell.transform.position.x, cell.generationPoints[j].z - cell.transform.position.z) < cell.Bridges[i + 1].angle)
                    {
                        cell.Zones[i].Points.Add(cell.generationPoints[j]);
                        cell.generationPoints.RemoveAt(j);
                        j = j - 1;

                    }
                }
            }
            //UnityEngine.Debug.Log(cell.Zones[i].Points.Count) ;

        }
        UnityEngine.Debug.Log("->");

        // Generation remaining points
        //for (int i = 0; i < cell.generationPoints.Count; i++)
        //{
        //    Instantiate(pointPrefab, cell.generationPoints[i], Quaternion.identity);
        //}
    }
}
