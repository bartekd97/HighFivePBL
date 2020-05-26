using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class ModelHolderLoader : MonoBehaviour
{
    public string libraryName = "Wpisz nazwę biblioteki";
    public string model = "Wpisz nazwę modelu";


    string output;

    public override string ToString()
    {
        output = "";
        output += "<component name=\"ModelHolderLoader\">";

        if (libraryName != "Wpisz nazwę katalogu z modelem")
        {
            output += "<property value=\"" + libraryName + ":" + model + "\" name=\"model\"/>";
        }

        output += "</component>";
        return output;
    }
}
