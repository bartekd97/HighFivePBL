using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class PushButtonUpdater : MonoBehaviour
{

    public CharController player;
    Text text;

    private void Start()
    {
        text = gameObject.GetComponentInChildren<Text>();
    }

    void Update()
    {
        if (player.nextPushBackTime < Time.time)
        {
            text.enabled = true;
        }
        else
        {
            text.enabled = false;
        }

        float T = (Time.time - (player.nextPushBackTime - player.pushCooldownTime ))/ player.pushCooldownTime;
        if (T <= 1f)
        {
            GetComponent<Image>().color = new Color(T,T,T,T);
        }
    }
}
