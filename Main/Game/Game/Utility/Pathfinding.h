#pragma once

#include <vector>
#include <algorithm>
#include <list>
#include <memory>
#include <glm/glm.hpp>
#include <Utility/Logger.h>

class PathfindingGrid
{
public:
	struct PathNode {
		// all in local positions
		glm::vec2 position;
		glm::ivec2 index;
		int gCost;
		int hCost;
		int fCost;
		bool isAvailable = false;
		PathNode* cameFromNode;

		void CalculateFCost()
		{
			fCost = gCost + hCost;
		}

		bool operator==(const PathNode& q)
		{
			return (index == q.index);
		}
	};

private:
	std::vector<std::vector<PathNode>> grid;
	const int MOVE_STRAIGHT_COST = 10;
	const int MOVE_DIAGONAL_COST = 14;

public:
	const int width;
	const int height;

private:
	PathfindingGrid(int w, int h) : width(w), height(h) {
		// reserve memory
		grid.resize(width);
		for (auto& col : grid) col.resize(height);
	}

	glm::ivec2 FindClosestNode(float xPosition, float yPosition)
	{
		glm::vec2 objectPosition = glm::vec2(xPosition, yPosition);
		glm::vec2 closestNodePos = glm::vec2(grid[0][0].position);
		glm::ivec2 closestNodeIndex = glm::ivec2(grid[0][0].index);

		float currDistance = glm::distance2(objectPosition, closestNodePos);
		float newDistance;
		for (int i = 0; i < width; i++)
		{
			for (int j = 0; j < height; j++)
			{
				newDistance = glm::distance2(objectPosition, glm::vec2(grid[i][j].position));
				if (grid[i][j].isAvailable && currDistance > newDistance)
				{
					closestNodePos = grid[i][j].position;
					closestNodeIndex = grid[i][j].index;
					currDistance = newDistance;
				}
			}
		}
		return closestNodeIndex;
	}

	std::list<PathNode*> GetNeighboursList(PathNode& currentNode)
	{
		std::list<PathNode*> neighboursList;
		if (currentNode.index.x - 1 >= 0)
		{
			neighboursList.push_back(&grid[currentNode.index.x - 1][currentNode.index.y]);
			if (currentNode.index.y - 1 >= 0)
			{
				neighboursList.push_back(&grid[currentNode.index.x - 1][currentNode.index.y - 1]);

			}
			if (currentNode.index.y + 1 < height)
			{
				neighboursList.push_back(&grid[currentNode.index.x - 1][currentNode.index.y + 1]);

			}
		}
		if (currentNode.index.x + 1 < width)
		{
			neighboursList.push_back(&grid[currentNode.index.x + 1][currentNode.index.y]);
			if (currentNode.index.y - 1 >= 0)
			{
				neighboursList.push_back(&grid[currentNode.index.x + 1][currentNode.index.y - 1]);

			}
			if (currentNode.index.y + 1 < height)
			{
				neighboursList.push_back(&grid[currentNode.index.x + 1][currentNode.index.y + 1]);
			}
		}
		if (currentNode.index.y + 1 < height)
		{
			neighboursList.push_back(&grid[currentNode.index.x][currentNode.index.y + 1]);
		}
		if (currentNode.index.y - 1 >= 0)
		{
			neighboursList.push_back(&grid[currentNode.index.x][currentNode.index.y - 1]);
		}
		return neighboursList;
	}

	std::deque<glm::vec3> CalculatePath(PathNode* endNode, glm::vec3 cellPos)
	{
		std::deque<glm::vec3> path;
		path.push_back(glm::vec3(endNode->position.x + cellPos.x, 0.0f, endNode->position.y + cellPos.z));
		PathNode* currentNode = endNode;

		while (currentNode->cameFromNode != NULL)
		{
			currentNode = currentNode->cameFromNode;

			path.push_back(glm::vec3(currentNode->position.x + cellPos.x, 0.0f, currentNode->position.y + cellPos.z));
		}

		reverse(path.begin(), path.end());
		return path;
	}

	int CalculateDistanceCost(PathNode& a, PathNode& b)
	{
		int xDistance = abs(a.index.x - b.index.x);
		int yDistance = abs(a.index.y - b.index.y);
		int remaining = abs(xDistance - yDistance);
		return MOVE_DIAGONAL_COST * std::min(xDistance, yDistance) + MOVE_STRAIGHT_COST * remaining;
	}

	PathNode* GetLowestFCostNode(std::list<PathNode*>* pathNodeList)
	{
		//std::list<PathNode>::iterator it = pathNodeList->begin();

		PathNode* lowestFCostNode = pathNodeList->front();
		for (std::list<PathNode*>::iterator it = pathNodeList->begin(); it != pathNodeList->end(); ++it)
		{
			if ((*it)->fCost < lowestFCostNode->fCost)
			{
				lowestFCostNode = *it;
			}
		}
		return lowestFCostNode;
	}

public:
	inline static std::shared_ptr<PathfindingGrid> Create(int width, int height) {
		return std::shared_ptr<PathfindingGrid>(new PathfindingGrid(width, height));
	}

	inline void SetNode(int x, int y, PathNode& node) {
		assert(x >= 0 && x < width && "Out of range");
		assert(y >= 0 && y < height && "Out of range");
		grid[x][y] = node;
	}

	inline const PathNode& GetNode(int x, int y) {
		assert(x >= 0 && x < width && "Out of range");
		assert(y >= 0 && y < height && "Out of range");
		return grid[x][y];
	}

	std::deque<glm::vec3> FindPath(glm::vec3 startPos, glm::vec3 endPos, glm::vec3 cellPos)
	{
		std::list<PathNode*> openList;
		std::list<PathNode*> closedList;

		std::deque <glm::vec3> path;

		glm::ivec2 startNodeInd = FindClosestNode(startPos.x - cellPos.x, startPos.z - cellPos.z);
		glm::ivec2 endNodeInd = FindClosestNode(endPos.x - cellPos.x, endPos.z - cellPos.z);;

		PathNode* startNode = &grid[startNodeInd.x][startNodeInd.y];
		PathNode* endNode = &grid[endNodeInd.x][endNodeInd.y];
		startNode->cameFromNode = NULL;


		openList.push_back(startNode);

		if (startNode->isAvailable == false || endNode->isAvailable == false)
		{
			return path;
		}

		startNode->gCost = 0;
		startNode->hCost = CalculateDistanceCost(*startNode, *endNode);
		startNode->CalculateFCost();

		while (openList.size() > 0)
		{
			PathNode* currentNode = GetLowestFCostNode(&openList);
			if (currentNode->index == endNode->index)
			{
				return CalculatePath(currentNode, cellPos);
			}

			std::list<PathNode*>::iterator toErase;
			for (std::list<PathNode*>::iterator it = openList.begin(); it != openList.end(); it++)
			{
				if ((*it)->index == currentNode->index)
				{
					toErase = it;
					if (openList.size() == 0)
					{
						break;
					}
				}
			}
			openList.erase(toErase);

			closedList.push_back(currentNode);

			for(PathNode* neighbour : GetNeighboursList(*currentNode))
			{
				if (std::find(closedList.begin(), closedList.end(), neighbour) != closedList.end())
				{
					continue;
				}
				if (!neighbour->isAvailable)
				{
					closedList.push_back(neighbour);
					continue;
				}
				int tentativeGCost = currentNode->gCost + CalculateDistanceCost(*currentNode, *neighbour);

				neighbour->cameFromNode = currentNode;
				neighbour->gCost = tentativeGCost;
				neighbour->hCost = CalculateDistanceCost(*neighbour, *endNode);
				neighbour->CalculateFCost();

				if (std::find(openList.begin(), openList.end(), neighbour) != openList.end() == false)
				{
					openList.push_back(neighbour);
				}

			}
		}

		return path;
	}



};