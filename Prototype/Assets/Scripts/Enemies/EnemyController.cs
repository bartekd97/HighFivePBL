using System.Collections;
using System.Collections.Generic;
using System.Diagnostics;
using UnityEngine;

public class EnemyController : MonoBehaviour
{
    public GameObject player;
    public CharController charController;
    public GameManager gameManager;
    public HealthBar healthBar;

    public Rigidbody rb;
    public float speed = 3.0f;
    public float playerInRange = 15.0f;
    public float stoppingDistance = 1.5f;
    public float maxHealth = 10.0f;
    public float enemyHealth;

    public int cellNumber;


    public Color defaultColor;
    public Color damagedColor;
    public float dmgAnimationDuration = 0.5f;
    public float lastDmgTime = 0.0f;

    public float slow = 0.0f;
    public float mudSlow = 0.0f;
    public float frozenTo = 0.0f;

    public bool isPoisoned;
    public float poisonCooldownTime = 1.0f;
    public float nextPoisonTime = 0.0f;
    public float poisoningStart = -1.0f;
    public float poisoningEnd = -1.0f;

    public bool isBurnt;
    public float burningCooldownTime = 1.0f;
    public float nextBurnTime = 0.0f;

    //attacking
    public float attackDelay = 1.0f;
    public float timestampAttack;
    public float timestampAfterAttackPushStart;
    public float timestampAfterAttackPushStop;
    //private bool isPushedAfterAttack;
    public float pushBackForceAfterAttack = 5.0f;
    public float damage = 1.0f;

   

    public void ChasePlayer()
    {
        Vector3 direction = ((Vector3)player.transform.position - transform.position).normalized;
        Quaternion lookRotation = Quaternion.LookRotation(new Vector3(direction.x, 0, direction.z));
        transform.rotation = Quaternion.Slerp(transform.rotation, lookRotation, Time.deltaTime * 5f);
        //transform.position = Vector3.Slerp(transform.position, transform.position + direction * speed, Time.deltaTime);
        rb.MovePosition(transform.position + direction * (speed - slow - mudSlow) * Time.deltaTime);
    }

    public void Stop()
    {
        rb.velocity = Vector3.zero;
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

    public void PushEnemy(Vector3 direction, float force)
    {
        //direction = ((Vector3)transform.position - player.transform.position).normalized;
        //rb.MovePosition(transform.position + direction * speed * Time.deltaTime * 60.0f);
        rb.AddForce(direction * force, ForceMode.VelocityChange);
    }

    public void SetMeshColor(Color color)
    {
        Renderer[] renderers = GetComponentsInChildren<Renderer>();
        for (int i = 0; i < renderers.Length; i++)
        {
            renderers[i].material.SetColor("_BaseColor", color);
        }
    }

    public void CalculateColor()
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

    public void OnTriggerEnter(Collider other)
    {
        if (other.CompareTag("Mud"))
        {
            mudSlow = 2.0f;
        }
        if (other.CompareTag("ToxicFog"))
        {
            isPoisoned = true;
        }
        if (other.CompareTag("Fire"))
        {
            isBurnt = true;
        }    
    }

    private void OnTriggerExit(Collider other)
    {
        if (other.CompareTag("Mud"))
        {
            slow = 0.0f;
        }
        if (other.CompareTag("ToxicFog"))
        {
            isPoisoned = false;
            poisoningStart = Time.time;
            poisoningEnd = Time.time + 2.0f;
        }
        if (other.CompareTag("Fire"))
        {
            isBurnt = false;
        }
    }

    //string output;

    //public override string ToString()
    //{
    //    output = "";
    //    output += "<component name=\"ScriptComponent\">";
    //    output += "<property name=\"name\" value=\"" + this.name + "\"/>";
    //    output += "</component>";
    //    return output;
    //}
}
