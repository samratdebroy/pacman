//example skeleton code
//modified from http://learnopengl.com/

#include "stdafx.h"

#include "..\glew\glew.h"	// include GL Extension Wrangler
#include "..\glfw\glfw3.h"	// include GLFW helper library
#include <stdio.h>
#include <iostream>
#include <string>
#include <fstream>
#include "glm.hpp"
#include "gtc/matrix_transform.hpp"
#include "gtc/type_ptr.hpp"
#include "objloader.hpp"  //include the object loader

#include "Camera.h"
#include "Shader.h"
#include "Terrain.h"
#include "WorldAxes.h"
#include "GridEntity.h"
#include "GridManager.h"

using namespace std;

// Window dimensions
const GLuint WIDTH = 800, HEIGHT = 800;

glm::vec3 camera_position;
glm::vec3 triangle_scale;
glm::mat4 projection;

// Camera Settings
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f)); // Camera with starting position
float lastX = WIDTH / 2.0f, lastY = HEIGHT / 2.0f; // start at center of screen
bool firstMouse = true; // flag for first mouse movement

// Mouse Buttons held down
bool leftButtonClicked = false;
bool rightButtonClicked = false;
bool middleButtonClicked = false;

// Timing Variables
float deltaTime = 0.0f; // Time b/w last frame and current frame
float lastFrame = 0.0f;
float lastEnemyMovement = 0.0f;

// Player controlled variables
GLenum drawMode = GL_TRIANGLES;
bool stopEnemyMovement = true;
bool restart = true;

// Grid entities and grid
std::vector<GridEntity> consumables;
std::vector<GridEntity> enemies;
std::vector<GridEntity> walls;
GridEntity* pacman;
GridManager* grid;

// Setup Variables for World Orientation
float worldYaw = 0;
float worldPitch = 0;
glm::mat4 worldOrientation(1.0f);

// Prototype
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void processInput(GLFWwindow *window);
void calcWorldOrientation();
void setupGridEntities(int numOfConsumables = 10, int numOfEnemies = 4);
void setDrawMode(GLenum newDrawMode);
void moveEnemies(std::vector<GridEntity>& enemies);


// The MAIN function, from here we start the application and run the game loop
int main()
{
	std::cout << "Starting GLFW context, OpenGL 3.3" << std::endl;
	// Init GLFW
	glfwInit();
	// Set all the required options for GLFW
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Create a GLFWwindow object that we can use for GLFW's functions
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Load one cube", nullptr, nullptr);
	if (window == nullptr)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// Set the callback functions for mouse movements and frame size change
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);


	// Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
	glewExperimental = GL_TRUE;
	// Initialize GLEW to setup the OpenGL Function pointers
	if (glewInit() != GLEW_OK)
	{
		std::cout << "Failed to initialize GLEW" << std::endl;
		return -1;
	}

	// Define the viewport dimensions
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);

	glViewport(0, 0, width, height);
	glEnable(GL_DEPTH_TEST); // enable the z-buffer and depth testing
	glDepthFunc(GL_LESS); // re-enable the depth buffer to normal


	projection = glm::perspective(45.0f, (GLfloat)width / (GLfloat)height, 0.01f, 100.0f);

	// Ask user for grid Size
	int gridWidth = 999, gridHeight = 999;
	while (gridWidth < 8 || gridWidth > 500)
	{
		cout << "Please enter the width (10 to 500) of the Grid you would like" << endl;
		cin >> gridWidth;
	}

	while (gridHeight < 8 || gridHeight > 500)
	{
		cout << "Please enter the height (10 to 500) of the Grid you would like" << endl;
		cin >> gridHeight;
	}

	cout << "Press 'Q' to let the enemies chase you" << endl;

	Shader shader("shaders/vertex.shader", "shaders/fragment.shader");
	shader.UseProgram();

	triangle_scale = glm::vec3(0.5f); //set to 0.03f for pacman model

									  // Terrain Plain
	Terrain terrain(gridWidth, gridHeight);
	Shader terrainShader("shaders/terrain.vert", "shaders/terrain.frag");

	// Global Axes
	WorldAxes worldAxes;
	Shader axisShader("shaders/axis.vert", "shaders/axis.frag");

	// Do while loop so that everytime a player hits an ennemy, game restarts
	do {

		// Setup up Grid and Entities
		setupGridEntities((gridHeight + gridWidth) / 2, 4);
		GridManager newGrid = GridManager(&terrain, pacman, &consumables, &enemies, &walls);
		grid = &newGrid;

		// Game loop
		while (!glfwWindowShouldClose(window))
		{
			// per-frame Time logic
			float currentFrame = glfwGetTime();
			deltaTime = currentFrame - lastFrame;
			lastFrame = currentFrame;

			// Handle inputs
			processInput(window);

			if (currentFrame - lastEnemyMovement > 0.3 && !stopEnemyMovement)
			{
				moveEnemies(enemies);
				lastEnemyMovement = currentFrame;
			}

			// Render
			// Clear the colorbuffer
			glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			// View matrix
			glm::mat4 view;
			view = camera.ViewMatrix();

			// World Orientation MAtrix
			calcWorldOrientation();

			// PACMAN
			glm::mat4 pacman_model = worldOrientation;
			pacman_model *= pacman->getGridPosition();
			pacman_model = glm::translate(pacman_model, glm::vec3(-terrain.getWidth() / 2.0f, 0.0f, -terrain.getHeight() / 2.0f)); // Follow the terrains translation
			pacman_model = glm::scale(pacman_model, triangle_scale);
			pacman->fixOrientation(pacman_model);
			//pacman_model = glm::rotate(pacman_model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f)); // Don't use with teapot

			shader.UseProgram();
			shader.setMat4("projection_matrix", projection);
			shader.setMat4("view_matrix", view);
			shader.setMat4("model_matrix", pacman_model);
			shader.setVec4("ColorIn", glm::vec4(1.0f, 1.0f, 0.0f, 1.0f));

			pacman->Draw(drawMode);

			// Draw remaining Consumables
			for (int i = 0; i < consumables.size(); i++)
			{
				if (!consumables[i].wasDeleted)
				{
					glm::mat4 consumable_model = worldOrientation;
					consumable_model *= consumables[i].getGridPosition();
					consumable_model = glm::translate(consumable_model, glm::vec3(-terrain.getWidth() / 2.0f, 0.0f, -terrain.getHeight() / 2.0f)); // Follow the terrains translation
					consumable_model = glm::scale(consumable_model, triangle_scale * glm::vec3(0.4f)); // do * vec3(10) if using pacman model

					shader.UseProgram();
					shader.setMat4("projection_matrix", projection);
					shader.setMat4("view_matrix", view);
					shader.setMat4("model_matrix", consumable_model);
					shader.setVec4("ColorIn", glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));

					consumables[i].Draw(drawMode);
				}
			}

			// Draw Enemies
			for (int i = 0; i < enemies.size(); i++)
			{
				glm::mat4 enemy_model = worldOrientation;
				enemy_model *= enemies[i].getGridPosition();
				enemy_model = glm::translate(enemy_model, glm::vec3(-terrain.getWidth() / 2.0f, 0.0f, -terrain.getHeight() / 2.0f)); // Follow the terrains translation
				enemy_model = glm::scale(enemy_model, triangle_scale * glm::vec3(0.2f));
				enemies[i].fixOrientation(enemy_model);

				shader.UseProgram();
				shader.setMat4("projection_matrix", projection);
				shader.setMat4("view_matrix", view);
				shader.setMat4("model_matrix", enemy_model);
				// Just to make things interesting, have the ghosts change color with time
				shader.setVec4("ColorIn", glm::vec4(glm::sin(glfwGetTime() *(i + 1)), 0.8f, glm::sin(glfwGetTime() / (i + 1.0f)), 1.0f));

				enemies[i].Draw(drawMode);

			}

			// Terrain
			terrainShader.UseProgram();
			glm::mat4 model = worldOrientation;
			model = glm::translate(model, glm::vec3(-terrain.getWidth() / 2.0f, -0.75f, -terrain.getHeight() / 2.0f));
			terrainShader.setMat4("projection", projection);
			terrainShader.setMat4("view", view);
			terrainShader.setMat4("model", model);
			terrain.Draw();

			// Axes
			glDepthFunc(GL_ALWAYS); // always render the world axes 
			axisShader.UseProgram();
			model = worldOrientation;
			axisShader.setMat4("projection", projection);
			axisShader.setMat4("view", view);
			axisShader.setMat4("model", model);
			worldAxes.Draw();
			glDepthFunc(GL_LESS); // Set depth buffer function back to normal

			// Swap the screen buffers
			glfwSwapBuffers(window);
			// Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
			glfwPollEvents();

			// If the player touched an enemy, restart the game
			if (grid->gameOver)
				break;

		}

		enemies.clear();
		consumables.clear();

	}while (restart);

	// Terminate GLFW, clearing any resources allocated by GLFW.
	glfwTerminate();
	return 0;
}

// Process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
void processInput(GLFWwindow *window)
{
	// Exits the application if escape was pressed
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		grid->gameOver = true;
		restart = false;
		glfwSetWindowShouldClose(window, GL_TRUE);
	}

	// World Orientation keys
	float worldRotationSpeed = 15.0f * deltaTime;
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
		worldPitch += worldRotationSpeed;
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
		worldPitch -= worldRotationSpeed;
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
		worldYaw += worldRotationSpeed;
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
		worldYaw -= worldRotationSpeed;
	if (glfwGetKey(window, GLFW_KEY_HOME) == GLFW_PRESS)
	{
		worldPitch = 0;
		worldYaw = 0;
	}


	// Scale up and Down
	if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS)
		triangle_scale += 0.01 * deltaTime;
	if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS)
		triangle_scale -= 0.01 * deltaTime;

	// Change Render Mode
	if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
		setDrawMode(GL_LINES);
	if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS)
		setDrawMode(GL_TRIANGLES);
	if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
		setDrawMode(GL_POINTS);

	// Place pacman in a random location
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		grid->resetPacmanPosition();

	// Pacman movement keys
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		grid->move(pacman, UP);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		grid->move(pacman, DOWN);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		grid->move(pacman, RIGHT);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		grid->move(pacman, LEFT);

	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		stopEnemyMovement = !stopEnemyMovement;
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	// right mouse button
	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
		rightButtonClicked = true;
	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE)
		rightButtonClicked = false;

	// left mouse button
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
		leftButtonClicked = true;
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
		leftButtonClicked = false;


	// middle mouse button
	if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_PRESS)
		middleButtonClicked = true;
	if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_RELEASE)
		middleButtonClicked = false;
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	// Calculate offsets
	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates range from bottom to top

	lastX = xpos;
	lastY = ypos;

	float cameraSensitivity = 0.1f;

	if(leftButtonClicked)
		camera.DisplacePosition(camera.forwardDirection() * yoffset * cameraSensitivity);

	// Pan around y-axis if right mouse button is held down
	if (rightButtonClicked)
		camera.DisplacePosition(camera.rightDirection() * xoffset * cameraSensitivity);

	// Tilt around x-axis if middle button is held down
	if (middleButtonClicked)
		camera.ChangePitch(yoffset * cameraSensitivity);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions
	glViewport(0, 0, width, height);
	// Update the project matrix to ensure we keep the proper aspect ratio
	projection = glm::perspective(45.0f, (GLfloat)width / (GLfloat)height, 0.01f, 100.0f); 
}

void setupGridEntities(int numOfConsumables, int numOfEnemies)
{
	pacman = new GridEntity ("models/teapot.obj", PACMAN);

	for (int i = 0; i < numOfConsumables; i++)
	{
		GridEntity tempEntity("models/sphere.obj", CONSUMABLE);
		consumables.push_back(tempEntity);
	}

	for (int i = 0; i < numOfEnemies; i++)
	{
		GridEntity tempEntity("models/MazeGhostWhite.obj", ENEMY);
		enemies.push_back(tempEntity);
	}

	// TODO: Set up walls
}

void setDrawMode(GLenum newDrawMode)
{
	drawMode = newDrawMode;
}

void calcWorldOrientation()
{
	worldOrientation = glm::mat4(1.0f);
	worldOrientation = glm::rotate(worldOrientation, glm::radians(worldPitch), glm::vec3(1, 0, 0));
	worldOrientation = glm::rotate(worldOrientation, glm::radians(worldYaw), glm::vec3(0, 1, 0));
}

void moveEnemies(std::vector<GridEntity>& enemies)
{
	for (int i = 0; i < enemies.size(); i++) {
		glm::vec3 enemyPosition = enemies[i].getGridPoint()->getPosition();
		glm::vec3 pacmanPosition = pacman->getGridPoint()->getPosition();

		// Used to randomize whether next movement will be on x or z axis
		int moveXFirst = rand() % 2;

		if(moveXFirst == 0)
		{
			if (enemyPosition.x > pacmanPosition.x)
				grid->move(&enemies[i], LEFT);
			if (enemyPosition.x < pacmanPosition.x)
				grid->move(&enemies[i], RIGHT);
		}

		if (enemyPosition.z > pacmanPosition.z)
			grid->move(&enemies[i], UP);
		if (enemyPosition.z < pacmanPosition.z)
			grid->move(&enemies[i], DOWN);

		if (moveXFirst != 0)
		{
			if (enemyPosition.x > pacmanPosition.x)
				grid->move(&enemies[i], LEFT);
			if (enemyPosition.x < pacmanPosition.x)
				grid->move(&enemies[i], RIGHT);
		}
	}
}