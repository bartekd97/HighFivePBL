using System.Collections;
using System.Collections.Generic;
using UnityEngine;

[System.Serializable]
public class MapFence
{
    [System.Serializable]
    public class FenceEntity
    {
        public string name;
        public GameObject gameObject;
        public float length;
    }
    public List<FenceEntity> entities;

    public float MaxBarrierSegmentLength;

    private float _VertexRadius;
    private float _DividerLength;
    private float _BarrierLength;

    bool _initialized = false;
    public void Initialize()
    {
        if (_initialized) return;

        _VertexRadius = entities.Find(e => e.name == "vertex").length * 0.5f;
        _DividerLength = entities.Find(e => e.name == "divider").length;
        _BarrierLength = entities.Find(e => e.name == "barrier").length;

        _initialized = true;
    }

    public void CreateEdgeFence(Vector3 from, Vector3 to, GameObject parent)
    {
        if (!_initialized) throw new System.Exception("MapFence not initialized.");

        float totalLength = (from - to).magnitude;
        float targetLength = totalLength - (_VertexRadius * 2f);

        int segments = 1;
        while ((targetLength / segments) > MaxBarrierSegmentLength)
            segments++;

        float fenceLength = /*(_DividerLength * 2) +*/ (_DividerLength * (segments - 1));
        int barriersNum = 0;
        while (fenceLength < targetLength)
        {
            fenceLength += _BarrierLength;
            barriersNum++;
        }

        int barriersPerSegment = barriersNum / segments;
        int leftBarriersForLastSegment = barriersNum - (barriersPerSegment * segments);


        List<string> spawnList = new List<string>();
        //spawnList.Add("divider");
        for (int i = 0; i < segments; i++)
        {
            if (i != 0)
                spawnList.Add("divider");
            for (int j = 0; j < barriersPerSegment; j++)
                spawnList.Add("barrier");
        }
        for (int k = 0; k < leftBarriersForLastSegment; k++)
            spawnList.Add("barrier");
        //spawnList.Add("divider");

        Vector3 direction = (to - from).normalized;
        Vector3 position = from + direction * ((totalLength - fenceLength) * 0.5f);

        foreach (string entityName in spawnList)
        {
            FenceEntity entity = entities.Find(e => e.name == entityName);
            GameObject part = Object.Instantiate(entity.gameObject, parent.transform);
            part.transform.position = position + direction * entity.length * 0.5f;
            part.transform.LookAt(to);
            position += direction * entity.length;
        }

    }

    public void CreateVertex(Vector3 position, GameObject parent)
    {
        FenceEntity entity = entities.Find(e => e.name == "vertex");
        GameObject part = Object.Instantiate(entity.gameObject, parent.transform);
        part.transform.position = position;
    }
}
