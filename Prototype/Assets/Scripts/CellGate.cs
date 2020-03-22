using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class CellGate : MonoBehaviour
{
    public CellBridge Bridge;
    public MapCell Cell;

    public bool IsClosed { get; private set; }
    public Transform LeftDoor;
    public Transform RightDoor;

    [SerializeField]
    float closeTime = 0.35f;
    [SerializeField]
    float openTime = 1.5f;

    private void Awake()
    {
        IsClosed = false;
    }

    public void Open()
    {
        if (!IsClosed) return;
        StartCoroutine(coOpen());
        IsClosed = false;
    }
    IEnumerator coOpen()
    {
        float dt = 0.0f;
        while (dt < openTime)
        {
            yield return new WaitForEndOfFrame();
            float rot = Mathf.SmoothStep(0.0f, 90.0f, dt / openTime);
            LeftDoor.localRotation = Quaternion.Euler(0.0f, rot, 0.0f);
            RightDoor.localRotation = Quaternion.Euler(0.0f, -rot, 0.0f);
            dt += Time.deltaTime;
        }
    }

    public void Close()
    {
        if (IsClosed) return;
        StartCoroutine(coClose());
        IsClosed = true;
    }
    IEnumerator coClose()
    {
        float dt = 0.0f;
        while (dt < closeTime)
        {
            yield return new WaitForEndOfFrame();
            float rot = Mathf.SmoothStep(90.0f, 0.0f, dt / closeTime);
            LeftDoor.localRotation = Quaternion.Euler(0.0f, rot, 0.0f);
            RightDoor.localRotation = Quaternion.Euler(0.0f, -rot, 0.0f);
            dt += Time.deltaTime;
        }
    }
}
