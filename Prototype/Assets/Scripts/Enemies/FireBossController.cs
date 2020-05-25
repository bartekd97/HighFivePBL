using System.Collections;
using System.Collections.Generic;
using System.Linq;
using UnityEditor.Experimental.GraphView;
using UnityEngine;

public class FireBossController : EnemyController
{
    public GameObject firewall;
    public GameObject preFireball;
    public GameObject fireball;
    public GameObject clearFire;


    public float burningCooldownTime = 4.0f;
    public float nextBurnTime = 0.0f;

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
        ThrowFireball();
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

        Vector3 pushDirection = Vector3.zero;


        if (nextBurnTime < Time.time && Vector3.Distance(transform.position, player.transform.position) <= playerInRange)
        {
            if(true)
            {

            }




            nextBurnTime = burningCooldownTime + Time.time;
        }
  

      
    }

    private IEnumerator Fireball(float waitTime, Vector3 fireballPosition)
    {
       
            yield return new WaitForSeconds(waitTime);
            GameObject newFire = GameObject.Instantiate(fireball, fireballPosition, transform.rotation);
            Destroy(newFire, 5.0f);
    }

    private IEnumerator ClearFireball(float waitTime, Vector3 fireballPosition)
    {

        yield return new WaitForSeconds(waitTime);
        GameObject newFire = GameObject.Instantiate(clearFire, fireballPosition, transform.rotation);
        Destroy(newFire, 2.0f);
    }

    private void ThrowFireball()
    {
        Vector3 fireballPosition = player.transform.position;
        GameObject newPreFire = GameObject.Instantiate(preFireball, fireballPosition, transform.rotation);
        Destroy(newPreFire, 2.0f);
        StartCoroutine(Fireball(2.0f, fireballPosition));
        StartCoroutine(ClearFireball(6.0f, fireballPosition));
    }

    private void ThrowFirewall()
    {

        GameObject newFire = GameObject.Instantiate(firewall, transform.position, transform.rotation);
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
