using System.Collections;
using System.Collections.Generic;
using System.Linq;
using UnityEngine;

[System.Serializable]
public class CellFenceConfig
{
    [System.Serializable]
    public class FenceEntity
    {
        public GameObject gameObject;
        public float length;
    }

    //public List<FenceEntity> sectionEntities;
    //public FenceEntity endingEntity;

    public FenceEntity gateEntity;
    public FenceEntity fragmentEntity;
    public FenceEntity connectorEntity;
    public float gateDistance = 3.0f;
    public int fragmentCount = 2;
    public float innerLevelFenceLocation = 0.9f;

    /*
    public float GetLength()
    {
        return sectionEntities.Sum(e => e.length);
    }
    public float GetLengthWithEnd()
    {
        return sectionEntities.Sum(e => e.length) + endingEntity.length;
    }
    */
}
