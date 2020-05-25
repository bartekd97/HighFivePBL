﻿using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEditor;

public class MeshRendererXML : MonoBehaviour
{
    public bool configureFromHolder = false;

    public bool useMeshPath = false;
    public string meshPath;

    public bool useMaterialPath = false;
    public string materialPath;

    string output;

    public override string ToString()
    {
        output = "";
        output += "<component name=\"MeshRendererLoader\">";
        output += "<property value=\"" + configureFromHolder + "\" name=\"configureFromHolder\"/>";

        output += "<property value=\"" + useMeshPath + "\" name=\"useMeshPath\"/>";
        output += "<property value=\"" + useMaterialPath + "\" name=\"useMaterialPath\"/>";

        if (useMeshPath == true)
        {
            output += "<property value=\"" + meshPath + "\" name=\"mesh\"/>";
        }

        if (useMaterialPath == true)
        {
            output += "<property value=\"" + materialPath + "\" name=\"material\"/>";
        }

        output += "</component>";
        return output;
    }
}
