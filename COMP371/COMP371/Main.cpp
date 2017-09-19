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
float longDeltaTime = 0.0f;

// Player variables
glm::mat4 pacman_model; //Pacman world location
glm::vec3 pacmanGridPosition(0.0f,0.0f,0.0f);

// Prototype
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void processInput(GLFWwindow *window);
void getPlayerPosition(glm::mat4 &modelMat, glm::vec3 gridPosition);
void updatePlayerPosition(glm::vec3 &currGridPosition, glm::vec3 displacementVector);

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

	Shader shader("shaders/vertex.shader", "shaders/fragment.shader");
	shader.UseProgram();

	GridEntity pacman("models/pacman.obj", PACMAN);
	
	triangle_scale = glm::vec3(0.03f);

	// Terrain Plain
	Terrain terrain(21, 21);
	Shader terrainShader("shaders/terrain.vert", "shaders/terrain.frag");

	// Global Axes
	WorldAxes worldAxes;
	Shader axisShader("shaders/axis.vert", "shaders/axis.frag");

	// Game loop
	while (!glfwWindowShouldClose(window))
	{
		// per-frame Time logic
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// Handle inputs
		processInput(window);


		// Render
		// Clear the colorbuffer
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm::mat4 view;
		view = camera.ViewMatrix();

		getPlayerPosition(pacman_model,pacmanGridPosition);
		pacman_model = glm::scale(pacman_model, triangle_scale);
		pacman_model = glm::rotate(pacman_model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));

		shader.UseProgram();
		shader.setMat4("projection_matrix", projection);
		shader.setMat4("view_matrix", view);
		shader.setMat4("model_matrix", pacman_model);

		pacman.Draw(GL_TRIANGLES);

		// Terrain
		terrainShader.UseProgram();
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-terrain.getWidth() / 2.0f, -0.75f, -terrain.getHeight() / 2.0f));
		terrainShader.setMat4("projection", projection);
		terrainShader.setMat4("view", view);
		terrainShader.setMat4("model", model);
		terrain.Draw();

		// Axes
		glDepthFunc(GL_ALWAYS); // always render the world axes 
		axisShader.UseProgram();
		model = glm::mat4(1.0f);
		axisShader.setMat4("projection", projection);
		axisShader.setMat4("view", view);
		axisShader.setMat4("model", model);
		worldAxes.Draw();
		glDepthFunc(GL_LESS); // Set depth buffer function back to normal

		// Swap the screen buffers
		glfwSwapBuffers(window);
		// Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
		glfwPollEvents();
	}

	// Terminate GLFW, clearing any resources allocated by GLFW.
	glfwTerminate();
	return 0;
}

// Process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
void processInput(GLFWwindow *window)
{
	// Exits the application if escape was pressed
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	// Camera movement keys
	float cameraSpeed = 8.5 * deltaTime;
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
		camera.DisplacePosition(camera.forwardDirection() * cameraSpeed);
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
		camera.DisplacePosition(-camera.forwardDirection() * cameraSpeed);
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
		camera.DisplacePosition(camera.rightDirection() * cameraSpeed);
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
		camera.DisplacePosition(-camera.rightDirection() * cameraSpeed);

	// Camera movement keys
	float playerSpeed = 10 * deltaTime;
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		updatePlayerPosition(pacmanGridPosition, glm::vec3(0.0f, 0.0f, -1.0f));
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		updatePlayerPosition(pacmanGridPosition, glm::vec3(0.0f, 0.0f, 1.0f));
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		updatePlayerPosition(pacmanGridPosition, glm::vec3(1.0f, 0.0f, 0.0f));
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		updatePlayerPosition(pacmanGridPosition, glm::vec3(-1.0f, 0.0f, 0.0f));

	if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS && !middleButtonClicked)
		middleButtonClicked = true;
	if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_RELEASE)
		middleButtonClicked = false;

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

	// Pan around y-axis if right mouse button is held down
	if (rightButtonClicked)
		camera.ChangeYaw(xoffset * cameraSensitivity);

	// Tilt around x-axis if middle button is held down
	if (middleButtonClicked)
		camera.ChangePitch(yoffset * cameraSensitivity);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}

void getPlayerPosition(glm::mat4 &modelMat, glm::vec3 gridPosition)
{
	glm::mat4 tempMat; // Identity matrix
	tempMat = glm::translate(tempMat, gridPosition + glm::vec3(0.5f,0.0f,0.5f));
	// TODO: Add local rotation
	modelMat = tempMat;
}

void updatePlayerPosition(glm::vec3 &currGridPosition, glm::vec3 displacementVector)
{
	float timeBetweenMoves = 0.1f; // seconds between each movement
	if(longDeltaTime > timeBetweenMoves)
	{
		currGridPosition += displacementVector;
		longDeltaTime = 0;
	}else
	{
		longDeltaTime += deltaTime;
	}
}