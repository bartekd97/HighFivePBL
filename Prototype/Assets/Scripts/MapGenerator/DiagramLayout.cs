using System.Collections;
using System.Collections.Generic;
using UnityEngine;

[System.Serializable]
public class DiagramLayout
{
    public float width = 192;
    public float height = 256;

    public int columns = 3;
    public int rows = 4;

    public Rectf GetBounds()
    {
        return new Rectf(0, 0, width, height);
    }

    public List<Vector2f> CreatePoints()
    {
        float wPart = width / columns;
        float hPart = height / rows;
        List<Vector2f> points = new List<Vector2f>();

        for (int x=0; x<columns; x++)
        {
            for (int y=0; y<rows; y++)
            {
                points.Add(
                    new Vector2f(
                        Random.Range(wPart * x, wPart * (x + 1)),
                        Random.Range(hPart * y, hPart * (y + 1))
                    )
                );
            }
        }
        return points;
    }
}
