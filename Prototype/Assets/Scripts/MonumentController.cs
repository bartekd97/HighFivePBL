using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class MonumentController : MonoBehaviour
{
    //private Rigidbody rb;
    private float lastDmgTime = 0.0f;
    public Color defaultColor;
    public Color damagedColor;
    public float dmgAnimationDuration = 0.5f;

    private float monumentHealth;
    private float maxMonumentHealth = 20.0f;
    public HealthBar healthBar;
    //public GameManager gameManager;


    // Start is called before the first frame update
    void Start()
    {
        monumentHealth = maxMonumentHealth;
        //rb = GetComponent<Rigidbody>();
        healthBar.SetMaxHealth(maxMonumentHealth);
        SetMeshColor(defaultColor);
    }
     
    // Update is called once per frame
    void Update()
    {
        CalculateColor();
    }

    public void ApplyDamageToMonument(float value)
    {
        lastDmgTime = Time.time;
        monumentHealth -= value;
        //Debug.Log("zycko:" + monumentHealth);
        healthBar.SetHealth(monumentHealth);
        //Debug.Log("pasek:" + healthBar.GetHealth());

        if (monumentHealth <= 0)
        {
            Destroy(gameObject);
            //gameManager.Upgrade(); ??
        }
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
}
