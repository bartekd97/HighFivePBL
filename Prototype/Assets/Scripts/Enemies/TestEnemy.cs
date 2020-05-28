using System.Collections;
using System.Collections.Generic;
using System.Diagnostics;
using UnityEngine;

public class TestEnemy : EnemyController
{
    // Start is called before the first frame update
    void Start()
    {
        charController = GameManager.Instance.player;
        player = charController.gameObject;

        rb = GetComponent<Rigidbody>();
        pathfinding = new PathFinding(cell);
        
    }

    // Update is called once per frame
    void Update()
    {
        List<MapCell.PathNode> path = pathfinding.FindPath(transform.position.x, transform.position.z, player.transform.position.x, player.transform.position.z);
        UnityEngine.Debug.Log(pathfinding.FindPath(transform.position.x, transform.position.z, player.transform.position.x, player.transform.position.z).Count);
        for (int i = 0; i < path.Count; i++)
        {
            Destroy(Instantiate(pointPrefab, new Vector3(path[i].x, 0, path[i].y), Quaternion.identity), 0.02f);
        }
    }
}
