
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class SkinAnimatorLoader : MonoBehaviour
{
    public bool configureFromHolder = false;

    public bool useClipsPath = false;
    public string clipsPath;
    public string libraryName = "Wpisz nazwę biblioteki";
    public string model = "Wpisz nazwę modelu";


    string output;

    public override string ToString()
    {
        output = "";
        output += "<component name=\"SkinAnimatorLoader\">";
        output += "<property value=\"" + useClipsPath + "\" name=\"useClipsPath\"/>";

        if (useClipsPath == true)
        {
            output += "<property value=\"" + clipsPath + "\" name=\"clips\"/>";
        }

     

        output += "</component>";
        return output;
    }
}
