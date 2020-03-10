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
    private bool pushedEnemiess;

    Vector3 moveDirection = Vector3.zero;
    Vector3 pushDirection = Vector3.zero;

    //attacking
    public float attackRange = 1.0f;
    public float attackDelay = 2.0f;
    private float timestampAttack;
    private float timestampAfterAttackPushStart;
    private float timestampAfterAttackPushStop;
    private bool isPushedAfterAttack;
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
        //Vector3 moveDirection = Vector3.zero;
        //Vector3 pushDirection = Vector3.zero;
        pushedEnemiess = charController.pushedEnemies;

        if (Vector3.Distance(transform.position, player.transform.position) <= playerInRange && Vector3.Distance(transform.position, player.transform.position) > stoppingDistance)
        {
            ChasePlayer(moveDirection);
        }
        else if (Vector3.Distance(transform.position, player.transform.position) <= stoppingDistance)
        {
            Stop();
        }

        if (pushedEnemiess == true && Vector3.Distance(transform.position, player.transform.position) <= stoppingDistance)
        {
            PushEnemy(pushDirection);
        }

        if (Vector3.Distance(transform.position, player.transform.position) <= attackRange)
        {
            if (timestampAttack <= Time.time)
            {
                timestampAttack = Time.time + attackDelay;
                player.GetComponent<CharController>().TakeDamage(damage);

                isPushedAfterAttack = true;
                timestampAfterAttackPushStart = Time.time;
                timestampAfterAttackPushStop = Time.time + 0.1f;
            }
        }


        if (timestampAfterAttackPushStart >= timestampAfterAttackPushStop)
        {
            isPushedAfterAttack = false;
        }
        if (isPushedAfterAttack)
        {
            Vector3 afterAttackPushDirection = ((Vector3)transform.position - player.transform.position).normalized;
            transform.position = Vector3.Slerp(transform.position, transform.position + afterAttackPushDirection, 0.15f);
            timestampAfterAttackPushStart = Time.time;
        }

    }

    void ChasePlayer(Vector3 direction)
    {
        direction = ((Vector3)player.transform.position - transform.position).normalized;
        Quaternion lookRotation = Quaternion.LookRotation(new Vector3(direction.x, 0, direction.z));
        transform.rotation = Quaternion.Slerp(transform.rotation, lookRotation, Time.deltaTime * 5f);
        transform.position = Vector3.Slerp(transform.position, transform.position + direction * speed, Time.deltaTime);

        //rb.MovePosition(transform.position + direction * speed * Time.deltaTime);
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

    public void PushEnemy(Vector3 direction)
    {
        direction = ((Vector3)transform.position - player.transform.position).normalized;


        rb.MovePosition(transform.position + direction * speed * Time.deltaTime * 100.0f);
        //rb.AddForce(direction * 20.0f, ForceMode.VelocityChange);
    }

}
