using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class RigidBodyXML : MonoBehaviour
{

    public Vector3 velocity;
    public Vector3 acceleration;
    public Vector3 movePosition;
    public float mass;

    string output;

    public override string ToString()
    {
        output = "";
        output += "<component name=\"RigidBody\">";

        if (velocity != Vector3.zero)
        {
            string velocityX = velocity.x.ToString("F2");
            string velocityY = velocity.y.ToString("F2");
            string velocityZ = velocity.z.ToString("F2");

            string velocityXX = velocityX.Replace(",", ".");
            string velocityYY = velocityY.Replace(",", ".");
            string velocityZZ = velocityZ.Replace(",", ".");

            output += "<property value=\"" + velocityXX + "," + velocityYY + "," + velocityZZ + "\" name=\"velocity\"/>";
        }
        if (acceleration != Vector3.zero)
        {
            string accelerationX = acceleration.x.ToString("F2");
            string accelerationY = acceleration.y.ToString("F2");
            string accelerationZ = acceleration.z.ToString("F2");

            string accelerationXX = accelerationX.Replace(",", ".");
            string accelerationYY = accelerationY.Replace(",", ".");
            string accelerationZZ = accelerationZ.Replace(",", ".");

            output += "<property value=\"" + accelerationXX + "," + accelerationYY + "," + accelerationZZ + "\" name=\"acceleration\"/>";
        }
        if (movePosition != Vector3.zero)
        {
            string movePositionX = movePosition.x.ToString("F2");
            string movePositionY = movePosition.y.ToString("F2");
            string vmovePositionZ = movePosition.z.ToString("F2");

            string movePositionXX = movePositionX.Replace(",", ".");
            string movePositionYY = movePositionY.Replace(",", ".");
            string movePositionZZ = vmovePositionZ.Replace(",", ".");

            output += "<property value=\"" + movePositionXX + "," + movePositionYY + "," + movePositionZZ + "\" name=\"movePosition\"/>";
        }
        if (mass != 0.0f)
        {
            output += "<property value=\"" + mass + "\" name=\"mass\"/>";
        }

        output += "</component>";
        return output;
    }

}
