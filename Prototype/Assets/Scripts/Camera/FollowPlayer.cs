using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class FollowPlayer : MonoBehaviour
{
    public CharController Player;
    public Transform PlayerTransform;
    public Transform GhostTransform;

    private Vector3 cameraOffset;

    [Range(0.01f, 1.0f)]
    public float SmoothFactor = 0.5f;

    // Start is called before the first frame update
    void Start()
    {
        cameraOffset = transform.position - PlayerTransform.position;
        //transform.LookAt(PlayerTransform);
    }

    // Update is called once per frame
    void LateUpdate()
    {
        Vector3 newPos;
        if (Player.ghostMovement && Player.ghost.IsMarking)
            newPos = GhostTransform.position + cameraOffset;
        else
            newPos = PlayerTransform.position + cameraOffset;

        transform.position = Vector3.Slerp(transform.position, newPos, SmoothFactor);
    }
}
