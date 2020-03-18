using System.Collections;
using System.Collections.Generic;
using UnityEngine;

[System.Serializable]
public class CellMeshConfig
{
    public float mainScale = 0.9f;
    public float depth = 7.0f;
    public float depthMax = 20.0f;
    public float innerScale = 0.8f;
    public int outlineSplits = 16;
    public float nextOutlineScale = 0.95f;
    public float nextOutlineScaleMultiplier = 1.0f;
    public float vertexBevel = 0.2f;
    public int vertexBevelSteps = 2;
    public int circularSegments = 32;
    public float noiseCenterRatio = 0.75f;
    public float noiseScale = 0.2f;
    public Vector2 noiseForce = new Vector2(5, 10);
}
