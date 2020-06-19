using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class CircleCollider : MonoBehaviour
{

    public float radius;
    public bool frozen;
    public enum ColliderTypes { DYNAMIC, TRIGGER, STATIC }
    public ColliderTypes type;

    string output;

    public override string ToString()
    {
        output = "";
        output += "<component name=\"CircleCollider\">";

        if (radius < 0.0f)
        {
            output += "<property value=\"" + radius + "\" name=\"radius\"/>";
        }
        output += "<property value=\"" + frozen.ToString().ToLower() + "\" name=\"frozen\"/>";
        output += "<property value=\"" + type + "\" name=\"type\"/>";

        output += "</component>";
        return output;
    }
}
