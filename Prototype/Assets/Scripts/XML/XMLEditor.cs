using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEditor;

[CustomEditor(typeof(PrefabExporter))]
public class XMLEditor : Editor
{
    public override void OnInspectorGUI()
    {
        base.OnInspectorGUI();

        PrefabExporter exporter = (PrefabExporter)target;

        if (GUILayout.Button("Generuj XML"))
        {
            Debug.Log("Trwa generowanie plików XML");
            exporter.Generate();
            Debug.Log("Zakończono generowanie plików XML");
        }
    }
}
