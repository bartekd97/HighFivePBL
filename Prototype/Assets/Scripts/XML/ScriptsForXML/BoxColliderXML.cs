using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class BoxColliderXML : MonoBehaviour
{
    public float width;
    public float height;
    public bool frozen;
    public enum ColliderTypes { DYNAMIC, TRIGGER, STATIC }
    public ColliderTypes type;

    string output;

    public override string ToString()
    {
        output = "";
        output += "<component name=\"BoxCollider\">";

        if (width < 0.0f)
        {
            output += "<property value=\"" + width + "\" name=\"width\"/>";
        }
        if (height < 0.0f)
        {
            output += "<property value=\"" + height + "\" name=\"height\"/>";
        }
        
        output += "<property value=\"" + frozen + "\" name=\"frozen\"/>";
        output += "<property value=\"" + type + "\" name=\"type\"/>";

        output += "</component>";
        return output;
    }
}
