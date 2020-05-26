using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class TankController : EnemyController
{
    public float meleeRange = 1.5f;


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
                    ChasePlayer();
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
        }


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
}
