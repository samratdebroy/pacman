#include "GridPoint.h"
#include <iostream>

GridPoint::GridPoint(int x, int z, GridManager* newGrid)
{
	gridPosition = glm::vec3(x, 0.0f, z);
	grid = newGrid;
}

bool GridPoint::hasEnemy()
{
	return (enemy != nullptr);
}

bool GridPoint::hasPacman()
{
	return (pacman != nullptr);
}

bool GridPoint::hasWall()
{
	return (wall != nullptr);
}

bool GridPoint::hasConsumable()
{
	return (consumable != nullptr);
}

/**
 * returns true if it has any entity on the grid point
 */
bool GridPoint::hasEntity()
{
	return (hasPacman() || hasConsumable() || hasEnemy() || hasWall());
}

void GridPoint::attach(GridEntity* entity)
{
	if(hasWall())
	{
		// If the current gridPoint has a wall already, don't attach anything
	}else
	{
		// Add the entity to the current grid point
		switch (entity->type)
		{
		case CONSUMABLE: consumable = entity; break;
		case PACMAN: pacman = entity; break;
		case ENEMY: enemy = entity; break;
		case WALL: wall = entity; break;
		}

		// If we have conflicting entities on the same grid point, resolve the issue
		// If both enemy and pacman present, gameover
		if (hasPacman() && hasEnemy())
		{
			std::cout << "ouch, you lose!" << std::endl;// TODO: add game restart;
			grid->gameOver = true;
		}

		// If both consumable and pacman are on the same point, destroy the consumable
		if (hasPacman() && hasConsumable())
		{
			grid->deleteConsumable(*consumable);
			detach(consumable);
		}

		// Update the entity's current grid point and detach the last one (if contained)
		if(entity->getGridPoint() != nullptr)
			entity->getGridPoint()->detach(entity); // Detach from last gridPoint
		entity->setGridPoint(*this); // Attach to this point
	}
	
}

void GridPoint::detach(GridEntity* entity)
{
	// Remove the entity from the current grid point
	switch (entity->type)
	{
	case CONSUMABLE: consumable = nullptr; break;
	case PACMAN: pacman = nullptr; break;
	case ENEMY:enemy = nullptr; break;
	case WALL: wall = nullptr; break;
	}
}

glm::vec3 GridPoint::getPosition()
{
	return gridPosition;
}

