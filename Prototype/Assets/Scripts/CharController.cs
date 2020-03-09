using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class CharController : MonoBehaviour
{
    public bool ghostMovement { get; private set; }


    [SerializeField]
    GameObject ghostObject;

    [SerializeField]
    float maxGhiostDistance = 8.0f;
    [SerializeField]
    float ghostDistanceRecoverySpeed = 1.0f;

    [SerializeField]
    float speed = 4.0f;

    Vector3 forward, right;
    float leftGhostDistance;

    // Start is called before the first frame update
    void Start()
    {
        forward = Camera.main.transform.forward;
        forward.y = 0;
        forward = Vector3.Normalize(forward);
        right = Quaternion.Euler(new Vector3(0, 90, 0)) * forward;
        ghostMovement = false;
        leftGhostDistance = maxGhiostDistance;
    }

    public float GetLeftGhostLevel()
    {
        return leftGhostDistance / maxGhiostDistance;
    }

    // Update is called once per frame
    void Update()
    {
        if (Input.GetKeyDown(KeyCode.Mouse0) && leftGhostDistance > 0.0f)
            StartGhost();
        else if (Input.GetKeyUp(KeyCode.Mouse0))
            StopGhost();

        if (ghostMovement && Input.GetKey(KeyCode.Mouse0))
            MoveGhost();
        else

        if (!ghostMovement)
        {
            leftGhostDistance = Mathf.Min(
                    maxGhiostDistance,
                    leftGhostDistance + Time.deltaTime * ghostDistanceRecoverySpeed
                );

            if (Input.anyKey)
                Move();
        }
    }

    void StartGhost()
    {
        if (ghostMovement) return;
        ghostObject.SetActive(true);

        ghostObject.transform.position = transform.position;

        Ray mouseRay = Camera.main.ScreenPointToRay(Input.mousePosition);
        float yDist = mouseRay.origin.y - ghostObject.transform.position.y;
        Vector3 rayPoint = mouseRay.origin - mouseRay.direction * (yDist / mouseRay.direction.y);
        Quaternion toRot = Quaternion.LookRotation(rayPoint - ghostObject.transform.position);
        ghostObject.transform.rotation = toRot;

        ghostMovement = true;
    }
    void StopGhost()
    {
        if (!ghostMovement) return;
        ghostObject.SetActive(false);
        ghostMovement = false;
    }

    void MoveGhost()
    {
        Ray mouseRay = Camera.main.ScreenPointToRay(Input.mousePosition);
        float yDist = mouseRay.origin.y - ghostObject.transform.position.y;
        Vector3 rayPoint = mouseRay.origin - mouseRay.direction * (yDist / mouseRay.direction.y);
        Quaternion toRot = Quaternion.LookRotation(rayPoint - ghostObject.transform.position);

        ghostObject.transform.rotation = Quaternion.Lerp(ghostObject.transform.rotation, toRot, Time.deltaTime * 5f);
        Vector3 moveBy = Vector3.forward * speed * Time.deltaTime;
        ghostObject.transform.Translate(moveBy, Space.Self);

        Vector3 heading = ghostObject.transform.position - transform.position;
        heading.y = 0;
        heading = Vector3.Normalize(heading);
        transform.forward = heading;

        leftGhostDistance -= moveBy.magnitude;
        if (leftGhostDistance <= 0.0f)
        {
            leftGhostDistance = 0.0f;
            StopGhost();
        }
    }

    void Move()
    {
        Vector3 direction = new Vector3(Input.GetAxis("HorizontalKey"), 0, Input.GetAxis("VerticalKey"));
        Vector3 rightMovement = right * speed * Time.deltaTime * Input.GetAxis("HorizontalKey");
        Vector3 forwardMovement = forward * speed * Time.deltaTime * Input.GetAxis("VerticalKey");

        Vector3 heading = Vector3.Normalize(rightMovement + forwardMovement);

        transform.forward = heading;
        transform.position += rightMovement;
        transform.position += forwardMovement;
    }

    void PushEnemies()
    {

    }
}
