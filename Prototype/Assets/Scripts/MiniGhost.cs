using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class MiniGhost : MonoBehaviour
{
    public GameObject graphicsObject;
    public float attackScaleMultiplier = 1.5f;
    public float attackPreparationTime = 0.3f;
    public float attackSpeed = 8.0f;
    public Color attackColor;
    public float fadeOutTime = 0.3f;
    public float maxAttackTime = 1.5f;

    public float damageToEnemies = 2.0f;

    public bool doingAttack { get; private set; }
    float attackDisappearTime = 0;
    bool disappearingAttack = false;

    private void Awake()
    {
        doingAttack = false;
    }

    private void Update()
    {
        if (doingAttack || disappearingAttack)
            transform.Translate(Vector3.forward * Time.deltaTime * attackSpeed);

        if (!disappearingAttack && doingAttack && Time.time >= attackDisappearTime)
        {
            DisappearDoingAttack();
        }
    }

    void DisappearDoingAttack()
    {
        doingAttack = false;
        disappearingAttack = true;
        FadeOut();
    }

    public void FadeOut()
    {
        StartCoroutine(coFadeOut());
    }
    IEnumerator coFadeOut()
    {
        float dt = 0.0f;
        Vector3 scale = graphicsObject.transform.localScale;
        while (dt < fadeOutTime)
        {
            yield return new WaitForEndOfFrame();
            dt += Time.deltaTime;
            graphicsObject.transform.localScale = Vector3.Lerp(scale, Vector3.zero, dt / fadeOutTime);
        }
        Destroy(gameObject);
    }

    public void DoAttack(Vector3 target)
    {
        Vector3 noise = Random.onUnitSphere;
        noise.y = 0;
        noise *= Random.Range(1.0f,3.0f);
        transform.forward = ((target + noise) - transform.position).normalized;
        StartCoroutine(coDoAttackPrepare());
    }
    IEnumerator coDoAttackPrepare()
    {
        yield return new WaitForSeconds(Random.Range(0.0f, 0.55f));

        MeshRenderer[] renderers = GetComponentsInChildren<MeshRenderer>();

        float dt = 0.0f;
        Vector3 scale = graphicsObject.transform.localScale;
        Color color = renderers[0].material.GetColor("_BaseColor");
        while (dt < attackPreparationTime)
        {
            yield return new WaitForEndOfFrame();
            dt += Time.deltaTime;
            graphicsObject.transform.localScale = Vector3.Lerp(scale, scale * attackScaleMultiplier, dt / attackPreparationTime);
            foreach (var r in renderers)
                r.material.SetColor("_BaseColor", Color.Lerp(color, attackColor, dt / attackPreparationTime));
        }
        doingAttack = true;
        attackDisappearTime = Time.time + maxAttackTime;
    }

    private void OnTriggerEnter(Collider other)
    {
        if (!doingAttack)
            return;

        if (!other.gameObject.CompareTag("Enemy"))
            return;

        EnemyController ec = other.GetComponent<EnemyController>();
        if (ec != null)
        {
            ec.TakeDamage(damageToEnemies);
            //DisappearDoingAttack();
            Destroy(gameObject);
        }
    }
}
