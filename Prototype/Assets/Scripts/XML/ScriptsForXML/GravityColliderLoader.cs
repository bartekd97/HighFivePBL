using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class GravityColliderLoader : MonoBehaviour
{
    public Vector3 ZValues;
    public Vector3 heightValues;

    string output;

    public override string ToString()
    {
        string forceX;
        string forceY;
        string forceZ;

        string forceXX;
        string forceYY;
        string forceZZ;
        output = "";
        output += "<component name=\"GravityColliderLoader\">";



        if (true)
        {
            forceX = ZValues.x.ToString("F2");
            forceY = ZValues.y.ToString("F2");
            forceZ = ZValues.z.ToString("F2");

            forceXX = forceX.Replace(",", ".");
            forceYY = forceY.Replace(",", ".");
            forceZZ = forceZ.Replace(",", ".");

            output += "<property value=\"" + forceXX + "," + forceYY + "," + forceZZ + "\" name=\"ZValues\"/>";
        }

        if (true)
        {
            forceX = heightValues.x.ToString("F2");
            forceY = heightValues.y.ToString("F2");
            forceZ = heightValues.z.ToString("F2");

            forceXX = forceX.Replace(",", ".");
            forceYY = forceY.Replace(",", ".");
            forceZZ = forceZ.Replace(",", ".");

            output += "<property value=\"" + forceXX + "," + forceYY + "," + forceZZ + "\" name=\"heights\"/>";
        }

        output += "</component>";
        return output;
    }
}
