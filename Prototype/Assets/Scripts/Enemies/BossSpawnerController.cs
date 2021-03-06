﻿using System.Collections;
using System.Collections.Generic;
using System.Linq;
using UnityEditor.Experimental.GraphView;
using UnityEngine;

public class BossSpawnerController : EnemyController
{
    public float retreatDistance = 0.75f;

    private float timestampSpawn;
    public float spawnDelay = 3.0f;
    public GameObject enemy;
    public int enemyWaveNumber = 3;
    private int startWaveNumber = 0;

    bool isMoving = false;
    private float timestampMovement = 2.0f;
    public float delayMovement = 2.0f;
    public float movingDistance = 6.0f;
    private Vector3 positionStart;
    private Vector3 position1;
    private Vector3 position2;
    private Vector3[] movePositions = new Vector3[2];
    private int startSpot;
    private float waitTime;
    public float startWaitTime;
    private bool updatedPositions = false;

    // Start is called before the first frame update
    void Start()
    {
        charController = GameManager.Instance.player;
        player = charController.gameObject;

        rb = GetComponent<Rigidbody>();
        //timestampAttack = 0.0f;
        //timestampAfterAttackPushStart = 0.0f;
        //timestampAfterAttackPushStop = 0.0f;
        enemyHealth = maxHealth;
        healthBar.SetMaxHealth(maxHealth);
        SetMeshColor(defaultColor);
        //timestampMovement = 0.0f;
        startSpot = 0;
        positionStart = this.transform.position;
        position1 = this.transform.position;
        position2 = this.transform.position;
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
        /*if (transform.position.y < -10)
        {
            Destroy(gameObject);
        }*/
        //Vector3 moveDirection = Vector3.zero;
        Vector3 pushDirection = Vector3.zero;
        //pushedEnemiess = charController.pushedEnemies;

        //if (pushedEnemiess == true && Vector3.Distance(transform.position, player.transform.position) <= (stoppingDistance * 10.0f))
        //{
        //    PushEnemy(pushDirection);
        //}


        if (Vector3.Distance(transform.position, player.transform.position) <= playerInRange && Vector3.Distance(transform.position, player.transform.position) > stoppingDistance)
        {
            //ChasePlayer();
            LookAtPlayer();
            MoveToGetBetterPosition();
            isMoving = true;
            updatedPositions = true;
            /*
            if (pushedEnemiess == true && Vector3.Distance(transform.position, player.transform.position) <= (stoppingDistance * 3.0f))
            {
                PushEnemy(pushDirection);
            }
            */
        }
        else if (Vector3.Distance(transform.position, player.transform.position) <= playerInRange && Vector3.Distance(transform.position, player.transform.position) < stoppingDistance
                                                        && Vector3.Distance(transform.position, player.transform.position) > (retreatDistance + 1.0f))
        {
            //Stop();
            LookAtPlayer();
            //MoveSideToSide();
            isMoving = false;
            updatedPositions = false;
        }
        else if (Vector3.Distance(transform.position, player.transform.position) <= playerInRange && Vector3.Distance(transform.position, player.transform.position) < (retreatDistance + 1.0f)
                                                        && Vector3.Distance(transform.position, player.transform.position) > retreatDistance)
        {
            //MoveSideToSide();
            isMoving = false;
            updatedPositions = false;
        }
        else if (Vector3.Distance(transform.position, player.transform.position) < retreatDistance)
        {
            RunAwayFromPlayer();
            isMoving = true;
            updatedPositions = false;
        }
        else if (Vector3.Distance(transform.position, player.transform.position) < stoppingDistance)
        {
            //Stop();
            //MoveSideToSide();
            LookAtPlayer();
        }

        if (Vector3.Distance(transform.position, player.transform.position) <= playerInRange)
        {
            //Debug.Log("eldorado");

            if (timestampSpawn <= Time.time && startWaveNumber < enemyWaveNumber)
            {
                timestampSpawn = Time.time + spawnDelay;
                //player.GetComponent<CharController>().TakeDamage(damage);

                //isPushedAfterAttack = true;
                //timestampAfterAttackPushStart = Time.time;
                //timestampAfterAttackPushStop = Time.time + 0.1f;

                // push after attack
                //Vector3 dir = transform.position - player.transform.position;
                //dir = Vector3.Normalize(dir.normalized + Vector3.up * 0.75f);
                //PushEnemy(dir, pushBackForceAfterAttack);
                Vector3 startPosition = transform.position;

                Vector3 startDirection = transform.forward;
                //Quaternion playerRotation = player.transform.rotation;
                float spawnDistance = 2;

                Vector3 spawnPos = startPosition + startDirection * spawnDistance;

                for (int i = 1; i <= 3; i++)
                {
                    spawnPos.x += i * spawnDistance;
                    //spawnPos.z += i * spawnDistance;
                    GameObject newEnemy = GameObject.Instantiate(enemy, spawnPos, transform.rotation);
                }
                startWaveNumber++;

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
    /*
    void ChasePlayer()
    {
        Vector3 direction = ((Vector3)player.transform.position - transform.position).normalized;
        Quaternion lookRotation = Quaternion.LookRotation(new Vector3(direction.x, 0, direction.z));
        transform.rotation = Quaternion.Slerp(transform.rotation, lookRotation, Time.deltaTime * 5f);
        //transform.position = Vector3.Slerp(transform.position, transform.position + direction * speed, Time.deltaTime);
        rb.MovePosition(transform.position + direction * (speed - slow - mudSlow) * Time.deltaTime);
    }
    */
    void RunAwayFromPlayer()
    {
        Vector3 direction = ((Vector3)player.transform.position - transform.position).normalized;
        Quaternion lookRotation = Quaternion.LookRotation(new Vector3(-direction.x, 0, -direction.z));
        transform.rotation = Quaternion.Slerp(transform.rotation, lookRotation, Time.deltaTime * 5f);
        //transform.position = Vector3.Slerp(transform.position, transform.position + direction * speed, Time.deltaTime);
        rb.MovePosition(transform.position + direction * (-speed - slow - mudSlow) * Time.deltaTime);
    }
    /*
    void MoveSideToSide()
    {
        //transform.LookAt(transform.forward);
        //transform.position = Vector2.MoveTowards(transform.position, moveSpots[startSpot], speed);
        Vector3 direction2 = (moveSpots[startSpot] - transform.position);
        rb.MovePosition(transform.position + direction2 * speed * Time.deltaTime);

        if (Vector2.Distance(transform.position, moveSpots[startSpot]) < 0.2f)
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
    */
    void MoveToGetBetterPosition()
    {
        //startRotation = Quaternion.AngleAxis(-90, transform.forward);
        //transform.rotation = Quaternion.Lerp(transform.rotation, startRotation, speed / 10 * Time.deltaTime);
        //Vector3 direction = Vector3.right;
        //rb.MovePosition(transform.position + Vector3.forward * speed * Time.deltaTime);

        if (updatedPositions == false)
        {
            movePositions[0] = transform.position;
            //movePositions[0].x += movingDistance;
            //movePositions[0].z += movingDistance;
            movePositions[1] = transform.position;
            //movePositions[1].x -= movingDistance;
            //movePositions[1].z -= movingDistance;

            Vector3 vNormalized = (player.transform.position - transform.position).normalized;

            Vector3 vPerpendicular = new Vector3(vNormalized.z, vNormalized.y, -vNormalized.x).normalized;
            Vector3 vPerpendicular2 = new Vector3(-vNormalized.z, vNormalized.y, vNormalized.x).normalized;
            Vector3 P4 = new Vector3(transform.position.x, transform.position.y, transform.position.z) + (vPerpendicular * movingDistance);
            Vector3 P3 = new Vector3(transform.position.x, transform.position.y, transform.position.z) + (vPerpendicular2 * movingDistance);

            movePositions[0] = P4;
            movePositions[1] = P3;

            Instantiate(pointPrefab, movePositions[0], transform.rotation);
            Instantiate(pointPrefab, movePositions[1], transform.rotation);
        }
        //Debug.Log("pos1= " + position1 + ", pos2= " + position2);

        Vector3 direction = (movePositions[startSpot] - transform.position).normalized;
        rb.MovePosition(transform.position + direction * speed * Time.deltaTime);

        if (Vector2.Distance(transform.position, movePositions[startSpot]) < 0.4f)
        {
            Debug.Log("no hej");
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

    void LookAtPlayer()
    {
        transform.LookAt(player.transform);
    }
    

    string output;

    public override string ToString()
    {
        output = "";
        output += "<component name=\"ScriptComponent\">";
        output += "<property name=\"name\" value=\"" + this.name + "\"/>";
        output += "</component>";
        return output;
    }
}
