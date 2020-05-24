using System.Collections;
using System.Collections.Generic;
using TMPro;
using UnityEngine;

public class Ghost : MonoBehaviour
{
    public float damageToEnemies = 5.0f;
    [SerializeField]
    public GameObject lineGeneratorPrefab;

    public class GhostCrossing : System.IEquatable<GhostCrossing>
    {
        public Vector2 position;
        public GhostLine a;
        public GhostLine b;

        public bool Equals(GhostCrossing other)
        {
            return Vector2.Distance(position, other.position) < Vector2.kEpsilon;
        }
    }
    public class GhostLine
    {
        //public Vector2 from;
        //public Vector2 to;
        public List<Vector2> points;
        public List<GhostCrossing> crosings = new List<GhostCrossing>();
        public List<MiniGhost> ghosts;
        public GameObject lineRend;
    }

    public GameObject recordedLine;
    List<Vector3> recordedLinePositions;

    public GameObject miniGhostPrefab;
    public float miniGhostSpawnDistance = 1.5f;
    public int maxActiveLines = 4;

    public List<GhostLine> activeLines = new List<GhostLine>();

    List<Vector2> recordedPositions;
    float distanceReached;
    Vector3 lastDistanceRecordPos;

    //Vector3 lastMiniGhostSpawnPosition;

    List<MiniGhost> spawnedMiniGhostsCurrent;

    public EnemyController enemyController;
    public bool firstEnemyHit;
    public int numberOfEnemyToHit;
    public int numberOfEnemyHit;

    public MonumentController monumentController;
    public float lineSlow = 0.0f;
    public float ghostFreezeTime = 0.0f;
    public float dotTick = 0.0f;
    public float dotDmg = 0.0f;

    private float lastDotTick = 0.0f;

    public ParticleSystem sparkCrossing;

    public BossSpawnerController bossController;
    public bool bossHit;

    public bool IsMarking { get; private set; }
    private void Awake()
    {
        IsMarking = false;
        firstEnemyHit = false;
        numberOfEnemyToHit = 1;
        numberOfEnemyHit = 1;
        bossHit = false;
    }
    private void Start()
    {
        Hide();
    }

    private void Update()
    {
        if (IsMarking)
        {
            distanceReached += Vector3.Distance(lastDistanceRecordPos, transform.position);
            lastDistanceRecordPos = transform.position;
            if (distanceReached >= miniGhostSpawnDistance)
            {
                MiniGhost mg = Instantiate(
                    miniGhostPrefab,
                    transform.position,
                    transform.rotation
                    ).GetComponent<MiniGhost>();
                spawnedMiniGhostsCurrent.Add(mg);
                recordedPositions.Add(new Vector2(
                    transform.position.x,
                    transform.position.z
                ));
                recordedLinePositions.Add(new Vector3(
                transform.position.x,
                transform.position.y,
                transform.position.z
                ));
                UpdateLineRenderer(recordedLinePositions);
                distanceReached = 0.0f;
            }
            /*
            if (Vector3.Distance(lastMiniGhostSpawnPosition, transform.position)
                >= miniGhostSpawnDistance)
            {
                MiniGhost mg = Instantiate(
                    miniGhostPrefab,
                    transform.position,
                    transform.rotation
                    ).GetComponent<MiniGhost>();
                spawnedMiniGhostsCurrent.Add(mg);
                lastMiniGhostSpawnPosition = transform.position;
            }
            */
        }

        List<GameObject> enemies = new List<GameObject>(GameObject.FindGameObjectsWithTag("Enemy"));
        enemies.ForEach(enemy => enemy.GetComponent<EnemyController>().slow = 0.0f);

        bool isTimeToStrikexD = false;
        if (dotDmg > 0 && dotTick > 0)
        {
            if ((Time.time - lastDotTick) >= dotTick)
            {
                lastDotTick = Time.time;
                isTimeToStrikexD = true;
            }
        }

        //Slow + DoT
        activeLines.ForEach(line =>
        {
            line.ghosts.ForEach(ghost =>
            {
                BoxCollider coll = ghost.GetComponentInParent<BoxCollider>();
                enemies.ForEach(enemy =>
                {
                    BoxCollider enemyColl = enemy.GetComponent<BoxCollider>();
                    if (coll.bounds.Intersects(enemyColl.bounds))
                    {
                        enemy.GetComponent<EnemyController>().slow = lineSlow;
                        if (isTimeToStrikexD) enemy.GetComponent<EnemyController>().TakeDamage(dotDmg);
                    }
                });
            });
        });

        //Freeze
        enemies.ForEach(enemy =>
        {
            BoxCollider enemyColl = enemy.GetComponent<BoxCollider>();
            if (enemyColl.bounds.Intersects(GetComponentInParent<BoxCollider>().bounds))
            {
                enemy.GetComponent<EnemyController>().frozenTo = Time.time + ghostFreezeTime;
            }
        });

        //boss
        List<GameObject> bosses = new List<GameObject>(GameObject.FindGameObjectsWithTag("Boss"));
        bosses.ForEach(enemy => enemy.GetComponent<BossSpawnerController>().slow = 0.0f);

        bool isTimeToStrikeBoss = false;
        if (dotDmg > 0 && dotTick > 0)
        {
            if ((Time.time - lastDotTick) >= dotTick)
            {
                lastDotTick = Time.time;
                isTimeToStrikeBoss = true;
            }
        }

        //Slow + DoT (boss)
        activeLines.ForEach(line =>
        {
            line.ghosts.ForEach(ghost =>
            {
                BoxCollider coll = ghost.GetComponentInParent<BoxCollider>();
                bosses.ForEach(boss =>
                {
                    BoxCollider bossColl = boss.GetComponent<BoxCollider>();
                    if (coll.bounds.Intersects(bossColl.bounds))
                    {
                        boss.GetComponent<EnemyController>().slow = lineSlow;
                        if (isTimeToStrikeBoss) boss.GetComponent<BossSpawnerController>().TakeDamage(dotDmg);
                    }
                });
            });
        });

        //Freeze (boss)
        bosses.ForEach(enemy =>
        {
            BoxCollider bossColl = enemy.GetComponent<BoxCollider>();
            if (bossColl.bounds.Intersects(GetComponentInParent<BoxCollider>().bounds))
            {
                enemy.GetComponent<BossSpawnerController>().frozenTo = Time.time + ghostFreezeTime;
            }
        });
    }

    public void Show(Transform start)
    {
        transform.position = start.position;
    }
    public void Hide()
    {
        transform.position = new Vector3(0, -100, 0);
    }

    public void StartMarking()
    {
        if (IsMarking) return;
        /*
        startPosition = new Vector2(
                transform.position.x,
                transform.position.z
            );
            */
        recordedPositions = new List<Vector2>();
        recordedLinePositions = new List<Vector3>();
        recordedPositions.Add(new Vector2(
                transform.position.x,
                transform.position.z
            ));
        recordedLinePositions.Add(new Vector3(
                transform.position.x,
                transform.position.y,
                transform.position.z
            ));
        recordedLine = SpawnLineGenerator(recordedLinePositions);
        //lastMiniGhostSpawnPosition = transform.position;
        distanceReached = 0.0f;
        lastDistanceRecordPos = transform.position;
        spawnedMiniGhostsCurrent = new List<MiniGhost>();
        IsMarking = true;
        firstEnemyHit = true;
        bossHit = true;
        numberOfEnemyHit = 0;
    }

    public void EndMarking()
    {
        if (!IsMarking) return;
        /*
        Vector2 endPosition = new Vector2(
                transform.position.x,
                transform.position.z
            );
            */
        recordedPositions.Add(new Vector2(
            transform.position.x,
            transform.position.z
        ));
        recordedLinePositions.Add(new Vector3(
                transform.position.x,
                transform.position.y,
                transform.position.z
            ));
        UpdateLineRenderer(recordedLinePositions);

        if (spawnedMiniGhostsCurrent.Count > 0)
        {
            activeLines.Add(new GhostLine()
            {
                //from = startPosition,
                //to = endPosition,
                points = recordedPositions,
                ghosts = spawnedMiniGhostsCurrent,
                lineRend = recordedLine

        });
        }

        //SpawnLineGenerator(recordedLinePositions);
        recordedLine = null;
        recordedPositions = null;
        recordedLinePositions = null;
        spawnedMiniGhostsCurrent = null;

        IsMarking = false;

        UpdateLineCrossings();
        CheckClosedLines();

        if (activeLines.Count > maxActiveLines)
        {
            //while (activeLines.Count > 0)
            FadeOutLine(activeLines[0]);
        }
    }

    void UpdateLineCrossings()
    {
        foreach (var l1 in activeLines)
        {
            foreach (var l2 in activeLines)
            {
                if (l1 == l2) continue;
                var crossings = CalculateCrossings(l1, l2);
                //if (crossing == null) continue;
                foreach (var crossing in crossings)
                {
                    if (l1.crosings.Exists(c => c.Equals(crossing))) continue;
                    l1.crosings.Add(crossing);
                    l2.crosings.Add(crossing);
                }
            }
        }
    }

    void CheckClosedLines()
    {
        List<GhostLine> possibleLines = new List<GhostLine>();
        foreach (GhostLine l in activeLines)
            if (l.crosings.Count >= 2)
                possibleLines.Add(l);

        List<GhostCrossing> figureCrossing = new List<GhostCrossing>();
        foreach (GhostLine l in activeLines)
            foreach (GhostCrossing c in l.crosings)
                if (possibleLines.Contains(c.a) && possibleLines.Contains(c.b))
                    if (!figureCrossing.Exists(fc => fc.Equals(c)))
                        figureCrossing.Add(c);

        if (figureCrossing.Count >= possibleLines.Count)
        {
            AttackWithClosedFigure(possibleLines, figureCrossing);
            return;
        }
    }

    void FadeOutLine(GhostLine ghostLine)
    {
        ghostLine.ghosts.ForEach(g => g.FadeOut());
        Destroy(ghostLine.lineRend);
        activeLines.Remove(ghostLine);
    }

    void AttackWithClosedFigure(List<GhostLine> lines, List<GhostCrossing> crossings)
    {
        Vector2 center = Vector2.zero;
        foreach (GhostCrossing c in crossings)
            center += c.position;
        center /= (float)crossings.Count;

        Vector3 center3 = new Vector3(center.x, 0, center.y);
        foreach(var line in lines)
        {
            line.ghosts.ForEach(g => g.DoAttack(center3));
            Destroy(line.lineRend);
            activeLines.Remove(line);
            var clones = GameObject.FindGameObjectsWithTag("Spark");
            foreach (var clone in clones)
            {
                Destroy(clone);
            }
        }

    }

    List<GhostCrossing> CalculateCrossings(GhostLine l1, GhostLine l2)
    {
        List<GhostCrossing> crossing = new List<GhostCrossing>();
        for (int i = 0; i < l1.points.Count - 1; i++)
        {
            for (int j = 0; j < l2.points.Count - 1; j++)
            {
                Vector2 p = GetSegmentsCommonPoint(
                        l1.points[i], l1.points[i + 1],
                        l2.points[j], l2.points[j + 1]
                    );
                if (p != Vector2.zero) {
                    crossing.Add(new GhostCrossing()
                    {
                        a = l1,
                        b = l2,
                        position = p
                    });
                    Vector3 sparkPosition = new Vector3(p.x, 0.4f, p.y);
                    Instantiate(sparkCrossing, sparkPosition, Quaternion.identity);
                }
            }
        }
        return crossing;
        /*
        float
            dXab = l1.to.x - l1.from.x,
            dYab = l1.to.y - l1.from.y,

            dXac = l2.from.x - l1.from.x,
            dYac = l2.from.y - l1.from.y,

            dXcd = l2.to.x - l2.from.x,
            dYcd = l2.to.y - l2.from.y;

        float t1 = (dXac * dYcd - dYac * dXcd) / (dXab * dYcd - dYab * dXcd);
        float t2 = (dXac * dYab - dYac * dXab) / (dXab * dYcd - dYab * dXcd);

        if (t1 < 0 || t1 > 1 || t2 < 0 || t2 > 1)
            return null;

        Vector2 p = Vector2.Lerp(l1.from, l1.to, t1);
        return new GhostCrossing()
        {
            a = l1,
            b = l2,
            position = p
        };
        */
    }
    Vector2 GetSegmentsCommonPoint(Vector2 a, Vector2 b, Vector2 c, Vector2 d)
    {
        float
            dXab = b.x - a.x,
            dYab = b.y - a.y,

            dXac = c.x - a.x,
            dYac = c.y - a.y,

            dXcd = d.x - c.x,
            dYcd = d.y - c.y;

        float t1 = (dXac * dYcd - dYac * dXcd) / (dXab * dYcd - dYab * dXcd);
        float t2 = (dXac * dYab - dYac * dXab) / (dXab * dYcd - dYab * dXcd);

        if (t1 < 0 || t1 > 1 || t2 < 0 || t2 > 1)
            return Vector2.zero;

        return Vector2.Lerp(a, b, t1);
    }

    private void OnTriggerEnter(Collider other)
    {
        if (!IsMarking)
            return;

        if (!other.gameObject.CompareTag("Enemy") && !other.gameObject.CompareTag("Monument") && !other.gameObject.CompareTag("Boss"))
            return;

        enemyController = other.GetComponent<EnemyController>();
        if (enemyController != null && numberOfEnemyHit < numberOfEnemyToHit)
        {
            enemyController.TakeDamage(damageToEnemies);
            numberOfEnemyHit++;
            firstEnemyHit = false;
        }
        
        monumentController = other.GetComponent<MonumentController>();
        if (monumentController != null)
        {
            monumentController.ApplyDamageToMonument(damageToEnemies);
        }

        bossController = other.GetComponent<BossSpawnerController>();
        if (bossController != null && bossHit == true)
        {
            bossController.TakeDamage(damageToEnemies);
            bossHit = false;
        }
    }

    private GameObject SpawnLineGenerator(List<Vector3> linePoints)
    {
        Vector3[] linePointsV = linePoints.ToArray();
        GameObject newLineGen = Instantiate(lineGeneratorPrefab);
        LineRenderer IRend = newLineGen.GetComponent<LineRenderer>();

        IRend.positionCount = linePointsV.Length;
        IRend.SetPositions(linePointsV);
        return newLineGen;
        // Destroy(newLineGen);
    }

    private void UpdateLineRenderer(List<Vector3> linePoints)
    {
        Vector3[] linePointsV = linePoints.ToArray();
        LineRenderer IRend = recordedLine.GetComponent<LineRenderer>();

        IRend.positionCount = linePointsV.Length;
        IRend.SetPositions(linePointsV);
    }

    string output;

    public override string ToString()
    {
        output = "";
        output += "<component name=\"ScriptComponent\">";
        output += "<property name=\"name\" value=\"" + this.name + "\"/>";
        output += "</component>";
        return output;
    }
}