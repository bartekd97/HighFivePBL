using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class EnemyController : MonoBehaviour
{
    private Rigidbody rb;
    private float speed = 3.0f;
    private float playerInRange = 15.0f;
    private float stoppingDistance = 1.5f;
    public GameObject player;

    private float enemyHealth = 10.0f;


    // Start is called before the first frame update
    void Start()
    {
        rb = GetComponent<Rigidbody>();
    }

    // Update is called once per frame
    void Update()
    {
        Vector3 direction = Vector3.zero;

        if (Vector3.Distance(transform.position, player.transform.position) <= playerInRange && Vector3.Distance(transform.position, player.transform.position) > stoppingDistance)
        {
            ChasePlayer(direction);
        }
        else if (Vector3.Distance(transform.position, player.transform.position) <= stoppingDistance)
        {
            Stop();
        }
    }

    void ChasePlayer(Vector3 direction)
    {
        direction = ((Vector3)player.transform.position - transform.position).normalized;
        rb.MovePosition(transform.position + direction * speed * Time.deltaTime);
    }

    void Stop()
    {
        rb.velocity = Vector3.zero;
    }

    private void OnTriggerEnter(Collider other)
    {
        Debug.Log("eldorado");
        enemyHealth -= 5;

        if (enemyHealth <= 0)
        {
            Destroy(gameObject);
        }
    }

}
