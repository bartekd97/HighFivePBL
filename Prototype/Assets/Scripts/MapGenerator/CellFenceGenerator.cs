using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using FenceEntity = CellFenceConfig.FenceEntity;

public class CellFenceGenerator
{
    /*
    class FenceSection
    {
        public Vector2 position;
        public Vector2 direction;
        public bool withEnd = false;
    }
    List<FenceSection> fenceSections;
    */

    class FenceObject
    {
        public Vector2 position;
        public Vector2 direction;
        public FenceEntity entity;
    }
    //List<FenceObject> fenceObjects;

    class Gate : System.IComparable<Gate>
    {
        public Vector2 position;
        public Vector2 direction;

        public int CompareTo(Gate other)
        {
            return Vector2.SignedAngle(Vector2.up, position).CompareTo(
                Vector2.SignedAngle(Vector2.up, other.position)
                );
        }
    }
    List<Gate> gates;

    class Segment
    {
        public Vector2 position;
        public Vector2 direction;
        public int fragmentCount;
        public float fragmentLength;

        public Vector2 endPosition { get { return position + direction * fragmentLength * fragmentCount; } }
    }
    class BrokenCurve
    {
        public List<Vector2> points;
        public List<Vector2> reversedPoints;

        public List<Segment> forawrdSegments = new List<Segment>();
        public List<Segment> backwardSegments = new List<Segment>();

        public int[] targetPointI = new int[] { 1 };
        public int[] targetReversedPointI = new int[] { 1 };

        public List<FenceObject> objects;
        public List<FenceObject> filler;

        public float HoleDistance()
        {
            Vector2 from = points[0];
            if (forawrdSegments.Count > 0)
                from = forawrdSegments[forawrdSegments.Count - 1].endPosition;

            Vector2 to = reversedPoints[0];
            if (backwardSegments.Count > 0)
                to = backwardSegments[backwardSegments.Count - 1].endPosition;

            return (from - to).magnitude;
        }
    }
    List<BrokenCurve> brokenCurves;

    CellFenceConfig config;
    ConvexPolygon polygon;

    public CellFenceGenerator(CellFenceConfig config, ConvexPolygon polygon)
    {
        this.config = config;
        this.polygon = polygon;
        gates = new List<Gate>();
    }

    public GameObject CreateGate(GameObject bridge, GameObject parent)
    {
        Vector3 offsetPosition = bridge.transform.position - parent.transform.position;
        Vector2 bridgePosition = new Vector2(
            offsetPosition.x,
            offsetPosition.z
            );
        Vector2 frontDirection = new Vector3(
            bridge.transform.forward.x,
            bridge.transform.forward.z
            ).normalized;
        Vector2 gateDirection = new Vector3(
            bridge.transform.right.x,
            bridge.transform.right.z
            ).normalized;

        if (bridgePosition.magnitude < (bridgePosition + frontDirection).magnitude)
        {
            frontDirection = -frontDirection;
            gateDirection = -gateDirection;
        }

        Vector2 gatePosition = bridgePosition + frontDirection * config.gateDistance;

        GameObject element = GameObject.Instantiate(config.gateEntity.gameObject);
        element.transform.parent = parent.transform;
        element.transform.localPosition = new Vector3(
            gatePosition.x,
            0.0f,
            gatePosition.y
            );
        element.transform.right = new Vector3(
            gateDirection.x,
            0.0f,
            gateDirection.y
            );

        gates.Add(new Gate() { position = gatePosition, direction = gateDirection });

        return element;
    }

    public void PrepareBrokenCurves()
    {
        brokenCurves = new List<BrokenCurve>();
        gates.Sort();

        List<Vector2> outline = new List<Vector2>();
        outline.AddRange(polygon.Points);
        outline.AddRange(polygon.Points); // make it overlap for easier indexing

        Gate fromGate, toGate;
        Vector2 startPoint, endPoint;
        for (int i=0; i<gates.Count; i++)
        {
            fromGate = gates[i];
            toGate = gates[i == gates.Count - 1 ? 0 : i + 1];

            startPoint = fromGate.position + fromGate.direction * config.gateEntity.length * 0.5f;
            endPoint = toGate.position - toGate.direction * config.gateEntity.length * 0.5f;

            int fromIndex = 0, toIndex = 0;
            for (int j=0; j<polygon.Points.Length; j++)
            {
                if ((startPoint - outline[j]).magnitude
                    < (startPoint - outline[fromIndex]).magnitude)
                    fromIndex = j;
                if ((endPoint - outline[j]).magnitude
                    < (endPoint - outline[toIndex]).magnitude)
                    toIndex = j;
            }

            if (toIndex < fromIndex)
                toIndex += polygon.Points.Length;

            if (toIndex <= 2)
            {
                fromIndex += polygon.Points.Length;
                toIndex += polygon.Points.Length;
            }

            
            if (Vector2.Dot(
                (outline[fromIndex + 1] - outline[fromIndex]).normalized,
                (startPoint - outline[fromIndex]).normalized
                ) > -0.2f)
                fromIndex++;
            if (Vector2.Dot(
                (outline[toIndex - 1] - outline[toIndex]).normalized,
                (endPoint - outline[toIndex]).normalized
                ) > -0.2f)
                toIndex--;

            if (Vector2.Dot(
                (outline[fromIndex + 1] - outline[fromIndex]).normalized,
                (startPoint - outline[fromIndex]).normalized
                ) > -0.2f)
                fromIndex++;
            if (Vector2.Dot(
                (outline[toIndex - 1] - outline[toIndex]).normalized,
                (endPoint - outline[toIndex]).normalized
                ) > -0.2f)
                toIndex--;


            List<Vector2> curve = new List<Vector2>();
            curve.Add(startPoint);
            for (int k = fromIndex; k <= toIndex; k++)
                curve.Add(outline[k]);
            curve.Add(endPoint);

            List<Vector2> reversedCurve = new List<Vector2>(curve);
            reversedCurve.Reverse();

            brokenCurves.Add(new BrokenCurve()
            {
                points = curve,
                reversedPoints = reversedCurve
            });
        }
    }

    public void CreateSections()
    {
        float lengthForNextSegment = config.fragmentEntity.length * config.fragmentCount + config.connectorEntity.length;
        float minHoleGap = lengthForNextSegment + config.connectorEntity.length;

        List<Vector2> targetPoints;
        List<Segment> targetSegments;
        int[] targetPointI;

        Vector2 nextPosition;
        Vector2 direction;
        Segment lastSegment;

        foreach (var curve in brokenCurves)
        {
            int i = 0;
            int created = 0;
            while (curve.HoleDistance() > minHoleGap || created < 2 )
            {
                if (i % 2 == 0)
                {
                    targetPoints = curve.points;
                    targetSegments = curve.forawrdSegments;
                    targetPointI = curve.targetPointI;
                }
                else
                {
                    targetPoints = curve.reversedPoints;
                    targetSegments = curve.backwardSegments;
                    targetPointI = curve.targetReversedPointI;
                }

                nextPosition = targetPoints[0];
                if (targetSegments.Count > 0)
                {
                    lastSegment = targetSegments[targetSegments.Count - 1];
                    nextPosition = lastSegment.position + lastSegment.direction * lengthForNextSegment;
                }

                direction = targetPoints[targetPointI[0]] - nextPosition;
                if (direction.magnitude >= lengthForNextSegment)
                {
                    targetSegments.Add(new Segment()
                    {
                        position = nextPosition,
                        direction = direction.normalized,
                        fragmentCount = config.fragmentCount,
                        fragmentLength = config.fragmentEntity.length
                    });
                    created++;
                }
                else if (targetPointI[0] < targetPoints.Count - 1)
                {
                    targetPointI[0]++;
                }

                i++;
            }
        }

    }

    public void TryFillGapsInSections()
    {
        Segment forward, backward;
        foreach (var curve in brokenCurves)
        {
            forward = curve.forawrdSegments[curve.forawrdSegments.Count - 1];
            backward = curve.backwardSegments[curve.backwardSegments.Count - 1];
            curve.filler = TryFillGap(forward, backward);
        }
    }
    List<FenceObject> TryFillGap(Segment forward, Segment backward)
    {
        List<FenceObject> filler = new List<FenceObject>();

        FenceEntity fragment = config.fragmentEntity;
        FenceEntity connector = config.connectorEntity;
        float conLen2 = connector.length * 2.0f;

        Vector2 forwardDirFrom = forward.direction;
        Vector2 forwardDirTo = (backward.position - forward.position).normalized;
        Vector2 backwarddDirFrom = backward.direction;
        Vector2 backwardDirTo = (forward.position - backward.position).normalized;

        for (float f = 1.0f; f >= 0.0f; f -= 0.2f)
        {
            forward.direction = Vector2.Lerp(
                    forwardDirFrom, forwardDirTo, f
                ).normalized;
            backward.direction = Vector2.Lerp(
                    backwarddDirFrom, backwardDirTo, f
                ).normalized;

            float gap = (forward.endPosition - backward.endPosition).magnitude;
            Vector2 dir = (backward.endPosition - forward.endPosition).normalized;

            // maybe just 1 connector?
            if (connector.InFillRange(gap))
            {
                filler.Add(new FenceObject()
                {
                    position = forward.endPosition + dir * gap * 0.5f,
                    direction = dir,
                    entity = connector
                });
                return filler;
            }

            // maybe 2 connectors + fragment?
            if (fragment.InFillRange(gap - conLen2))
            {
                float fragLen = gap - conLen2;
                filler.Add(new FenceObject()
                {
                    position = forward.endPosition + dir * connector.length * 0.5f,
                    direction = dir,
                    entity = connector
                });
                filler.Add(new FenceObject()
                {
                    position = forward.endPosition + dir * (connector.length + fragLen * 0.5f),
                    direction = dir,
                    entity = fragment
                });
                filler.Add(new FenceObject()
                {
                    position = forward.endPosition + dir * (connector.length * 1.5f + fragLen),
                    direction = dir,
                    entity = connector
                });
                return filler;
            }

            // maybe 2 connectors + 2 fragments?
            if (fragment.InFillRange(gap - conLen2, 2))
            {
                float fragsLen = gap - conLen2;
                float fragHalfLen = (fragsLen - fragment.length) * 0.5f;
                filler.Add(new FenceObject()
                {
                    position = forward.endPosition + dir * connector.length * 0.5f,
                    direction = dir,
                    entity = connector
                });
                filler.Add(new FenceObject()
                {
                    position = forward.endPosition + dir * (connector.length + fragHalfLen),
                    direction = dir,
                    entity = fragment
                });
                filler.Add(new FenceObject()
                {
                    position = forward.endPosition + dir * (connector.length + fragHalfLen + fragment.length),
                    direction = dir,
                    entity = fragment
                });
                filler.Add(new FenceObject()
                {
                    position = forward.endPosition + dir * (connector.length * 1.5f + fragsLen),
                    direction = dir,
                    entity = connector
                });
                return filler;
            }


            if (f < 0.00001f)
            {
                // couldn't find fill?
                // remove one fragment from one segment and try again
                if (backward.fragmentCount == 2)
                {
                    backward.fragmentCount = 1;
                    f = 1.2f;
                }
                // couldnt find then? try with three fragments instead
                else if (backward.fragmentCount == 1)
                {
                    backward.fragmentCount = 3;
                    f = 1.2f;
                }
                // couldnt find even then? reverse change and cancel finding :c
                else if (backward.fragmentCount == 3)
                {
                    // leave it "random"
                    if (Mathf.FloorToInt(backward.position.magnitude) % 2 == 0)
                    {
                        forward.fragmentCount = 2;
                        backward.fragmentCount = 3;
                    }
                    else
                    {
                        forward.fragmentCount = 3;
                        backward.fragmentCount = 2;
                    }
                    break;
                }
            }
        }

        return filler;
    }

    public void MakeHoles()
    {
        // TODO - make it better
        foreach (var curve in brokenCurves)
        {
            if (curve.filler != null && curve.filler.Count == 0)
            {
                curve.forawrdSegments[curve.forawrdSegments.Count - 1].fragmentCount = 0;
                curve.backwardSegments[curve.backwardSegments.Count - 1].fragmentCount = 0;
            }
        }
    }

    public void PrepareObjects()
    {
        foreach (var curve in brokenCurves)
        {
            curve.objects = new List<FenceObject>();
            PrepareObjectsForSection(curve.forawrdSegments, curve.objects, false);
            PrepareObjectsForSection(curve.backwardSegments, curve.objects, true);
            if (curve.filler != null)
                curve.objects.AddRange(curve.filler);
        }
    }
    void PrepareObjectsForSection(List<Segment> segments, List<FenceObject> target, bool reversed)
    {
        for (int i=0; i<segments.Count; i++)
        {
            if (i > 0)
            {
                Vector2 connectorFrom = segments[i - 1].endPosition;
                Vector2 connectorTo = segments[i].position;
                Vector2 connectorDirection = (connectorTo - connectorFrom).normalized;
                target.Add(new FenceObject()
                {
                    position = (connectorFrom + connectorTo) * 0.5f,
                    direction = reversed ? -connectorDirection : connectorDirection,
                    entity = config.connectorEntity
                });
            }

            float dist = 0.0f;
            for (int j=0; j<segments[i].fragmentCount; j++)
            {
                float eDist = dist + config.fragmentEntity.length * 0.5f;
                target.Add(new FenceObject()
                {
                    position = segments[i].position + segments[i].direction * eDist,
                    direction = reversed ? -segments[i].direction : segments[i].direction,
                    entity = config.fragmentEntity
                });
                dist += config.fragmentEntity.length;
            }
        }
    }

    public void BuildSections(GameObject parent)
    {
        GameObject element, container;
        for (int i = 0; i < brokenCurves.Count; i++)
        {
            container = new GameObject(i.ToString());
            container.transform.parent = parent.transform;
            container.transform.position = parent.transform.position;

            foreach (var o in brokenCurves[i].objects)
            {
                element = GameObject.Instantiate(o.entity.gameObject);
                element.transform.parent = container.transform;
                element.transform.localPosition = new Vector3(
                    o.position.x,
                    0.0f,
                    o.position.y
                    );
                element.transform.right = new Vector3(
                    o.direction.x,
                    0.0f,
                    o.direction.y
                    );
            }
        }   
    }

    /*
    public void PrepareOutlineSections()
    {
        fenceSections = new List<FenceSection>();

        List<Vector2> controlPoints = new List<Vector2>(polygon.Points);
        controlPoints.Add(controlPoints[0]);

        float sectionLength = config.GetLength();
        float sectionLengthWithEnd = config.GetLengthWithEnd();

        int targetPointI = 1;
        Vector2 nextPosition = controlPoints[0];
        Vector2 direction;
        while (targetPointI < controlPoints.Count)
        {
            direction = controlPoints[targetPointI] - nextPosition;
            if (direction.magnitude >= sectionLengthWithEnd)
            {
                fenceSections.Add(new FenceSection()
                {
                    position = nextPosition,
                    direction = direction.normalized
                });
                nextPosition += direction.normalized * sectionLength;
            }
            else
            {
                targetPointI++;
            }
        }

        fenceSections[fenceSections.Count - 1].withEnd = true;
    }

    public void PrepareObjects()
    {
        fenceObjects = new List<FenceObject>();
        var entites = new List<CellFenceConfig.FenceEntity>(config.sectionEntities);
        var entitesWithEnd = new List<CellFenceConfig.FenceEntity>(config.sectionEntities);
        entitesWithEnd.Add(config.endingEntity);

        Debug.Log(fenceSections.Count);
        foreach (FenceSection section in fenceSections)
        {
            float dist = 0.0f;
            foreach (var entity in section.withEnd ? entitesWithEnd : entites)
            {
                float eDist = dist + entity.length * 0.5f;
                fenceObjects.Add(new FenceObject()
                {
                    prefab = entity.gameObject,
                    position = section.position + section.direction * eDist,
                    direction = section.direction,
                    length = entity.length
                });
                dist += entity.length;
            }
        }
    }
    */
    /*
    public void Build(GameObject parent)
    {
        GameObject element;
        foreach (var o in fenceObjects)
        {
            element = GameObject.Instantiate(o.entity.gameObject);
            element.transform.parent = parent.transform;
            element.transform.localPosition = new Vector3(
                o.position.x,
                0.0f,
                o.position.y
                );
            element.transform.right = new Vector3(
                o.direction.x,
                0.0f,
                o.direction.y
                );
        }
    }
    */
}
