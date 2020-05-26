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

    private int state;
    public float movingDistance = 1.0f;
    public Vector3 position1;
    public Vector3 position2;
    private int startSpot;
    Vector3[] movePositions = new Vector3[2];
    private float waitTime;
    public float startWaitTime = 1.0f;

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
        state = 0;
        movePositions[0] = position1;
        movePositions[1] = position2;
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
            if (Vector3.Distance(transform.position, player.transform.position) <= playerInRange && Vector3.Distance(transform.position, player.transform.position) > stoppingDistance)
            {
                state = 1; //gonienie
                ChasePlayer();
                canShoot = false;
            }
            else if (Vector3.Distance(transform.position, player.transform.position) <= playerInRange && Vector3.Distance(transform.position, player.transform.position) < stoppingDistance
                                                            && Vector3.Distance(transform.position, player.transform.position) > retreatDistance)
            {
                if (state != 3)
                    state = 2; //szczelanie, przed policzeniem punktow
                if (enemyTriggered == false)
                {
                    MoveToGetBetterPosition();
                }

                canShoot = true;
                state = 3; //szczelanie, punkty policzone
                
            }
            else if (Vector3.Distance(transform.position, player.transform.position) < retreatDistance)
            {
                state = 4; //ucieczka
                RunAwayFromPlayer();
                canShoot = false;
            }
            else if (Vector3.Distance(transform.position, player.transform.position) <= stoppingDistance)
            {
                state = 5; //poza zasiegiem
                Stop();
                canShoot = false;
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
        //transform.position = Vector3.Slerp(transform.position, transform.position + direction * speed, Time.deltaTime);
        rb.MovePosition(transform.position + direction * (-speed - slow - mudSlow) * Time.deltaTime);
    }

    void CheckIfEnemySeePlayer()
    {
        var heading = player.transform.position - transform.position;
        var distance = heading.magnitude;
        var direction = (heading / distance);
        RaycastHit hit;
        //Debug.Log(hit.collider.gameObject);
        Debug.DrawRay(transform.position, direction);

        if (Vector2.Distance(transform.position, player.transform.position) < playerInRange)
        {
            if (Physics.Raycast(transform.position, direction, out hit, playerInRange))
            {
                if (hit.collider != null && hit.collider.gameObject == player)
                {
                    enemyTriggered = true;
                }
                else enemyTriggered = false;
            }
        }
    }

    void MoveToGetBetterPosition()
    {
        //startRotation = Quaternion.AngleAxis(-90, transform.forward);
        //transform.rotation = Quaternion.Lerp(transform.rotation, startRotation, speed / 10 * Time.deltaTime);
        //Vector3 direction = Vector3.right;
        //rb.MovePosition(transform.position + Vector3.forward * speed * Time.deltaTime);

        if (state == 2)
        {
            position1 = transform.position;
            position1.x += movingDistance;
            position2 = transform.position; 
            position2.x -= movingDistance;
        }

        Vector3 direction = (movePositions[startSpot] - transform.position);
        rb.MovePosition(transform.position + direction * speed * Time.deltaTime);

        if (Vector2.Distance(transform.position, movePositions[startSpot]) < 0.1f)
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
