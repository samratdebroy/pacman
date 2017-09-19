#include "Terrain.h"
#include <glew.h>

// TODO Reformat so that data is stored in Mesh.Vertex instead of arrays
// TODO add normals and UVs so that we can just use Mesh.h to render

Terrain::Terrain(int width, int height)
{
	this->width = width;
	this->height = height;
	getVertices(width, height);
	getIndices(width, height);
	setupMesh();
}


Terrain::~Terrain()
{
}

float* Terrain::getVertices(int width, int height)
{
	if (vertices) return vertices;

	vertices = new float[getVerticesCount(width, height)];
	int i = 0;

	// Populate Vertex positions
	for (int row = 0; row < height; row++)
	{
		for (int col = 0; col <width; col++)
		{
			vertices[i++] = (float)col;	// x pos
			vertices[i++] = 0.0f;			// y pos
			vertices[i++] = (float)row;	// z pos
		}
	}

	return vertices;
}

int* Terrain::getIndices(int width, int height)
{
	if (indices) return indices;

	indices = new int[getIndicesCount(width, height)];
	int numTriStrips = height - 1; // number of triangle strips required
	int offset = 0;

	for (int y = 0; y < numTriStrips; y++)
	{
		// Repeat the first vertex to complete the degenerate triangles from the last Tri strip
		if (y > 0)
			indices[offset++] = y * width; // first vertex of new strip

										   // Add the indices of the vertices on the triangle strip
		for (int x = 0; x < width; x++)
		{
			indices[offset++] = y * width + x; // Top row of the triangle strip
			indices[offset++] = (y + 1) * width + x; // bottom row of the triangle strip
		}

		// Repeat the last vertec for the degenerate triangle to the next triangle strip
		if (y < height - 2)
			indices[offset++] = (y + 2) * width - 1; // last vertex of curr strip
	}
	return indices;
}

void Terrain::Draw()
{
	// draw mesh
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLE_STRIP, getIndicesCount(width, height), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

int Terrain::getWidth() const
{
	return width;
}

int Terrain::getHeight() const
{
	return height;
}

int Terrain::getVerticesCount(int width, int height)
{
	return width * height * 3; //TODO maybe add normals in the future
}

int Terrain::getIndicesCount(int width, int height)
{
	int numTriStrips = height - 1; // number of triangle strips required
	int numDegenIndices = 2 * (numTriStrips - 1); // Number of degenerate indices required (used to move from one strip to another)
	int verticesPerStrip = 2 * width;
	return (verticesPerStrip * numTriStrips + numDegenIndices);
}

void Terrain::setupMesh()
{
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, getVerticesCount(width, height) * sizeof(float), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, getIndicesCount(width, height) * sizeof(int), indices, GL_STATIC_DRAW);

	// vertex positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void*)0);

	glBindVertexArray(0);
}
