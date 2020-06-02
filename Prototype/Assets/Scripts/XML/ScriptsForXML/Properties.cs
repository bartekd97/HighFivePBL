using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Properties : MonoBehaviour
{
    public Vector3 localPosition;
    public Vector3 localRotation;
    public Vector3 localScale;

    public float widthBC;
    public float heightBC;
    string output;

    public float length;
    public bool isLength;

    public float minForFill;
    public bool isMinForFill;

    public float maxForFill;
    public bool isMaxForFill;


    public bool isParent;

    public override string ToString()
    {
        output = "";

        if(isParent)
        {
            localScale = this.transform.localScale;
            output += "<prefab defaultScale=\"";

            string localScaleX = localScale.x.ToString("F2");
            string localScaleY = localScale.y.ToString("F2");
            string localScaleZ = localScale.z.ToString("F2");

            string localScaleXX = localScaleX.Replace(",", ".");
            string localScaleYY = localScaleY.Replace(",", ".");
            string localScaleZZ = localScaleZ.Replace(",", ".");

            output += localScaleXX + "," + localScaleYY + "," + localScaleZZ + "\">";

            string localRotationX = localRotation.x.ToString("F2");
            string localRotationY = localRotation.y.ToString("F2");
            string localRotationZ = localRotation.z.ToString("F2");

            string localRotationXX = localRotationX.Replace(",", ".");
            string localRotationYY = localRotationY.Replace(",", ".");
            string localRotationZZ = localRotationZ.Replace(",", ".");

            output += "<property value=\"" + localRotationXX + "\" name=\"rotx\"/>";
            output += "<property value=\"" + localRotationYY + "\" name=\"roty\"/>";
            output += "<property value=\"" + localRotationZZ + "\" name=\"rotz\"/>";

        }
        else
        {
            localPosition = this.transform.localPosition;
            localRotation = this.transform.localRotation.eulerAngles;
            localScale = this.transform.localScale;
            output += "<child name=\"" + this.gameObject.name + "\" scale =\"";

            string localScaleX = localScale.x.ToString("F2");
            string localScaleY = localScale.y.ToString("F2");
            string localScaleZ = localScale.z.ToString("F2");

            string localScaleXX = localScaleX.Replace(",", ".");
            string localScaleYY = localScaleY.Replace(",", ".");
            string localScaleZZ = localScaleZ.Replace(",", ".");

            output += localScaleXX + "," + localScaleYY + "," + localScaleZZ + "\" rotation=\"";

            string localRotationX = localRotation.x.ToString("F2");
            string localRotationY = localRotation.y.ToString("F2");
            string localRotationZ = localRotation.z.ToString("F2");

            string localRotationXX = localRotationX.Replace(",", ".");
            string localRotationYY = localRotationY.Replace(",", ".");
            string localRotationZZ = localRotationZ.Replace(",", ".");

            output += localRotationXX + "," + localRotationYY + "," + localRotationZZ + "\" position=\"";
           
            string localPositionX = localPosition.x.ToString("F2");
            string localPositionY = localPosition.y.ToString("F2");
            string localPositionZ = localPosition.z.ToString("F2");

            string localPositionXX = localPositionX.Replace(",", ".");
            string localPositionYY = localPositionY.Replace(",", ".");
            string localPositionZZ = localPositionZ.Replace(",", ".");

            output += localPositionXX + "," + localPositionYY + "," + localPositionZZ + "\">";

        }

        if(isLength)
        {
            output += "<property value=\"" + length + "\" name=\"length\"/>";
        }

        if (isMinForFill)
        {
            output += "<property value=\"" + minForFill + "\" name=\"minForFill\"/>";
        }

        if (isMaxForFill)
        {
            output += "<property value=\"" + maxForFill + "\" name=\"maxForFill\"/>";
        }
        if (widthBC > 0.0f)
        {
            string widthBCstring = widthBC.ToString("F2");
            widthBCstring = widthBCstring.Replace(",", ".");

            output += "<property value=\"" + widthBCstring + "\" name=\"width\"/>";
        }
        if (heightBC > 0.0f)
        {
            string heightBCstring = heightBC.ToString("F2");
            heightBCstring = heightBCstring.Replace(",", ".");

            output += "<property value=\"" + heightBCstring + "\" name=\"height\"/>";
        }


        return output;
    }
}
