using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class CircleCollider : MonoBehaviour
{

    public float radius;

    string output;

    public override string ToString()
    {
        output = "";
        output += "<component name=\"CircleCollider\">";

        if (radius < 0.0f)
        {
            output += "<property value=\"" + radius + "\" name=\"radius\"/>";
        }

        output += "</component>";
        return output;
    }
}
