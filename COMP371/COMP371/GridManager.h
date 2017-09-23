#pragma once

#include "GridPoint.h"
#include "Terrain.h"
#include <stdlib.h>   
#include <time.h>      

// Forward declaration
class GridEntity;
class GridPoint;
enum Movement_Direction;

class GridManager
{
private:
	std::vector<std::vector<GridPoint> > gridPoints;
	std::vector<GridEntity>* consumables;
	std::vector<GridEntity>* enemies;
	std::vector<GridEntity>* walls;
	GridEntity* pacman;
	int width, height;
	void placeEntity(GridEntity* entity, int xCoord, int zCoord);
	void placeEntity(std::vector<GridEntity>* entity);
	void generateGrid();

	float timeBetweenMovements = 0.1f; // Time b/w subsequent movements
public:
	GridManager(Terrain * terrain, GridEntity* pacman, std::vector<GridEntity>* consumables, std::vector<GridEntity>* enemies, std::vector<GridEntity>* walls);
	void placeWalls();
	void resetPacmanPosition();
	void move(GridEntity *entity, Movement_Direction direction);

	// GETTERS
	std::vector<GridEntity>* getConsumables() const;
	std::vector<GridEntity>* getEnemies() const;
	std::vector<GridEntity>* getWalls() const;
	GridEntity* getPacman() const;
	GridPoint getGridPoint(int xCoord, int zCoord);
	void deleteConsumable(GridEntity &entity);

	bool gameOver = false;
};

