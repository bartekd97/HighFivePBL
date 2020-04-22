using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System.IO;
using System.Xml.Serialization;


public class PrefabExporter : MonoBehaviour
{

    public List<GameObject> prefabs;
    public List<Component> components;
    string content;

    public void Generate()
    {
        AddPrefabs();

        Debug.Log(prefabs.Count);

        foreach (GameObject prefab in prefabs)
        {
            //open stream
            content += "<prefab>";
            AddComponentAndChildren(prefab);
            content += "</prefab>";
            //Serialize(prefab, "test.xml");

            //close stream
            CreateFile(prefab.name);
        }

    }

    void AddComponentAndChildren(GameObject prefab)
    {
        components = new List<Component>();
        foreach (var component in prefab.GetComponents<Component>())
        {
            if (component != this) components.Add(component);

            if(component.GetType().ToString().Substring(0, 5) != "Unity")
            {
                content += component.ToString();
                //Debug.Log(component.GetInstanceID());
                //Debug.Log(component.GetType());
                //component.Kamil();
            }
            //Debug.Log(component.GetType());
            //Debug.Log(component.GetInstanceID());
            //Debug.Log(component.ToString());
            //string componentFun = component.GetType().ToString() + ".Kamil()";
            //component.Invoke("Kamil", 0.0f);
            //component.GetType().Kamil();

            //adding to stream info about component
        }

        if (prefab.transform.childCount != 0)
        {
            content += "<children>";
            foreach (Transform child in prefab.transform)
            {
                //adding to stream info about child
                content += "<child>";
                AddComponentAndChildren(child.gameObject);
                content += "</child>";
            }
            content += "</children>";

        }

        Debug.Log(prefab.transform.childCount);
    }

    //void Save(GameObject rootObject, string filename)
    //{
    //    BuildingInfo buildingInfo = new BuildingInfo(rootObject);
    //    XmlSerializer serializer = new XmlSerializer(typeof(BuildingInfo));
    //    TextWriter writer = new StreamWriter(filename);
    //    serializer.Serialize(writer, buildingInfo);
    //    writer.Close();
    //    print("Objects saved into XML file\n");
    //}

    //public static void Serialize(object item, string path)
    //{
    //    XmlSerializer serializer = new XmlSerializer(item.GetType());
    //    StreamWriter writer = new StreamWriter(path);
    //    serializer.Serialize(writer.BaseStream, item);
    //    writer.Close();
    //}

    private void AddPrefabs()
    {
        prefabs = new List<GameObject>();
        Object[] subPrefabsList = Resources.LoadAll("Prefabs", typeof(GameObject));
        foreach (GameObject subPrefab in subPrefabsList)
        {
            GameObject lo = (GameObject)subPrefab;

            prefabs.Add(lo);
        }
    }

    void CreateFile(string filename)
    {
        string path = Application.dataPath + "/XMLs/" + filename + ".xml";

        File.WriteAllText(path, content);

        content = null;

        //string content = "<elo></elo>";

        //File.AppendAllText(path, content);
    }

}
