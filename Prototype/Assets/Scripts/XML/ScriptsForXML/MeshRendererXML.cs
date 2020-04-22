using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEditor;

public class MeshRendererXML : MonoBehaviour
{

    public string path = "Wpisz ścieżkę względna katalogu z modelem od katalogu Assets";
    public string model = "Wpisz nazwę modelu";

    string output;

    public override string ToString()
    {
        output = "";
        output += "<component name=\"MeshRenderer\">";

        if (path != "Wpisz nazwę katalogu z modelem")
        {
            output += "<property value=\"" + path + ":" + model + "\" name=\"model\"/>";
        }

        output += "</component>";
        return output;
    }
}
