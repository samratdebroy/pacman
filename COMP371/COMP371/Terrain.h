#pragma once
class Terrain
{
public:
	Terrain(int width, int height);
	~Terrain();
	float* getVertices(int width, int height);
	int* getIndices(int width, int height);
	void Draw();
	int getWidth() const;
	int getHeight() const;

private:
	int width;
	int height;
	float* vertices = nullptr;
	int* indices = nullptr;
	int getVerticesCount(int width, int height);
	int getIndicesCount(int width, int height);

	/* Render Data */
	unsigned int VAO, VBO, EBO;
	void setupMesh();
};

