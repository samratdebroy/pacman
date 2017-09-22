#include "GridManager.h"
#include <glfw3.h>

/**
 * Place entity on an empty grid point
 */
void GridManager::placeEntity(GridEntity* entity, int xCoord = -1, int zCoord = -1)
{
	if(xCoord < 0 || zCoord < 0)
	{
		xCoord = rand() % width; // get a random x-coordinate
		zCoord = rand() % height; // get a random z-coordinate
	}

	//TODO if 1 + numOfWalls + numOfEnemies + numOfConsumables > width*height then there's not enough space for all objects

	 if (gridPoints[xCoord][zCoord].hasEntity())
	{
		// If there's already an entity here, look for another empty gridpoint
		placeEntity(entity); // Recursive call
	}
	else
	{
		// If the gridPoint is empty, then add this entity to it
		gridPoints[xCoord][zCoord].attach(entity);
	}
	
}

/**
 * Place every entity in the list somewhere on the grid to start
 */
void GridManager::placeEntity(std::vector<GridEntity>* entity)
{
	int size = entity->size();
	for (int i = 0; i < entity->size(); i++)
		placeEntity(&(*entity)[i]);
}


GridManager::GridManager(Terrain* terrain, GridEntity* Pacman, std::vector<GridEntity>* Consumables, std::vector<GridEntity>* Enemies, std::vector<GridEntity>* Walls)
{
	/* initialize random seed: */
	srand(time(NULL));

	width = terrain->getWidth();
	height = terrain->getHeight();

	// Setup GridPoints
	generateGrid();

	this->pacman = Pacman;
	this->consumables = Consumables;
	this->enemies = Enemies;
	this->walls = Walls;

	// Place all the entities in their starting positions if they're not null
	if(!walls->empty())
	placeWalls(); // Always place walls first
	if(pacman != nullptr)
	placeEntity(pacman, width/2, height/2); // Initially pacman is supposed to start at the origin (unless there's a wall)
	if(!enemies->empty())
	placeEntity(enemies);
	if(!consumables->empty())
	placeEntity(consumables);
}

void GridManager::placeWalls()
{
	// TODO: place walls
}

void GridManager::generateGrid()
{
	for(int i = 0 ; i < width; i++)
	{
		std::vector<GridPoint> tempRow;

		// Add new gridpoints to the row based on x-z coordinates
		for(int j = 0; j < height; j++)
		{
			tempRow.push_back(GridPoint(i, j));
		}
		
		// Add the row to the Grid
		gridPoints.push_back(tempRow);
	}
}

/**
 * Place pacman in a random empty position on the grid
 */
void GridManager::resetPacmanPosition()
{
	placeEntity(pacman);
}

void GridManager::move(GridEntity* entity, Movement_Direction direction)
{
	int xCoord = entity->getGridPoint()->getPosition().x;
	int zCoord = entity->getGridPoint()->getPosition().z;

	// Ensure objects move only every set amount of time
	float currentTime = glfwGetTime();
	float deltaTime = currentTime - lastTime;

	if(deltaTime > timeBetweenMovements)
	{
		switch (direction)
		{

		case DOWN:
			if (zCoord < height - 1)
			{
				gridPoints[xCoord][zCoord + 1].attach(entity);
			}
			break;
		case UP:
			if (zCoord > 0)
			{
				gridPoints[xCoord][zCoord - 1].attach(entity);
			}
			break;
		case LEFT:
			if (xCoord > 0)
			{
				gridPoints[xCoord - 1][zCoord].attach(entity);
			}
			break;
		case RIGHT:
			if (xCoord < width - 1)
			{
				gridPoints[xCoord + 1][zCoord].attach(entity);
			}
			break;
		default: break;

		}
		lastTime = currentTime;
		entity->lastDirection = direction; // Update the entities orientation
	}

}

std::vector<GridEntity>* GridManager::getConsumables() const
{
	return consumables;
}

std::vector<GridEntity>* GridManager::getEnemies() const
{
	return enemies;
}

std::vector<GridEntity>* GridManager::getWalls() const
{
	return walls;
}

GridEntity* GridManager::getPacman() const
{
	return pacman;
}

GridPoint GridManager::getGridPoint(int xCoord, int zCoord)
{
	return gridPoints[xCoord][zCoord];
}
