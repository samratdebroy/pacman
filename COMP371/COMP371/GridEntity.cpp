#include "GridEntity.h"
#include <matrix_transform.inl>

GridEntity::GridEntity(const char * path, Entity_Type setType)
{
	type = setType;
	loadOBJ(path, vertices, normals, UVs); //read the vertices from the cube.obj file
	setupMesh();
	lastDirection = RIGHT;
}

void GridEntity::Draw(GLenum drawMode)
{
	glBindVertexArray(VAO);
	glDrawArrays(drawMode, 0, vertices.size());
	glBindVertexArray(0);
}

GridPoint* GridEntity::getGridPoint() const
{
	return currGridPoint;
}

void GridEntity::setGridPoint(GridPoint& newPoint)
{
	currGridPoint = &newPoint;
}

glm::mat4 GridEntity::getGridPosition() const
{
	glm::mat4 tempMat; // Identity matrix
	tempMat = glm::translate(tempMat, currGridPoint->getPosition());
	return tempMat;
}

/**
 * Orients the local rotation of the entity assuming it's forward vector points in +x axis by default
 */
void GridEntity::fixOrientation(glm::mat4& forwardFacingModelMat) const
{
	switch(lastDirection)
	{
	case UP: 
		forwardFacingModelMat = glm::rotate(forwardFacingModelMat, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		break;
	case DOWN:
		forwardFacingModelMat = glm::rotate(forwardFacingModelMat, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		break;
	case LEFT: 
		forwardFacingModelMat = glm::rotate(forwardFacingModelMat, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		break;
	case RIGHT: break;
	default: ;
	}
}

void GridEntity::setupMesh()
{
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	// Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &vertices_VBO);

	// Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, vertices_VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices.front(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glGenBuffers(1, &normals_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, normals_VBO);
	glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals.front(), GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0); // Unbind VAO (it's always a good thing to unbind any buffer/array to prevent strange bugs), remember: do NOT unbind the EBO, keep it bound to this VAO

}
