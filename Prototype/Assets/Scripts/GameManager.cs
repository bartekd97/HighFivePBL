using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;


public class GameManager : MonoBehaviour
{
    public static GameManager Instance { get; private set; }

    public MapCell currentCell;
    public GameObject playerObject;
    public Ghost ghost;
    public CharController player;
    enum Upgrades { 
        GhostSpawnDistance, GhostSpeed, MaxGhostDistance, GhostDistanceRecoverySpeed,
        PlayerSpeed, LineSlow, GhostFreezeTime, PushBackDistance, PushBackForce,
        PushBackCooldown, GhostCooldown, NumberOfEnemiesToHit, NumberOfActiveLines,
        DotTick, DotDmg
    };
    List<Upgrades> upgradesList = new List<Upgrades>();
    public GameObject upgradesPanel;

    Upgrades firstUpgrade;
    Upgrades secondUpgrade;
    Upgrades thirdUpgrade;
    public Text firstUpgradeText;
    public Text secondUpgradeText;
    public Text thirdUpgradeText;

    //List<MapCell> cells;

    //bool isCellClear;

    private void Awake()
    {
        Instance = this;
        Upgrade();
    }

    public void SetCurrentCell(MapCell cell)
    {
        currentCell = cell;
        if (!IsCellClear(currentCell))
        {
            CloseGates(currentCell);
        }
    }

    bool IsCellClear(MapCell cell)
    {
        bool isClear = true;
        foreach (GameObject e in cell.Enemies)
        {
            if (!e.Equals(null))
            {
                isClear = false;
                break;
            }
        }
        return isClear;
    }

    void Update()
    {
        if (currentCell != null)
        {
            //SetCurrentCell();

            /*
            if (currentCell.Enemies.Count == 0)
            {
                isCellClear = true;
            }

            foreach (GameObject e in currentCell.Enemies)
            {
                isCellClear = true;
                if (!e.Equals(null))
                {
                    isCellClear = false;
                    break;
                }
            }
            */

            if (IsCellClear(currentCell))
            {
                //ActivateBridges();
                OpenGates(currentCell);
            }

        }

    }

    void OpenGates(MapCell cell)
    {
        foreach (MapCell.BridgeTo b in cell.Bridges)
        {
            b.Gate.Open();
        }
    }

    void CloseGates(MapCell cell)
    {
        foreach (MapCell.BridgeTo b in cell.Bridges)
        {
            b.Gate.Close();
        }
    }

    void prepareUpgrades()
    {
        for (int i = 0; i < 15; i++)
        {
            upgradesList.Add((Upgrades)i);
        }
    }

    void Upgrade()
    {
        prepareUpgrades();
        int r = Random.Range(0, upgradesList.Count);
        upgradesList.Remove((Upgrades)r);
        firstUpgrade = (Upgrades)r;

        r = Random.Range(0, upgradesList.Count);
        upgradesList.Remove((Upgrades)r);
        secondUpgrade = (Upgrades)r;

        r = Random.Range(0, upgradesList.Count);
        upgradesList.Remove((Upgrades)r);
        thirdUpgrade = (Upgrades)r;

        firstUpgradeText.text = SetTextUpgrade(firstUpgrade);
        secondUpgradeText.text = SetTextUpgrade(secondUpgrade);
        thirdUpgradeText.text = SetTextUpgrade(thirdUpgrade);

        upgradesPanel.SetActive(true);
    }

    string SetTextUpgrade(Upgrades upgrade)
    {
        if (upgrade == (Upgrades)0)
        {
            return "Increase MiniGhosts Number";
        }
        else if (upgrade == (Upgrades)1)
        {
            return "Increase GhostBar Recovery";
        }
        else if (upgrade == (Upgrades)2)
        {
            return "Increase Ghost Max Distance";
        }
        else if (upgrade == (Upgrades)3)
        {
            return "Increase Ghost Recovery Speed";
        }
        else if (upgrade == (Upgrades)4)
        {
            return "Increase Player Speed";
        }
        else if (upgrade == (Upgrades)5)
        {
            return "Increase Line Slow";
        }
        else if (upgrade == (Upgrades)6)
        {
            return "Increase Line FreezeTime";
        }
        else if (upgrade == (Upgrades)7)
        {
            return "Increse PushBack Distance";
        }
        else if (upgrade == (Upgrades)8)
        {
            return "Increase PushBack Force";
        }
        else if (upgrade == (Upgrades)9)
        {
            return "Decrese PushBack Cooldown";
        }
        else if (upgrade == (Upgrades)10)
        {
            return "Decrese Ghost Cooldown";
        }
        else if (upgrade == (Upgrades)11)
        {
            return "Increase Number Of Enemies Hit";
        }
        else if (upgrade == (Upgrades)12)
        {
            return "Increase Number Of Active Lines";
        }
        else if (upgrade == (Upgrades)13)
        {
            return "Increase Dot Tick";
        }
        else
        {
            return "Increase Dot Dmg";
        }
    }

    void SetUpgrade(Upgrades upgrade)
    {
        if (upgrade == (Upgrades)0)
        {
            ghost.miniGhostSpawnDistance = 1.5f* ghost.miniGhostSpawnDistance;
        }
        else if (upgrade == (Upgrades)1)
        {
            player.ghostSpeed = 1.5f * player.ghostSpeed;
        }
        else if (upgrade == (Upgrades)2)
        {
            player.maxGhostDistance = 1.5f * player.maxGhostDistance;
        }
        else if (upgrade == (Upgrades)3)
        {
            player.ghostDistanceRecoverySpeed = 1.5f * player.ghostDistanceRecoverySpeed;
        }
        else if (upgrade == (Upgrades)4)
        {
            player.speed = 1.5f * player.speed;
        }
        else if (upgrade == (Upgrades)5)
        {
            ghost.lineSlow += 0.3f;
        }
        else if (upgrade == (Upgrades)6)
        {
            ghost.ghostFreezeTime += 0.3f;
        }
        else if (upgrade == (Upgrades)7)
        {
            player.pushBackDistance = 1.5f * player.pushBackDistance;
        }
        else if (upgrade == (Upgrades)8)
        {
            player.pushBackForce = 1.5f * player.pushBackForce;
        }
        else if (upgrade == (Upgrades)9)
        {
            player.pushCooldownTime = 0.8f * player.pushCooldownTime;
        }
        else if (upgrade == (Upgrades)10)
        {
            player.ghostCooldownTime = 0.8f * player.ghostCooldownTime;
        }
        else if (upgrade == (Upgrades)11)
        {
            ghost.numberOfEnemyToHit += 1;
        }
        else if (upgrade == (Upgrades)12)
        {
            ghost.maxActiveLines += 1;
        }
        else if (upgrade == (Upgrades)13)
        {
            ghost.dotTick += 1f;
        }
        else if (upgrade == (Upgrades)14)
        {
            ghost.dotDmg += 1f;
        }
    }

    public void GetFirstUpgrade()
    {
        SetUpgrade(firstUpgrade);
        upgradesList = null;
        upgradesPanel.SetActive(false);
    }

    public void GetSecondUpgrade()
    {
        SetUpgrade(secondUpgrade);
        upgradesList = null;

        upgradesPanel.SetActive(false);
    }

    public void GetThirdUpgrade()
    {
        SetUpgrade(thirdUpgrade);
        upgradesList = null;

        upgradesPanel.SetActive(false);
    }

    /*
    // Start is called before the first frame update
    public void GameSetup()
    {
        cells = new List<MapCell>(MapCell.All);
    }


    void SetCurrentCell()
    {
        MapCell oldCurrentCell = currentCell;
        foreach (MapCell c in cells)
        {
            float cx = c.transform.position.x - playerObject.transform.position.x;
            float cz = c.transform.position.z - playerObject.transform.position.z; 
            if (c.PolygonSmoothInner.GetEdgeCenterRatio(new Vector2(cx, cz)) < 0.8)
            {
                currentCell = c;
                if (currentCell != oldCurrentCell)
                {
                    DisableBridges();
                }
                break;
            }
        }
    }

    void DisableBridges()
    {
        foreach(MapCell.BridgeTo b in  currentCell.Bridges)
        {
            b.Bridge.gameObject.SetActive(false);
        }
    }

    void ActivateBridges()
    {
        foreach (MapCell.BridgeTo b in currentCell.Bridges)
        {
            b.Bridge.gameObject.SetActive(true);
        }
    }
    */

    //private float GetDistanceFromPointToLine(Vector3 p,  Vector3 c)
    //{
    //    return Mathf.Abs((c.x - p.x)  + (c.z- p.z)  / Mathf.Sqrt(Mathf.Pow(-lineB.y + lineA.y, 2) + Mathf.Pow(lineB.x - lineA.x, 2));
    //}
}
