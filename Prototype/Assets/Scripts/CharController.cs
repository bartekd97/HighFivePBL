using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.SceneManagement;


public class CharController : MonoBehaviour
{
    public bool ghostMovement { get; private set; }

    public ParticleSystem smokePush;

    public Ghost ghost;
    public HealthBar healthBar;

    public Color defaultColor;
    public Color damagedColor;
    public float dmgAnimationDuration = 0.5f;
    private float lastDmgTime = 0.0f;

    [SerializeField]
    public float maxGhostDistance = 8.0f;
    [SerializeField]
    public float ghostDistanceRecoverySpeed = 1.0f;

    [SerializeField]
    public float speed = 4.0f;
    public float slow = 0.0f;

    [SerializeField]
    public float ghostSpeed = 8.0f;
    [SerializeField]
    public float ghostCooldownTime = 1.0f;

    [SerializeField]
    float maxHealth = 10.0f;
    private float health;

    [SerializeField]
    public float pushBackDistance = 5.0f;
    [SerializeField]
    public float pushBackForce = 10.0f;
    [SerializeField]
    public float pushCooldownTime = 1.0f;

    Vector3 forward, right;
    float leftGhostDistance;
    public float nextPushBackTime = 0.0f;
    public float nextGhostTime = 0.0f;

    //public bool pushedEnemies;

    bool isPoisoned;
    public float poisonCooldownTime = 1.0f;
    float nextPoisonTime = 0.0f;
    float poisoningStart = -1.0f;
    float poisoningEnd = -1.0f;

    bool isBurnt;
    public float burningCooldownTime = 1.0f;
    float nextBurnTime = 0.0f;

    // Start is called before the first frame update
    void Start()
    {
        forward = Camera.main.transform.forward;
        forward.y = 0;
        forward = Vector3.Normalize(forward);
        right = Quaternion.Euler(new Vector3(0, 90, 0)) * forward;
        ghostMovement = false;
        leftGhostDistance = maxGhostDistance;
        health = maxHealth;
        healthBar.SetMaxHealth(maxHealth);
        SetMeshColor(defaultColor);
        //pushedEnemies = false;
    }

    public float GetLeftGhostLevel()
    {
        return leftGhostDistance / maxGhostDistance;
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
        if (Input.GetKeyDown(KeyCode.Mouse0) && leftGhostDistance > 0.0f && Time.time >= nextGhostTime)
        {
            StartGhost();
            nextGhostTime = Time.time + ghostCooldownTime;
        }
        else if (Input.GetKeyUp(KeyCode.Mouse0))
        {
            StopGhost();
        }

        if (ghostMovement && Input.GetKey(KeyCode.Mouse0))
            MoveGhost();
        else

        if (!ghostMovement)
        {
            leftGhostDistance = Mathf.Min(
                    maxGhostDistance,
                    leftGhostDistance + Time.deltaTime * ghostDistanceRecoverySpeed
                );

            if (Input.anyKey)
                Move();
        }

        if (Input.GetKeyDown(KeyCode.Space) && Time.time >= nextPushBackTime)
        {
            PushEnemiesBack();
            nextPushBackTime = Time.time + pushCooldownTime;
        }
        if (transform.position.y < -10)
        {
            KillPlayer();
        }

        /*
        if (Input.GetKeyDown(KeyCode.Space))
        {
            pushedEnemies = true;
        }
        else if (Input.GetKeyUp(KeyCode.Space))
        {
            pushedEnemies = false;
        }
        */
    }

    void PushEnemiesBack()
    {
        smokePush.transform.eulerAngles = new Vector3(
            transform.eulerAngles.x - 90,
            transform.eulerAngles.y,
            transform.eulerAngles.z
        );

        smokePush.transform.position = new Vector3(
            transform.position.x,
            transform.position.y + 0.5f,
            transform.position.z
        );

        Instantiate(smokePush, smokePush.transform.position, smokePush.transform.rotation);

        foreach (EnemyController ec in FindObjectsOfType<EnemyController>())
        {
            Vector3 dir = ec.transform.position - transform.position;
            if (dir.magnitude < pushBackDistance)
            {
                dir = Vector3.Normalize(dir.normalized + Vector3.up * 0.75f);
                ec.PushEnemy(dir, pushBackForce);
            }
        }
    }


    void StartGhost()
    {
        if (ghostMovement) return;
        ghost.Show(transform);

        //ghost.transform.position = transform.position;

        Ray mouseRay = Camera.main.ScreenPointToRay(Input.mousePosition);
        float yDist = mouseRay.origin.y - ghost.transform.position.y;
        Vector3 rayPoint = mouseRay.origin - mouseRay.direction * (yDist / mouseRay.direction.y);
        Quaternion toRot = Quaternion.LookRotation(rayPoint - ghost.transform.position);
        ghost.transform.rotation = toRot;

        ghostMovement = true;
        ghost.StartMarking();
    }
    void StopGhost()
    {
        if (!ghostMovement) return;
        ghost.EndMarking();
        ghost.Hide();
        ghostMovement = false;
    }

    void MoveGhost()
    {
        
        Ray mouseRay = Camera.main.ScreenPointToRay(Input.mousePosition);
        float yDist = mouseRay.origin.y - ghost.transform.position.y;
        Vector3 rayPoint = mouseRay.origin - mouseRay.direction * (yDist / mouseRay.direction.y);
        Quaternion toRot = Quaternion.LookRotation(rayPoint - ghost.transform.position);
        ghost.transform.rotation = Quaternion.Lerp(ghost.transform.rotation, toRot, Time.deltaTime * 5f);
        

        Vector3 moveBy = Vector3.forward * ghostSpeed * Time.deltaTime;
        ghost.transform.Translate(moveBy, Space.Self);

        Vector3 heading = ghost.transform.position - transform.position;
        heading.y = 0;
        heading = Vector3.Normalize(heading);
        transform.forward = heading;

        leftGhostDistance -= moveBy.magnitude;
        if (leftGhostDistance <= 0.0f)
        {
            leftGhostDistance = 0.0f;
            StopGhost();
        }
    }

    void Move()
    {
        Vector3 direction = new Vector3(Input.GetAxis("HorizontalKey"), 0, Input.GetAxis("VerticalKey"));
        Vector3 rightMovement = right * (speed - slow) * Time.deltaTime * Input.GetAxis("HorizontalKey");
        Vector3 forwardMovement = forward * (speed - slow) * Time.deltaTime * Input.GetAxis("VerticalKey");

        Vector3 heading = Vector3.Normalize(rightMovement + forwardMovement);

        transform.forward = heading;
        transform.position += rightMovement;
        transform.position += forwardMovement;
    }

    public void TakeDamage(float damage)
    {
        lastDmgTime = Time.time;
        health -= damage;
        healthBar.SetHealth(health);
        Debug.Log("Health remaining: " + health);
        if (health <= 0)
        {
            KillPlayer();
        }
    }

    public void TakeDamage(Arrow arrow)
    {
        lastDmgTime = Time.time;
        health -= arrow.arrowDamage;
        healthBar.SetHealth(health);
        Debug.Log("Health remaining: " + health);
        if (health <= 0)
        {
            KillPlayer();
        }
    }

    public void KillPlayer()
    {
        SceneManager.LoadScene(SceneManager.GetActiveScene().buildIndex);
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
        if (other.CompareTag("Gate"))
        {
            CellGate gate = other.GetComponent<CellGate>();
            if (gate != null)
            {
                GameManager.Instance.SetCurrentCell(gate.Cell);
            }
        }
        if (other.CompareTag("Mud"))
        {
            slow = 3.0f;
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
