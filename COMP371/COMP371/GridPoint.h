#pragma once

#include "GridEntity.h"
#include <algorithm>
#include "GridManager.h"

// Forward declaration
class GridEntity;
class GridManager;

class GridPoint
{
	GridManager* grid;
	GridEntity* consumable = nullptr;
	GridEntity *pacman = nullptr;
	GridEntity *wall = nullptr;
	std::vector<GridEntity *> enemies;
	glm::vec3 gridPosition;
public:
	GridPoint(int x, int z, GridManager* newGrid);

	// Functions to see what's currently on the grid point
	bool hasEnemy();
	bool hasPacman();
	bool hasWall();
	bool hasConsumable();
	bool hasEntity();

	// Functions to add/remove items on the grid
	void attach(GridEntity * entity);
	void detach(GridEntity * entity);

	glm::vec3 getPosition();
};

