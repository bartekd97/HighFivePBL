using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class PerkTestPanel : MonoBehaviour
{
    public float lineSlow = 0.0f;
    public float ghostFreezeTime = 0.0f;

    public Ghost ghost;
    public CharController player;

    private void Update()
    {
        List<GameObject> enemies = new List<GameObject>(GameObject.FindGameObjectsWithTag("Enemy"));
        enemies.ForEach(enemy => enemy.GetComponent<EnemyController>().slow = 0.0f); ;
        
        //Slow
        ghost.activeLines.ForEach(line =>
        {
            line.ghosts.ForEach(ghost =>
            {
                BoxCollider coll = ghost.GetComponentInParent<BoxCollider>();
                enemies.ForEach(enemy =>
                {
                    BoxCollider enemyColl = enemy.GetComponent<BoxCollider>();
                    if (coll.bounds.Intersects(enemyColl.bounds))
                    {
                        enemy.GetComponent<EnemyController>().slow = lineSlow;
                    }
                });
            });
        });

        //Freeze
        enemies.ForEach(enemy =>
        {
            BoxCollider enemyColl = enemy.GetComponent<BoxCollider>();
            if (enemyColl.bounds.Intersects(ghost.GetComponentInParent<BoxCollider>().bounds))
            {
                enemy.GetComponent<EnemyController>().frozenTo = Time.time + ghostFreezeTime;
            }
        });
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
        lineSlow = slow;
    }

    public void SetGhostFreezeTime(float time)
    {
        ghostFreezeTime = time;
    }
}
