using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class PerkTestPanel : MonoBehaviour
{

    public Ghost ghost;
    public CharController player;


    private void Update()
    {
        

        
    }

    public void EnableDisablePanel()
    {
        if (this.gameObject.activeSelf == true)
            this.gameObject.SetActive(false);
        else
            this.gameObject.SetActive(true);
    }

    public void SetGhostSpawnDistance(float distance)
    {
        ghost.miniGhostSpawnDistance = distance;
    }

    public void SetGhostSpeed(float speed)
    {
        player.ghostSpeed = speed;
    }

    public void SetMaxGhostDistance(float distance)
    {
        player.maxGhostDistance = distance;
    }

    public void SetGhostDistanceRecoverySpeed(float speed)
    {
        player.ghostDistanceRecoverySpeed = speed;
    }

    public void SetPlayerSpeed(float speed)
    {
        player.speed = speed;
    }

    public void SetLineSlow(float slow)
    {
        ghost.lineSlow = slow;
    }

    public void SetGhostFreezeTime(float time)
    {
        ghost.ghostFreezeTime = time;
    }
    public void SetPushBackDistance(float distance)
    {
        player.pushBackDistance = distance;
    }

    public void SetPushBackForce(float force)
    {
        player.pushBackForce = force;
    }

    public void SetPushBackCooldown(float time)
    {
        player.pushCooldownTime = time;
    }

    public void SetGhostCooldown(float time)
    {
        player.ghostCooldownTime = time;
    }

    public void SetNumberOfEnemiesToHit(float number)
    {
        ghost.numberOfEnemyToHit = (int)number;
    }

    public void SetNumberOfActiveLines(float number)
    {
        ghost.maxActiveLines = (int)number;
    }

    public void SetDotTick(float tick)
    {
        ghost.dotTick = tick;
    }

    public void SetDotDmg(float dmg)
    {
        ghost.dotDmg = dmg;
    }
}
