using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class ScriptLoader : MonoBehaviour
{
    public string name = "Wpisz nazwe skryptu";

    string output;

    public override string ToString()
    {
        output = "";
        output += "<component name=\"MeshRendererLoader\">";

        if (name != "Wpisz nazwę katalogu z modelem")
        {
            output += "<property value=\"" + name + "\" name=\"name\"/>";
        }

        output += "</component>";
        return output;
    }
}
