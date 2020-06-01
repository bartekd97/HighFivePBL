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

        if (width > 0.0f)
        {
            string widthstring = width.ToString("F2");
            widthstring = widthstring.Replace(",", ".");

            output += "<property value=\"" + widthstring + "\" name=\"width\"/>";
        }
        if (height > 0.0f)
        {
            string heightstring = height.ToString("F2");
            heightstring = heightstring.Replace(",", ".");

            output += "<property value=\"" + heightstring + "\" name=\"height\"/>";
        }
        
        output += "<property value=\"" + frozen.ToString().ToLower() + "\" name=\"frozen\"/>";
        output += "<property value=\"" + type + "\" name=\"type\"/>";

        output += "</component>";
        return output;
    }
}
