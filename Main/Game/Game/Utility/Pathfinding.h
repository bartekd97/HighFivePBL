#pragma once

#include <vector>
#include <memory>
#include <glm/glm.hpp>

class PathfindingGrid
{
public:
	struct PathNode {
		// all in local positions
		glm::vec2 position;
		glm::ivec2 index;
		bool isAvailable = false;
	};

private:
	std::vector<std::vector<PathNode>> grid;

public:
	const int width;
	const int height;

private:
	PathfindingGrid(int w, int h) : width(w), height(h) {
		// reserve memory
		grid.resize(width);
		for (auto& col : grid) col.resize(height);
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
};