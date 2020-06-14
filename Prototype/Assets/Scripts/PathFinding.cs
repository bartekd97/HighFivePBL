using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class PathFinding
{
    private const int MOVE_STRAIGHT_COST = 10;
    private const int MOVE_DIAGONAL_COST = 14;


    private MapCell.Grid grid;
    private List<MapCell.PathNode> openList;
    private List<MapCell.PathNode> closedList;

    public PathFinding(MapCell cell)
    {
        this.grid = cell.grid;
    }

    public List<MapCell.PathNode> FindPath(float startX, float startY, float endX, float endY)
    {
        MapCell.PathNode startNode = grid.GetNode(startX, startY);
        MapCell.PathNode endNode = grid.GetNode(endX, endY);

        if (startNode.isAvailable == false || endNode.isAvailable == false)
        {
            return null;
        }

        openList = new List<MapCell.PathNode> { startNode };
        closedList = new List<MapCell.PathNode>();


        for (int i = 0; i < 50; i++)
        {
            for (int j = 0; j < 50; j++)
            {
                MapCell.PathNode pathnode = grid.gridArray[i, j];
                pathnode.gCost = int.MaxValue;
                pathnode.CalculateFCost();
                pathnode.cameFromNode = null;
            }
        }

        startNode.gCost = 0;
        startNode.hCost = CalculateDistanceCost(startNode, endNode);
        startNode.CalculateFCost();

        while (openList.Count > 0)
        {
            MapCell.PathNode currentNode = GetLowestFCostNode(openList);
            if (currentNode == endNode)
            {
                return CalculatePath(endNode);
            }
            openList.Remove(currentNode);
            closedList.Add(currentNode);

            foreach (MapCell.PathNode neighbour in GetNeighboursList(currentNode))
            {
                if(closedList.Contains(neighbour))
                {
                    continue;
                }
                if (!neighbour.isAvailable)
                {
                    closedList.Add(neighbour);
                    continue;
                }
                int tentativeGCost = currentNode.gCost + CalculateDistanceCost(currentNode, neighbour);
                if(tentativeGCost < neighbour.gCost)
                {
                    neighbour.cameFromNode = currentNode;
                    neighbour.gCost = tentativeGCost;
                    neighbour.hCost = CalculateDistanceCost(neighbour, endNode);
                    neighbour.CalculateFCost();

                    if(!openList.Contains(neighbour))
                    {
                        openList.Add(neighbour);
                    }
                }
            }
        }

        return null;
    }

    private List<MapCell.PathNode> GetNeighboursList(MapCell.PathNode currentNode)
    {
        List<MapCell.PathNode> neighboursList = new List<MapCell.PathNode>();
        if(currentNode.indX - 1 >= 0)
        {
            neighboursList.Add(grid.gridArray[currentNode.indX - 1, currentNode.indY]);
            if(currentNode.indY - 1 >= 0)
            {
                neighboursList.Add(grid.gridArray[currentNode.indX - 1, currentNode.indY - 1]);

            }
            if (currentNode.indY + 1 < 50)
            {
                neighboursList.Add(grid.gridArray[currentNode.indX - 1, currentNode.indY + 1]);

            }
        }
        if (currentNode.indY - 1 >= 0)
        {
            neighboursList.Add(grid.gridArray[currentNode.indX, currentNode.indY - 1]);
        }
        if (currentNode.indX + 1 < 50)
        {
            neighboursList.Add(grid.gridArray[currentNode.indX + 1, currentNode.indY]);
            if (currentNode.indY - 1 >= 0)
            {
                neighboursList.Add(grid.gridArray[currentNode.indX + 1, currentNode.indY - 1]);

            }
            if (currentNode.indY + 1 < 50)
            {
                neighboursList.Add(grid.gridArray[currentNode.indX + 1, currentNode.indY + 1]);

            }
        }
        if (currentNode.indY + 1 < 50)
        {
            neighboursList.Add(grid.gridArray[currentNode.indX, currentNode.indY + 1]);
        }
        return neighboursList;
    }

    private List<MapCell.PathNode> CalculatePath(MapCell.PathNode endNode)
    {
        List<MapCell.PathNode> path = new List<MapCell.PathNode>();
        path.Add(endNode);
        MapCell.PathNode currentNode = endNode;
        while(currentNode.cameFromNode!= null)
        {
            path.Add(currentNode.cameFromNode);
            currentNode = currentNode.cameFromNode;
        }
        path.Reverse();
        return path;
    }

    private int CalculateDistanceCost(MapCell.PathNode a, MapCell.PathNode b)
    {
        int xDistance = Mathf.Abs(a.indX - b.indX);
        int yDistance = Mathf.Abs(a.indY - b.indY);
        int remaining = Mathf.Abs(xDistance - yDistance);
        return MOVE_DIAGONAL_COST * Mathf.Min(xDistance, yDistance) + MOVE_STRAIGHT_COST * remaining;
    }

    private MapCell.
        PathNode GetLowestFCostNode(List<MapCell.PathNode> pathNodeList)
    {
        MapCell.PathNode lowestFCostNode = pathNodeList[0];
        for (int i = 0; i < pathNodeList.Count; i++)
        {
            if(pathNodeList[i].fCost < lowestFCostNode.fCost)
            {
                lowestFCostNode = pathNodeList[i];
            }
        }
        return lowestFCostNode;
    }
}
