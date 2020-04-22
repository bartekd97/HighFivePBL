using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Gravity : MonoBehaviour
{

    public Vector3 force;

    string output;

    public override string ToString()
    {
        output = "";
        output += "<component name=\"Gravity\">";

        if (true)
        {
            string forceX = force.x.ToString("F2");
            string forceY = force.y.ToString("F2");
            string forceZ = force.z.ToString("F2");

            string forceXX = forceX.Replace(",", ".");
            string forceYY = forceY.Replace(",", ".");
            string forceZZ = forceZ.Replace(",", ".");

            output += "<property value=\"" + forceXX + "," + forceYY + "," + forceZZ + "\" name=\"force\"/>";
        }

        output += "</component>";
        return output;
    }
}
