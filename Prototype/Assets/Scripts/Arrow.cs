using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Arrow : MonoBehaviour
{
    private Rigidbody arrow;
    //public float arrowSpeed;
    public float arrowDamage;
    //public CharController player;
    public float lifetime = 3.0f;

    // Start is called before the first frame update
    void Start()
    {
        arrow = GetComponent<Rigidbody>();
        Invoke("DestroyArrow", lifetime);
        //player = FindObjectOfType<CharController>();
    }

    // Update is called once per frame
    void Update()
    {
        
    }

    void DestroyArrow()
    {
        Destroy(gameObject);
    }

    private void OnCollisionEnter (Collision collision)
    {
        if (collision.gameObject.tag == "Player")
        {
            collision.gameObject.SendMessage("TakeDamage", this);
            DestroyArrow();
        }
    }
}
