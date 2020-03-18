using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class ConvexPolygon
{
    public readonly Vector2[] Points;

    public ConvexPolygon(List<Vector2f> csdPoints, Vector2f center)
    {
        List<Vector2> points = new List<Vector2>();
        csdPoints.ForEach(p => points.Add(new Vector2(p.x - center.x, p.y - center.y)));
        Points = points.ToArray();
    }
    public ConvexPolygon(Vector2[] points)
    {
        Points = points;
    }

    public ConvexPolygon CreateCircular(int segments, int precision=4)
    {
        Vector2[] cpoints = new Vector2[segments];

        Vector2 point, direction;
        for (int i=0; i<segments; i++)
        {
            direction = new Vector2(
                    Mathf.Cos(((float)i / (float)segments) * Mathf.PI * 2.0f),
                    Mathf.Sin(((float)i / (float)segments) * Mathf.PI * 2.0f)
                );

            point = direction;
            while (IsPointInside(point))
                point += direction;


            direction *= 0.5f;
            point -= direction;
            for (int j = 0; j < precision; j++)
            {
                direction *= 0.5f;
                if (IsPointInside(point))
                    point += direction;
                else
                    point -= direction;
            }


            cpoints[i] = point;
        }

        return new ConvexPolygon(cpoints);
    }

    // https://stackoverflow.com/questions/1119627/how-to-test-if-a-point-is-inside-of-a-convex-polygon-in-2d-integer-coordinates
    public bool IsPointInside(Vector2 point)
    {
        //Check if a triangle or higher n-gon
        Debug.Assert(Points.Length >= 3);

        //n>2 Keep track of cross product sign changes
        var pos = 0;
        var neg = 0;

        for (var i = 0; i < Points.Length; i++)
        {
            //If point is in the polygon
            if (Points[i] == point)
                return true;

            //Form a segment between the i'th point
            var x1 = Points[i].x;
            var y1 = Points[i].y;

            //And the i+1'th, or if i is the last, with the first point
            var i2 = i < Points.Length - 1 ? i + 1 : 0;

            var x2 = Points[i2].x;
            var y2 = Points[i2].y;

            var x = point.x;
            var y = point.y;

            //Compute the cross product
            var d = (x - x1) * (y2 - y1) - (y - y1) * (x2 - x1);

            if (d > 0) pos++;
            if (d < 0) neg++;

            //If the sign changes, then point is outside
            if (pos > 0 && neg > 0)
                return false;
        }

        //If no change in direction, then on same side of all segments, and thus inside
        return true;
    }

    // 0.0 - at center
    // 0.5 - in the middle between center and edge
    // 1.0 - at efge (or outside)
    public float GetEdgeCenterRatio(Vector2 point, int precision=5)
    {
        if (!IsPointInside(point))
            return 1.0f;

        if (point.magnitude < 0.01f)
            return 0.0f;

        Vector2 direction = point.normalized;
        Vector2 edgePoint = point + direction;
        while (IsPointInside(edgePoint))
            edgePoint += direction;

        direction *= 0.5f;
        edgePoint -= direction;
        for (int j = 0; j < precision; j++)
        {
            direction *= 0.5f;
            if (IsPointInside(edgePoint))
                edgePoint += direction;
            else
                edgePoint -= direction;
        }

        return point.magnitude / edgePoint.magnitude;
    }


    public ConvexPolygon ScaledBy(float mult)
    {
        Vector2[] points = new Vector2[Points.Length];
        for (int i = 0; i < Points.Length; i++)
            points[i] = Points[i] * mult;
        return new ConvexPolygon(points);
    }

    public ConvexPolygon BeveledVerticesBy(float mult)
    {
        List<Vector2> points = new List<Vector2>();

        Vector2 previous, current, next;
        for (int i = 0; i < Points.Length; i++)
        {
            previous = Points[i > 0 ? i - 1 : Points.Length - 1];
            current = Points[i];
            next = Points[i < (Points.Length - 1) ? i + 1 : 0];

            points.Add(Vector2.Lerp(current, previous, mult));
            points.Add(Vector2.Lerp(current, next, mult));
        }

        return new ConvexPolygon(points.ToArray());
    }
}
