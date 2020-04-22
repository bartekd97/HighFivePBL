using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class CellBridge : MonoBehaviour
{
    public MapCell CellA;
    public CellGate GateA;

    public MapCell CellB;
    public CellGate GateB;

    string output;

    public override string ToString()
    {
        output = "";
        output += "<component name=\"ScriptComponent\">";
        output += "<property name=\"name\" value=\"" + this.name + "\"/>";
        output += "</component>";
        return output;
    }
}
