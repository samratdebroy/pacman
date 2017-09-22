#pragma once
#include "glm.hpp"
#include <vector>
#include <string>
#include <glew.h>
#include "objloader.hpp"
#include "GridPoint.h"

// Defines options for entity type on grid
enum Entity_Type {
	CONSUMABLE,
	PACMAN,
	ENEMY,
	WALL
};

enum Movement_Direction {
	UP,
	DOWN,
	LEFT,
	RIGHT
};

// Forward declaration
class GridPoint;
class GridManager;

class GridEntity
{

private:

	// Override the == operator
	friend inline bool operator==(const GridEntity& lhs, const GridEntity& rhs)
	{
		return (lhs.VAO == rhs.VAO && lhs.VBO == rhs.VBO && lhs.EBO == rhs.EBO && lhs.currGridPoint == rhs.currGridPoint);
	}

	//Grid data
	GridPoint* currGridPoint = nullptr;

	/* Render Data */
	unsigned int VAO, VBO, EBO;
	GLuint vertices_VBO, normals_VBO;
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec2> UVs;
	void setupMesh();

public:
	Entity_Type type;
	bool wasDeleted = false;
	GridEntity(const char * path, Entity_Type setType);
	void Draw(GLenum drawMode);
	GridPoint* getGridPoint() const;
	void setGridPoint(GridPoint &newPoint);
	glm::mat4 getGridPosition() const;
	void fixOrientation(glm::mat4 &forwardFacingModelMat) const;
	Movement_Direction lastDirection;
};