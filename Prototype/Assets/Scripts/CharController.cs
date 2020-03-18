using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.SceneManagement;


public class CharController : MonoBehaviour
{
    public bool ghostMovement { get; private set; }


    [SerializeField]
    Ghost ghost;

    [SerializeField]
    float maxGhiostDistance = 8.0f;
    [SerializeField]
    float ghostDistanceRecoverySpeed = 1.0f;

    [SerializeField]
    float speed = 4.0f;
    [SerializeField]
    float ghostSpeed = 8.0f;

    [SerializeField]
    float health = 10.0f;

    [SerializeField]
    float pushBackDistance = 5.0f;
    [SerializeField]
    float pushBackForce = 10.0f;
    [SerializeField]
    float pushCooldownTime = 1.0f;

    Vector3 forward, right;
    float leftGhostDistance;
    float nextPushBackTime = 0.0f;

    //public bool pushedEnemies;

    // Start is called before the first frame update
    void Start()
    {
        forward = Camera.main.transform.forward;
        forward.y = 0;
        forward = Vector3.Normalize(forward);
        right = Quaternion.Euler(new Vector3(0, 90, 0)) * forward;
        ghostMovement = false;
        leftGhostDistance = maxGhiostDistance;
        //pushedEnemies = false;
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

        if (Input.GetKeyDown(KeyCode.Space) && Time.time >= nextPushBackTime)
        {
            PushEnemiesBack();
            nextPushBackTime = Time.time + pushCooldownTime;
        }
        /*
        if (Input.GetKeyDown(KeyCode.Space))
        {
            pushedEnemies = true;
        }
        else if (Input.GetKeyUp(KeyCode.Space))
        {
            pushedEnemies = false;
        }
        */
    }

    void PushEnemiesBack()
    {
        foreach (EnemyController ec in FindObjectsOfType<EnemyController>())
        {
            Vector3 dir = ec.transform.position - transform.position;
            if (dir.magnitude < pushBackDistance)
            {
                dir = Vector3.Normalize(dir.normalized + Vector3.up * 0.75f);
                ec.PushEnemy(dir, pushBackForce);
            }
        }
    }


    void StartGhost()
    {
        if (ghostMovement) return;
        ghost.Show(transform);

        //ghost.transform.position = transform.position;

        Ray mouseRay = Camera.main.ScreenPointToRay(Input.mousePosition);
        float yDist = mouseRay.origin.y - ghost.transform.position.y;
        Vector3 rayPoint = mouseRay.origin - mouseRay.direction * (yDist / mouseRay.direction.y);
        Quaternion toRot = Quaternion.LookRotation(rayPoint - ghost.transform.position);
        ghost.transform.rotation = toRot;

        ghostMovement = true;
        ghost.StartMarking();
    }
    void StopGhost()
    {
        if (!ghostMovement) return;
        ghost.EndMarking();
        ghost.Hide();
        ghostMovement = false;
    }

    void MoveGhost()
    {
        
        Ray mouseRay = Camera.main.ScreenPointToRay(Input.mousePosition);
        float yDist = mouseRay.origin.y - ghost.transform.position.y;
        Vector3 rayPoint = mouseRay.origin - mouseRay.direction * (yDist / mouseRay.direction.y);
        Quaternion toRot = Quaternion.LookRotation(rayPoint - ghost.transform.position);
        ghost.transform.rotation = Quaternion.Lerp(ghost.transform.rotation, toRot, Time.deltaTime * 7f);
        

        Vector3 moveBy = Vector3.forward * ghostSpeed * Time.deltaTime;
        ghost.transform.Translate(moveBy, Space.Self);

        Vector3 heading = ghost.transform.position - transform.position;
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

    public void TakeDamage(float damage)
    {
        health -= damage;
        Debug.Log("Health remaining: " + health);
        if (health <= 0)
        {
            KillPlayer();
        }
    }

    public void KillPlayer()
    {
        SceneManager.LoadScene(SceneManager.GetActiveScene().buildIndex);
    }
}
