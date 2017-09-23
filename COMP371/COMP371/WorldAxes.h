#pragma once
class WorldAxes
{
private:
	float vertices[12] = {
		0.0f,0.0f,0.0f,
		3.0f,0.0f,0.0f,
		0.0f,3.0f,0.0f,
		0.0f,0.0f,3.0f  }; // Three axis points and origin

	int indices[6] = {0,1,0,2,0,3};
	unsigned int VAO, VBO, EBO;
	void setupMesh();
public:
	WorldAxes();
	void Draw();
};

