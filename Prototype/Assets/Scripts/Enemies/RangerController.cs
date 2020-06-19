using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class RangerController : EnemyController
{
    public float retreatDistance = 0.75f;

    public GameObject arrow;
    public float arrowSpeed = 10.0f;
    public float shootingRange = 10.0f;
    public float shootingDelay = 3.0f;
    public bool canShoot = false;

    public bool enemyTriggered = false;
    public Quaternion startRotation;

    //private int state;
    public float movingDistance = 10.0f;
    public Vector3 position1;
    public Vector3 position2;
    private int startSpot;
    Vector3[] movePositions = new Vector3[2];
    private float waitTime;
    public float startWaitTime = 1.0f;
    private bool updatedPositions = false;

    //public GameObject pointPrefab;

    // Start is called before the first frame update
    void Start()
    {
        charController = GameManager.Instance.player;
        player = charController.gameObject;
        rb = GetComponent<Rigidbody>();
        timestampAttack = 0.0f;
        timestampAfterAttackPushStart = 0.0f;
        timestampAfterAttackPushStop = 0.0f;
        enemyHealth = maxHealth;
        healthBar.SetMaxHealth(maxHealth);
        SetMeshColor(defaultColor);
        startRotation = transform.rotation;
        startSpot = 0;
    }

    // Update is called once per frame
    void Update()
    {
        if ((isPoisoned) && Time.time >= nextPoisonTime)
        {
            TakeDamage(0.5f);
            nextPoisonTime = Time.time + poisonCooldownTime;
        }
        if ((poisoningEnd >= Time.time && Time.time >= poisoningStart) && Time.time >= nextPoisonTime)
        {
            TakeDamage(0.5f);
            nextPoisonTime = Time.time + poisonCooldownTime;
        }
        if ((isBurnt) && Time.time >= nextBurnTime)
        {
            TakeDamage(1.0f);
            nextBurnTime = Time.time + burningCooldownTime;
        }

        CalculateColor();
        if (transform.position.y < -10)
        {
            Destroy(gameObject);
        }
        Vector3 pushDirection = Vector3.zero;


        if (Time.time > frozenTo)
        {
            Debug.Log("state: " + updatedPositions);
            if (Vector3.Distance(transform.position, player.transform.position) <= playerInRange && Vector3.Distance(transform.position, player.transform.position) > stoppingDistance)
            {
                ChasePlayer();
                canShoot = false;
                updatedPositions = false;
            }
            else if (Vector3.Distance(transform.position, player.transform.position) <= playerInRange && Vector3.Distance(transform.position, player.transform.position) < stoppingDistance
                                                            && Vector3.Distance(transform.position, player.transform.position) > retreatDistance + 1.0f)
            {
                if (enemyTriggered == false)
                {
                    MoveToGetBetterPosition();
                }
                canShoot = true;
                updatedPositions = true;
            }
            else if (Vector3.Distance(transform.position, player.transform.position) <= playerInRange && Vector3.Distance(transform.position, player.transform.position) < (retreatDistance + 1.0f)
                                                            && Vector3.Distance(transform.position, player.transform.position) > retreatDistance)
            {
                canShoot = true;
                updatedPositions = true;
            }
            else if (Vector3.Distance(transform.position, player.transform.position) < retreatDistance)
            {
                RunAwayFromPlayer();
                canShoot = false;
                updatedPositions = false;
            }
        }


        if (Vector3.Distance(transform.position, player.transform.position) <= shootingRange)
        {
            CheckIfEnemySeePlayer();
            if (timestampAttack <= Time.time && enemyTriggered == true)
            {
                timestampAttack = Time.time + shootingDelay;
                if (canShoot == true)
                {
                    ShootWithArrow();
                }
            }
        }
    }

    void RunAwayFromPlayer()
    {
        Vector3 direction = ((Vector3)player.transform.position - transform.position).normalized;
        Quaternion lookRotation = Quaternion.LookRotation(new Vector3(-direction.x, 0, -direction.z));
        transform.rotation = Quaternion.Slerp(transform.rotation, lookRotation, Time.deltaTime * 5f);
        rb.MovePosition(transform.position + direction * (-speed - slow - mudSlow) * Time.deltaTime);
    }

    void CheckIfEnemySeePlayer()
    {
        var heading = player.transform.position - transform.position;
        var distance = heading.magnitude;
        var direction = (heading / distance);
        RaycastHit hit;
        //Debug.DrawRay(transform.position, direction);

        if (Vector2.Distance(transform.position, player.transform.position) < playerInRange)
        {
            if (Physics.Raycast(transform.position, direction, out hit, playerInRange))
            {
                if (hit.collider != null && hit.collider.gameObject == player)
                {
                    enemyTriggered = true;
                }
                else
                {
                    enemyTriggered = false;
                }
            }
        }
    }

    void MoveToGetBetterPosition()
    {
        if (updatedPositions == false)
        {
            movePositions[0] = transform.position;
            movePositions[1] = transform.position;

            Vector3 vNormalized = (player.transform.position - transform.position).normalized;
            Vector3 vPerpendicular = new Vector3(vNormalized.z, vNormalized.y, -vNormalized.x).normalized;
            Vector3 vPerpendicular2 = new Vector3(-vNormalized.z, vNormalized.y, vNormalized.x).normalized;
            Vector3 P4 = new Vector3(transform.position.x, transform.position.y, transform.position.z) + (vPerpendicular * movingDistance);
            Vector3 P3 = new Vector3(transform.position.x, transform.position.y, transform.position.z) + (vPerpendicular2 * movingDistance);

            movePositions[0] = P4;
            movePositions[1] = P3;

            //Instantiate(pointPrefab, movePositions[0], transform.rotation);
            //Instantiate(pointPrefab, movePositions[1], transform.rotation);
        }

        Vector3 direction = (movePositions[startSpot] - transform.position).normalized;
        rb.MovePosition(transform.position + direction * speed * Time.deltaTime);

        if (Vector2.Distance(transform.position, movePositions[startSpot]) < 0.4f)
        {
            if (waitTime <= 0.0f)
            {
                startSpot += 1;
                waitTime = startWaitTime;

                if (startSpot > 1)
                {
                    startSpot = 0;
                }
            }
            else
            {
                waitTime -= Time.deltaTime;
            }
        }
    }

    void ShootWithArrow()
    {
        Vector3 arrowPosition = transform.position;
        arrowPosition.y += 2.0f;

        Quaternion arrowRotation = transform.rotation;
        //arrowRotation.x += 30.0f;

        Vector3 direction = ((Vector3)player.transform.position - transform.position).normalized;
        //direction = Vector3.Normalize(direction.normalized + Vector3.up * 0.75f);

        GameObject newArrow = GameObject.Instantiate(arrow, arrowPosition, arrowRotation);
        newArrow.SetActive(true);
        Rigidbody rbArrow = newArrow.GetComponent<Rigidbody>();
        rbArrow.AddForce(direction * arrowSpeed, ForceMode.Impulse);
        //rbArrow.velocity = Vector3.zero;
        //rbArrow.velocity = direction * arrowSpeed;
    }

}
