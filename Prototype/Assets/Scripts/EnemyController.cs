using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class EnemyController : MonoBehaviour
{
    private Rigidbody rb;
    private float speed = 3.0f;
    public float playerInRange = 15.0f;
    public float stoppingDistance = 1.5f;
    public GameObject player;

    private float enemyHealth = 10.0f;

    public CharController charController;
    //private bool pushedEnemiess;



    //attacking
    private float attackRange = 1.5f;
    private float attackDelay = 1.0f;
    private float timestampAttack;
    private float timestampAfterAttackPushStart;
    private float timestampAfterAttackPushStop;
    //private bool isPushedAfterAttack;
    public float pushBackForceAfterAttack = 5.0f;
    public float damage = 1.0f;

    // Start is called before the first frame update
    void Start()
    {
        rb = GetComponent<Rigidbody>();
        timestampAttack = 0.0f;
        timestampAfterAttackPushStart = 0.0f;
        timestampAfterAttackPushStop = 0.0f;
    }

    // Update is called once per frame
    void Update()
    {
        if(transform.position.y < -50)
        {
            Destroy(gameObject);
        }
        Vector3 moveDirection = Vector3.zero;
        Vector3 pushDirection = Vector3.zero;
        //pushedEnemiess = charController.pushedEnemies;

        //if (pushedEnemiess == true && Vector3.Distance(transform.position, player.transform.position) <= (stoppingDistance * 10.0f))
        //{
        //    PushEnemy(pushDirection);
        //}

        if (Vector3.Distance(transform.position, player.transform.position) <= playerInRange && Vector3.Distance(transform.position, player.transform.position) > stoppingDistance)
        {
            ChasePlayer(moveDirection);
            /*
            if (pushedEnemiess == true && Vector3.Distance(transform.position, player.transform.position) <= (stoppingDistance * 3.0f))
            {
                PushEnemy(pushDirection);
            }
            */
        }
        else if (Vector3.Distance(transform.position, player.transform.position) <= stoppingDistance)
        {
            Stop();
        }

        

        if (Vector3.Distance(transform.position, player.transform.position) <= attackRange)
        {
            //Debug.Log("eldorado");

            if (timestampAttack <= Time.time)
            {
                timestampAttack = Time.time + attackDelay;
                player.GetComponent<CharController>().TakeDamage(damage);

                //isPushedAfterAttack = true;
                timestampAfterAttackPushStart = Time.time;
                timestampAfterAttackPushStop = Time.time + 0.1f;

                // push after attack
                Vector3 dir = transform.position - player.transform.position;
                dir = Vector3.Normalize(dir.normalized + Vector3.up * 0.75f);
                PushEnemy(dir, pushBackForceAfterAttack);
            }
        }


        /*
        if (timestampAfterAttackPushStart >= timestampAfterAttackPushStop)
        {
            isPushedAfterAttack = false;
        }
        if (isPushedAfterAttack)
        {
            Debug.Log("eldo");
            Vector3 afterAttackPushDirection = ((Vector3)transform.position - player.transform.position).normalized;
            transform.position = Vector3.Slerp(transform.position, transform.position + afterAttackPushDirection, 0.15f);
            timestampAfterAttackPushStart = Time.time;
        }
        */
    }

    void ChasePlayer(Vector3 direction)
    {
        direction = ((Vector3)player.transform.position - transform.position).normalized;
        Quaternion lookRotation = Quaternion.LookRotation(new Vector3(direction.x, 0, direction.z));
        transform.rotation = Quaternion.Slerp(transform.rotation, lookRotation, Time.deltaTime * 5f);
        //transform.position = Vector3.Slerp(transform.position, transform.position + direction * speed, Time.deltaTime);

        rb.MovePosition(transform.position + direction * speed * Time.deltaTime);
    }

    void Stop()
    {
        rb.velocity = Vector3.zero;
    }

    private void OnTriggerEnter(Collider other)
    {
        Debug.Log("Enemy is hit");
        enemyHealth -= 5;

        if (enemyHealth <= 0)
        {
            Destroy(gameObject);
        }
    }

    public void PushEnemy(Vector3 direction, float force)
    {
        //direction = ((Vector3)transform.position - player.transform.position).normalized;


        //rb.MovePosition(transform.position + direction * speed * Time.deltaTime * 60.0f);
        rb.AddForce(direction * force, ForceMode.VelocityChange);
    }

}
