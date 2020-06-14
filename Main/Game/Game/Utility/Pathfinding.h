#pragma once

#include <vector>
#include <algorithm>
#include <memory>
#include <glm/glm.hpp>
#include <tsl/robin_set.h>
#include "Utility/Logger.h"

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

		inline void CalculateFCost()
		{
			fCost = gCost + hCost;
		}

		inline bool operator==(const PathNode& q)
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

	inline PathNode* FindClosestNode(float xPosition, float yPosition)
	{
		glm::vec2 objectPosition = glm::vec2(xPosition, yPosition);
		PathNode* closestNode = NULL;

		// try search with fast range first
		int nearestXind = 0;
		for (int i=1; i<width; i++)
			if (glm::abs(grid[i][0].position.x - xPosition) > glm::abs(grid[i-1][0].position.x - xPosition))
			{
				nearestXind = i - 1;
				break;
			}
		int nearestYind = 0;
		for (int j = 1; j < height; j++)
			if (glm::abs(grid[0][j].position.y - yPosition) > glm::abs(grid[0][j - 1].position.y - yPosition))
			{
				nearestYind = j - 1;
				break;
			}

		glm::ivec2 xGridRange = {
			glm::clamp(nearestXind - 2 ,0, width),
			glm::clamp(nearestXind + 3 ,0, width)
		};
		glm::ivec2 yGridRange = {
			glm::clamp(nearestYind - 2, 0, height),
			glm::clamp(nearestYind + 3, 0, height)
		};
		
		float currDistance = 999999.0f;
		float newDistance;
		do
		{
			for (int i = xGridRange.x; i < xGridRange.y; i++)
			{
				for (int j = yGridRange.x; j < yGridRange.y; j++)
				{
					newDistance = glm::distance2(objectPosition, glm::vec2(grid[i][j].position));
					if (grid[i][j].isAvailable && currDistance > newDistance)
					{
						closestNode = &grid[i][j];
						currDistance = newDistance;
					}
				}
			}
			
			// could find in fast range? then find with bruteforce
			if (closestNode == NULL)
			{
				xGridRange = { 0, width };
				yGridRange = { 0, height };
			}
		} while (closestNode == NULL);

		return closestNode;
	}

	inline void GetNeighboursList(PathNode& currentNode, std::vector<PathNode*>& list)
	{
		if (currentNode.index.x - 1 >= 0)
		{
			list.push_back(&grid[currentNode.index.x - 1][currentNode.index.y]);
			if (currentNode.index.y - 1 >= 0)
				list.push_back(&grid[currentNode.index.x - 1][currentNode.index.y - 1]);
			if (currentNode.index.y + 1 < height)
				list.push_back(&grid[currentNode.index.x - 1][currentNode.index.y + 1]);
		}
		if (currentNode.index.x + 1 < width)
		{
			list.push_back(&grid[currentNode.index.x + 1][currentNode.index.y]);
			if (currentNode.index.y - 1 >= 0)
				list.push_back(&grid[currentNode.index.x + 1][currentNode.index.y - 1]);
			if (currentNode.index.y + 1 < height)
				list.push_back(&grid[currentNode.index.x + 1][currentNode.index.y + 1]);
		}
		if (currentNode.index.y + 1 < height)
			list.push_back(&grid[currentNode.index.x][currentNode.index.y + 1]);
		if (currentNode.index.y - 1 >= 0)
			list.push_back(&grid[currentNode.index.x][currentNode.index.y - 1]);
	}

	inline std::deque<glm::vec3> CalculatePath(PathNode* endNode, glm::vec3 cellPos)
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

	inline int CalculateDistanceCost(PathNode* a, PathNode* b)
	{
		int xDistance = abs(a->index.x - b->index.x);
		int yDistance = abs(a->index.y - b->index.y);
		int remaining = abs(xDistance - yDistance);
		return MOVE_DIAGONAL_COST * glm::min(xDistance, yDistance) + MOVE_STRAIGHT_COST * remaining;
	}

	inline PathNode* GetLowestFCostNode(tsl::robin_set<PathNode*>& pathNodeList)
	{
		//std::list<PathNode>::iterator it = pathNodeList->begin();

		PathNode* lowestFCostNode = *pathNodeList.begin();
		for (auto& it: pathNodeList)
		{
			if ((it)->fCost < lowestFCostNode->fCost)
			{
				lowestFCostNode = it;
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

	inline std::deque<glm::vec3> FindPath(glm::vec3 startPos, glm::vec3 endPos, glm::vec3 cellPos)
	{
		tsl::robin_set<PathNode*> openList;
		tsl::robin_set<PathNode*> closedList;

		std::deque <glm::vec3> path;


		PathNode* startNode = FindClosestNode(startPos.x - cellPos.x, startPos.z - cellPos.z);
		PathNode* endNode = FindClosestNode(endPos.x - cellPos.x, endPos.z - cellPos.z);;
		startNode->cameFromNode = NULL;


		openList.insert(startNode);

		if (startNode->isAvailable == false || endNode->isAvailable == false)
		{
			return path;
		}

		startNode->gCost = 0;
		startNode->hCost = CalculateDistanceCost(startNode, endNode);
		startNode->CalculateFCost();

		std::vector<PathNode*> neightbours;
		neightbours.reserve(8);
		while (openList.size() > 0)
		{
			PathNode* currentNode = GetLowestFCostNode(openList);
			if (currentNode->index == endNode->index)
			{
				return CalculatePath(currentNode, cellPos);
			}

			openList.erase(currentNode);
			closedList.insert(currentNode);

			GetNeighboursList(*currentNode, neightbours);
			for (auto& neighbour : neightbours)
			{
				if (closedList.contains(neighbour))
				{
					continue;
				}
				if (!neighbour->isAvailable)
				{
					closedList.insert(neighbour);
					continue;
				}
				int tentativeGCost = currentNode->gCost + CalculateDistanceCost(currentNode, neighbour);

				neighbour->cameFromNode = currentNode;
				neighbour->gCost = tentativeGCost;
				neighbour->hCost = CalculateDistanceCost(neighbour, endNode);
				neighbour->CalculateFCost();
				openList.insert(neighbour);
			}
			neightbours.clear();
		}

		return path;
	}

};