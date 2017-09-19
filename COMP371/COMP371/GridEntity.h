#pragma once
#include "glm.hpp"
#include <vector>
#include <string>
#include <glew.h>
#include "objloader.hpp"

// Defines options for entity type on grid
enum Entity_Type {
	CONSUMABLE,
	PACMAN,
	ENNEMY,
	WALL
};

class GridEntity
{
private:

	/* Render Data */
	unsigned int VAO, VBO, EBO;
	GLuint vertices_VBO, normals_VBO;
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec2> UVs;
	void setupMesh();

public:
	Entity_Type type;
	GridEntity(const char * path, Entity_Type setType);
	void Draw(GLenum drawMode);
};

