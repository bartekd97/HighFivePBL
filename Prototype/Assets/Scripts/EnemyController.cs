using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class EnemyController : MonoBehaviour
{
    private Rigidbody rb;
    public float speed = 3.0f;
    public float playerInRange = 15.0f;
    public float stoppingDistance = 1.5f;
    public float retreatDistance = 0.75f;
    public GameObject player;
    public float maxHealth = 10.0f;

    public Color defaultColor;
    public Color damagedColor;
    public float dmgAnimationDuration = 0.5f;
    private float lastDmgTime = 0.0f;

    public float slow = 0.0f;
    public float mudSlow = 0.0f;
    public float frozenTo = 0.0f;

    private float enemyHealth;

    public CharController charController;
    public HealthBar healthBar;
    //private bool pushedEnemiess;



    //attacking
    private float meleeRange = 1.5f;
    public float attackDelay = 1.0f;
    private float timestampAttack;
    private float timestampAfterAttackPushStart;
    private float timestampAfterAttackPushStop;
    //private bool isPushedAfterAttack;
    public float pushBackForceAfterAttack = 5.0f;
    public float damage = 1.0f;

    bool isPoisoned;
    public float poisonCooldownTime = 1.0f;
    float nextPoisonTime = 0.0f;
    float poisoningStart = -1.0f;
    float poisoningEnd = -1.0f;

    bool isBurnt;
    public float burningCooldownTime = 1.0f;
    float nextBurnTime = 0.0f;

    public bool isMelee = false;

    public GameObject arrow;
    public float arrowSpeed = 10.0f;
    public float shootingRange = 10.0f;
    public float shootingDelay = 3.0f;
    private bool isMoving = false;

    private bool enemyTriggered = false;
    private Quaternion startRotation;

    // Start is called before the first frame update
    void Start()
    {
        rb = GetComponent<Rigidbody>();
        timestampAttack = 0.0f;
        timestampAfterAttackPushStart = 0.0f;
        timestampAfterAttackPushStop = 0.0f;
        enemyHealth = maxHealth;
        healthBar.SetMaxHealth(maxHealth);
        SetMeshColor(defaultColor);
        startRotation = transform.rotation;
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
        //Vector3 moveDirection = Vector3.zero;
        Vector3 pushDirection = Vector3.zero;
        //pushedEnemiess = charController.pushedEnemies;

        //if (pushedEnemiess == true && Vector3.Distance(transform.position, player.transform.position) <= (stoppingDistance * 10.0f))
        //{
        //    PushEnemy(pushDirection);
        //}

        if (Time.time > frozenTo)
        {
            if (isMelee == true)
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
                else if (Vector3.Distance(transform.position, player.transform.position) <= playerInRange && Vector3.Distance(transform.position, player.transform.position) < stoppingDistance
                                                                && Vector3.Distance(transform.position, player.transform.position) > retreatDistance)
                {
                    Stop();
                }
            }
            
            else if (isMelee == false)
            {
                if (Vector3.Distance(transform.position, player.transform.position) <= playerInRange && Vector3.Distance(transform.position, player.transform.position) > stoppingDistance)
                {
                    ChasePlayer();
                    isMoving = true;
                    /*
                    if (pushedEnemiess == true && Vector3.Distance(transform.position, player.transform.position) <= (stoppingDistance * 3.0f))
                    {
                        PushEnemy(pushDirection);
                    }
                    */
                }
                else if (Vector3.Distance(transform.position, player.transform.position) <= playerInRange && Vector3.Distance(transform.position, player.transform.position) < stoppingDistance
                                                                && Vector3.Distance(transform.position, player.transform.position) > retreatDistance)
                {
                    //Stop();
                    MoveToGetBetterPosition();
                    isMoving = false;
                }
                else if (Vector3.Distance(transform.position, player.transform.position) < retreatDistance)
                {
                    RunAwayFromPlayer();
                    isMoving = true;
                }
                else if (Vector3.Distance(transform.position, player.transform.position) <= stoppingDistance)
                {
                    Stop();
                    isMoving = false;
                }
            }
        }
        

        if (Vector3.Distance(transform.position, player.transform.position) <= meleeRange && isMelee == true)
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
        else if (Vector3.Distance(transform.position, player.transform.position) <= shootingRange && isMelee == false)
        {
            CheckIfEnemySeePlayer();
            if (timestampAttack <= Time.time && enemyTriggered == true)
            {
                timestampAttack = Time.time + shootingDelay;
                if (isMoving == false)
                {
                    ShootWithArrow();
                }
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

    void ChasePlayer()
    {
        Vector3 direction = ((Vector3)player.transform.position - transform.position).normalized;
        Quaternion lookRotation = Quaternion.LookRotation(new Vector3(direction.x, 0, direction.z));
        transform.rotation = Quaternion.Slerp(transform.rotation, lookRotation, Time.deltaTime * 5f);
        //transform.position = Vector3.Slerp(transform.position, transform.position + direction * speed, Time.deltaTime);
        rb.MovePosition(transform.position + direction * (speed - slow - mudSlow) * Time.deltaTime);
    }

    void RunAwayFromPlayer()
    {
        Vector3 direction = ((Vector3)player.transform.position - transform.position).normalized;
        Quaternion lookRotation = Quaternion.LookRotation(new Vector3(-direction.x, 0, -direction.z));
        transform.rotation = Quaternion.Slerp(transform.rotation, lookRotation, Time.deltaTime * 5f);
        //transform.position = Vector3.Slerp(transform.position, transform.position + direction * speed, Time.deltaTime);
        rb.MovePosition(transform.position + direction * (-speed - slow - mudSlow) * Time.deltaTime);
    }

    void Stop()
    {
        rb.velocity = Vector3.zero;
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
        if (enemyTriggered == false && Vector3.Distance(transform.position, player.transform.position) < stoppingDistance && Vector3.Distance(transform.position, player.transform.position) > (retreatDistance + 0.5f))
        {
            startRotation = Quaternion.AngleAxis(-90, transform.forward);

            transform.rotation = Quaternion.Lerp(transform.rotation, startRotation, speed/10 * Time.deltaTime);

            //Vector3 direction = Vector3.right;
            rb.MovePosition(transform.position + Vector3.forward * speed * Time.deltaTime);
        }
    }

    public void TakeDamage(float value)
    {
        lastDmgTime = Time.time;
        enemyHealth -= value;
        healthBar.SetHealth(enemyHealth);

        if (enemyHealth <= 0)
        {
            Destroy(gameObject);
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

    public void PushEnemy(Vector3 direction, float force)
    {
        //direction = ((Vector3)transform.position - player.transform.position).normalized;


        //rb.MovePosition(transform.position + direction * speed * Time.deltaTime * 60.0f);
        rb.AddForce(direction * force, ForceMode.VelocityChange);
    }

    private void SetMeshColor(Color color)
    {
        Renderer[] renderers = GetComponentsInChildren<Renderer>();
        for (int i = 0; i < renderers.Length; i++)
        {
            renderers[i].material.SetColor("_BaseColor", color);
        }
    }

    private void CalculateColor()
    {
        float R = Time.time - lastDmgTime;
        if (R <= dmgAnimationDuration)
        {
            float halfDuration = dmgAnimationDuration / 2.0f;
            if (R > halfDuration) R = dmgAnimationDuration - R;
            float wsp = R / halfDuration;
            SetMeshColor(((1.0f - wsp) * defaultColor) + (wsp * damagedColor));
        }
    }

    private void OnTriggerEnter(Collider other)
    {
        if (other.CompareTag("Mud"))
        {
            mudSlow = 2.0f;
        }
        if (other.CompareTag("MudOut"))
        {
            mudSlow = 0.0f;
        }
        if (other.CompareTag("ToxicFog"))
        {
            isPoisoned = true;
        }
        if (other.CompareTag("ToxicFogOut") && isPoisoned == true)
        {
            isPoisoned = false;
            poisoningStart = Time.time;
            poisoningEnd = Time.time + 2.0f;
        }
        if (other.CompareTag("Fire"))
        {
            isBurnt = true;
        }
        if (other.CompareTag("FireOut"))
        {
            isBurnt = false;
        }
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
