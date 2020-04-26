using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class ColliderXML : MonoBehaviour
{
    public enum ColliderTypes { DYNAMIC, STATIC }
    public ColliderTypes type;

    public enum ColliderShapes { CIRCLE, BOX }
    public ColliderShapes shape;

    string output;

    public override string ToString()
    {
        output = "";
        output += "<component name=\"Collider\">";

        output += "<property value=\"" + type + "\" name=\"type\"/>";

        output += "<property value=\"" + shape + "\" name=\"shape\"/>";

        output += "</component>";
        return output;
    }

}
