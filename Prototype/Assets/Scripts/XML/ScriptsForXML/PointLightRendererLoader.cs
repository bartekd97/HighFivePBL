using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class PointLightRendererLoader : MonoBehaviour
{
    public Vector3 color;
    public float radius;
    public float intensity;
    public float shadowIntensity;

    string output;

    public override string ToString()
    {
        output = "";
        output += "<component name=\"BoneAttacherLoader\">";

        string forceX = color.x.ToString("F2");
        string forceY = color.y.ToString("F2");
        string forceZ = color.z.ToString("F2");

        string forceXX = forceX.Replace(",", ".");
        string forceYY = forceY.Replace(",", ".");
        string forceZZ = forceZ.Replace(",", ".");
        output += "<property value=\"" + forceXX + "," + forceYY + "," + forceZZ + "\" name=\"color\"/>";


        output += "<property value=\"" + radius + "\" name=\"radius\"/>";
        output += "<property value=\"" + intensity + "\" name=\"intensity\"/>";
        output += "<property value=\"" + shadowIntensity + "\" name=\"shadowIntensity\"/>";


        output += "</component>";
        return output;
    }
}
