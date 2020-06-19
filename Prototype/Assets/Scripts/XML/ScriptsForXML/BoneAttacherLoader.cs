using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class BoneAttacherLoader : MonoBehaviour
{
    public string boneName = "Wpisz nazwe kosci";

    string output;

    public override string ToString()
    {
        output = "";
        output += "<component name=\"BoneAttacherLoader\">";

        if (name != "Wpisz nazwę katalogu z modelem")
        {
            output += "<property value=\"" + boneName + "\" name=\"boneName\"/>";
        }

        output += "</component>";
        return output;
    }
}
