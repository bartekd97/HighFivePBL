using System.Collections;
using System.Collections.Generic;
using System.Linq;
using UnityEditor.Experimental.GraphView;
using UnityEngine;

public class FireBossController : EnemyController
{
    public GameObject preFirewall;
    public GameObject firewall;
    public GameObject preFireball;
    public GameObject fireball;

    public GameObject thrower;


    public float attackCooldownTime = 3.0f;
    public float attackCooldownTimePhase2 = 1.5f;

    public float nextAttackTime = 0.0f;

    public float fireWallRange = 10.0f;

    public float retreatDistance = 6.75f;

    public float magicTime = 15.0f;





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
        //if ((isBurnt) && Time.time >= nextBurnTime)
        //{
        //    TakeDamage(1.0f);
        //    nextBurnTime = Time.time + burningCooldownTime;
        //}

        CalculateColor();

        Vector3 pushDirection = Vector3.zero;
        UnityEngine.Debug.Log(cellNumber);

        if (cellNumber == GameManager.Instance.currentCell.CellSiteIndex)
        {
            LookAtPlayer();
            if (nextAttackTime < Time.time && Vector3.Distance(transform.position, player.transform.position) <= playerInRange)
            {
                if (Vector3.Distance(transform.position, player.transform.position) <= fireWallRange)
                {
                    ThrowFirewall();
                }
                else
                {
                    ThrowFireball();
                }
                nextAttackTime = attackCooldownTime + Time.time;
            }
            if (Vector3.Distance(transform.position, player.transform.position) < retreatDistance)
            {
                RunAwayFromPlayer();
            }
        }

        if(enemyHealth < maxHealth / 2)
        {
            attackCooldownTime = attackCooldownTimePhase2;
        }
    }

    void RunAwayFromPlayer()
    {
        UnityEngine.Debug.Log("test");

        Vector3 direction = ((Vector3)player.transform.position - transform.position).normalized;
        //Quaternion lookRotation = Quaternion.LookRotation(new Vector3(-direction.x, 0, -direction.z));
        //transform.rotation = Quaternion.Slerp(transform.rotation, lookRotation, Time.deltaTime * 5f);
        //transform.position = Vector3.Slerp(transform.position, transform.position + direction * speed, Time.deltaTime);
        rb.MovePosition(transform.position + direction * (-speed - slow - mudSlow) * Time.deltaTime);
    }

    void LookAtPlayer()
    {
        transform.LookAt(new Vector3(player.transform.position.x, 1.0f, player.transform.position.z));
    }

    private IEnumerator Fireball(float waitTime, Vector3 fireballPosition)
    {
            yield return new WaitForSeconds(waitTime);
            GameObject newFire = GameObject.Instantiate(fireball, fireballPosition, player.transform.rotation);
            Destroy(newFire, magicTime);
    }

    private IEnumerator Firewall(float waitTime, Vector3 firewallPosition, Quaternion firewallRotation)
    {
        yield return new WaitForSeconds(waitTime);
        GameObject newFire = GameObject.Instantiate(firewall, firewallPosition, firewallRotation);
        Destroy(newFire, magicTime);
    }

    private void ThrowFireball()
    {
        Vector3 fireballPosition = new Vector3(player.transform.position.x, 0.02f, player.transform.position.z);
        GameObject newPreFire = GameObject.Instantiate(preFireball, fireballPosition, player.transform.rotation);
        Destroy(newPreFire, 2.0f);
        StartCoroutine(Fireball(2.0f, fireballPosition));
    }

    private void ThrowFirewall()
    {
        Vector3 firewallPosition = new Vector3(thrower.transform.position.x, 0.02f, thrower.transform.position.z);
        Quaternion firewallRotation = transform.rotation;

        GameObject newPreFire = GameObject.Instantiate(preFirewall, firewallPosition, firewallRotation);
        Destroy(newPreFire, 1.0f);
        StartCoroutine(Firewall(1.0f, firewallPosition, firewallRotation));
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
