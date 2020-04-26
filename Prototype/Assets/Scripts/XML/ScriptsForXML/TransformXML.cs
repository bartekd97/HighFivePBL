using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class TransformXML : MonoBehaviour
{

    public Vector3 localPosition;
    public Vector3 localRotation;
    public Vector3 localScale;

    string output;

    public override string ToString()
    {
        output = "";
        output += "<component name=\"Transform\">";

        localPosition = this.transform.position;
        localRotation = this.transform.rotation.eulerAngles;
        localScale = this.transform.localScale;


        if (localPosition != Vector3.zero)
        {
            string localPositionX = localPosition.x.ToString("F2");
            string localPositionY = localPosition.y.ToString("F2");
            string localPositionZ = localPosition.z.ToString("F2");

            string localPositionXX = localPositionX.Replace(",", ".");
            string localPositionYY = localPositionY.Replace(",", ".");
            string localPositionZZ = localPositionZ.Replace(",", ".");

            output += "<property value=\"" + localPositionXX + "," + localPositionYY + "," + localPositionZZ + "\" name=\"position\"/>";
        }
        if (localRotation != Vector3.zero)
        {
            string localRotationX = localRotation.x.ToString("F2");
            string localRotationY = localRotation.y.ToString("F2");
            string localRotationZ = localRotation.z.ToString("F2");

            string localRotationXX = localRotationX.Replace(",", ".");
            string localRotationYY = localRotationY.Replace(",", ".");
            string localRotationZZ = localRotationZ.Replace(",", ".");

            output += "<property value=\"" + localRotationXX + "," + localRotationYY + "," + localRotationZZ + "\" name=\"rotation\"/>";
        }
        if (localScale != Vector3.zero)
        {
            string localScaleX = localScale.x.ToString("F2");
            string localScaleY = localScale.y.ToString("F2");
            string localScaleZ = localScale.z.ToString("F2");

            string localScaleXX = localScaleX.Replace(",", ".");
            string localScaleYY = localScaleY.Replace(",", ".");
            string localScaleZZ = localScaleZ.Replace(",", ".");

            output += "<property value=\"" + localScaleXX + "," + localScaleYY + "," + localScaleZZ + "\" name=\"scale\"/>";
        }

        output += "</component>";
        return output;
    }

}
