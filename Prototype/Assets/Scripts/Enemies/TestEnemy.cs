using System.Collections;
using System.Collections.Generic;
using System.Diagnostics;
using UnityEngine;

public class TestEnemy : EnemyController
{
    public float meleeRange = 2f;
    public bool enemyTriggered = false;


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
        pathfinding = new PathFinding(cell);
        
    }

    // Update is called once per frame
    void Update()
    {
        //UnityEngine.Debug.Log(mudSlow);
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

        if (cell.CellSiteIndex == GameManager.Instance.currentCell.CellSiteIndex)
        {
            CheckIfEnemySeePlayer();
            if (!enemyTriggered)
            {
                List<MapCell.PathNode> path = pathfinding.FindPath(transform.position.x, transform.position.z, player.transform.position.x, player.transform.position.z);
                for (int i = 0; i < path.Count; i++)
                {
                    Destroy(Instantiate(pointPrefab, new Vector3(path[i].x, 0, path[i].y), Quaternion.identity), 0.02f);
                }
                if (path != null)
                {
                    if (path.Count > 0)
                    {
                        Vector3 direction = (new Vector3(path[1].x, 0, path[1].y) - transform.position).normalized;
                        Quaternion lookRotation = Quaternion.LookRotation(new Vector3(direction.x, 0, direction.z));
                        transform.rotation = Quaternion.Slerp(transform.rotation, lookRotation, Time.deltaTime * 5f);
                        //transform.position = Vector3.Slerp(transform.position, transform.position + direction * speed, Time.deltaTime);
                        rb.MovePosition(transform.position + direction * (speed - slow - mudSlow) * Time.deltaTime);
                    }
                }
            }
            else
            {
                ChasePlayer();

            }



            //for (int i = 0; i < path.Count; i++)
            //{
            //    Destroy(Instantiate(pointPrefab, new Vector3(path[i].x, 0, path[i].y), Quaternion.identity), 0.02f);
            //}

            if (Vector3.Distance(transform.position, player.transform.position) <= meleeRange)
            {
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

        }
        //List<MapCell.PathNode> path = pathfinding.FindPath(transform.position.x, transform.position.z, player.transform.position.x, player.transform.position.z);
        //for (int i = 0; i < path.Count; i++)
        //{
        //    Destroy(Instantiate(pointPrefab, new Vector3(path[i].x, 0, path[i].y), Quaternion.identity), 0.02f);
        //}
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

   
}
