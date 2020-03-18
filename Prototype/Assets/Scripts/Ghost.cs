using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Ghost : MonoBehaviour
{
    public float damageToEnemies = 5.0f;


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
        public Vector2 from;
        public Vector2 to;
        public List<GhostCrossing> crosings = new List<GhostCrossing>();
        public List<MiniGhost> ghosts;
    }

    public GameObject miniGhostPrefab;
    public float miniGhostSpawnDistance = 1.5f;
    public int maxActiveLines = 4;

    List<GhostLine> activeLines = new List<GhostLine>();

    Vector2 startPosition;
    Vector3 lastMiniGhostSpawnPosition;

    List<MiniGhost> spawnedMiniGhostsCurrent;

    public bool IsMarking { get; private set; }
    private void Awake()
    {
        IsMarking = false;
    }
    private void Start()
    {
        Hide();
    }

    private void Update()
    {
        if (IsMarking)
        {
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
        }
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

        startPosition = new Vector2(
                transform.position.x,
                transform.position.z
            );
        lastMiniGhostSpawnPosition = transform.position;
        spawnedMiniGhostsCurrent = new List<MiniGhost>();
        IsMarking = true;
    }

    public void EndMarking()
    {
        if (!IsMarking) return;

        Vector2 endPosition = new Vector2(
                transform.position.x,
                transform.position.z
            );

        if (spawnedMiniGhostsCurrent.Count > 0)
            activeLines.Add(new GhostLine()
            {
                from = startPosition,
                to = endPosition,
                ghosts = spawnedMiniGhostsCurrent
            });

        spawnedMiniGhostsCurrent = null;

        IsMarking = false;

        UpdateLineCrossings();
        CheckClosedLines();

        if (activeLines.Count == maxActiveLines)
            while (activeLines.Count > 0)
                FadeOutLine(activeLines[0]);
    }

    void UpdateLineCrossings()
    {
        foreach (var l1 in activeLines)
        {
            foreach (var l2 in activeLines)
            {
                if (l1 == l2) continue;
                var crossing = CalculateCrossing(l1, l2);
                if (crossing == null) continue;
                if (l1.crosings.Exists(c => c.Equals(crossing))) continue;
                l1.crosings.Add(crossing);
                l2.crosings.Add(crossing);
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

        if (figureCrossing.Count == possibleLines.Count)
        {
            AttackWithClosedFigure(possibleLines, figureCrossing);
            return;
        }
    }

    void FadeOutLine(GhostLine line)
    {
        line.ghosts.ForEach(g => g.FadeOut());
        activeLines.Remove(line);
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
            activeLines.Remove(line);
        }
    }

    GhostCrossing CalculateCrossing(GhostLine l1, GhostLine l2)
    {
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
    }

    private void OnTriggerEnter(Collider other)
    {
        if (!IsMarking)
            return;

        if (!other.gameObject.CompareTag("Enemy"))
            return;

        EnemyController ec = other.GetComponent<EnemyController>();
        if (ec != null)
            ec.TakeDamage(damageToEnemies);
    }
}
