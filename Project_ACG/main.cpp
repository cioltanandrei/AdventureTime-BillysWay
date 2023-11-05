// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

// Include GLEW
#include "dependente\glew\glew.h"

// Include GLFW
#include "dependente\glfw\glfw3.h"

// Include GLM
#include "dependente\glm\glm.hpp"
#include "dependente\glm\gtc\matrix_transform.hpp"
#include "dependente\glm\gtc\type_ptr.hpp"

#include "shader.hpp"

//variables
GLFWwindow* window;
const int width = 2256, height = 1504;
//scaling coord for mount
float sx = 1.0f, sy = 1.0f, sz = 0.0f;
// Player position
glm::vec3 playerPos = glm::vec3(0.0f, 0.0f, 0.0f);
float playerSpeed = 0.001f;
//variables for jump
float playerVelocityY = 0.0f;
const float gravity = 0.0001f;
bool isJumping = false;
const float jumpStrength = 0.01f;
//player size
int playerSizeX = 0.2;
// Sword position
glm::vec3 swordPos = glm::vec3(0.0f, 0.0f, 0.0f);
//variables for swing
float swingAngle = 0.0f;


//scaling for window resizing
void window_callback(GLFWwindow* window, int new_width, int new_height)
{
	//what should we do here?
	glViewport(0, 0, new_width, new_height);
}

//Handling cursor position
void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
	std::cout << "The mouse cursor is: " << xpos << " " << ypos << std::endl;
}


int main(void)
{
	// Initialise GLFW
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		getchar();
		return -1;
	}

	// Open a window and create its OpenGL context
	window = glfwCreateWindow(width, height, "Red RECTANGLE", NULL, NULL);
	if (window == NULL) {
		fprintf(stderr, "Failed to open GLFW window.");
		getchar();
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		getchar();
		glfwTerminate();
		return -1;
	}

	//specify the size of the rendering window
	glViewport(0, 0, width, height);

	// blue background
	glClearColor(0.8f, 0.9f, 1.0f, 0.8f);
	glClear(GL_COLOR_BUFFER_BIT);


	// Create and compile our GLSL program from the shaders
	GLuint programID = LoadShaders("SimpleVertexShaderM.vertexshader", "SimpleFragmentShaderM.fragmentshader");
	GLuint programID2 = LoadShaders("SimpleVertexShaderP.vertexshader", "SimpleFragmentShaderP.fragmentshader");
	GLuint programID3 = LoadShaders("SimpleVertexShaderS.vertexshader", "SimpleFragmentShaderS.fragmentshader");

	float verticesMountains[] = {
		 0.0f,  0.5f, 0.0f,  // top center
		-0.5f, -0.5f, 0.0f,  // bottom left
		0.5f, -0.5f, 0.0f,   // bottom right

		0.25f, -0.5f, 0.0f,
		0.5f, 0.25f, 0.0f,
		1.0f, -0.5f, 0.0f,

		-0.25f, -0.5f, 0.0f,
		-0.5f, 0.25f, 0.0f,
		-1.0f, -0.5f, 0.0f

	};

	unsigned int indicesMountains[] = {
		0, 1, 2, // Left slope
		3, 4 ,5,
		6, 7, 8
	};

	float verticesPlayer[] = {
	-0.8f, -0.4f, 0.0f,    // Top-left vertex
	-0.6f, -0.4f, 0.0f,    // Top-right vertex
	-0.6f, -0.6f, 0.0f,    // Bottom-right vertex
	-0.8f, -0.6f, 0.0f     // Bottom-left vertex
	};


	// Define the indices for the square
	GLuint indicesPlayer[] ={
		0, 1, 2,
		0, 2, 3 
	};

	float verticesSword[] = {
	-0.8f, -0.6f, 0.0f,    // Top-left vertex
	-0.75f, -0.6f, 0.0f,    // Top-right vertex
	-0.75f, -0.9f, 0.0f,    // Bottom-right vertex
	-0.8f, -0.9f, 0.0f     // Bottom-left vertex
	};

	// Define the indices for the rectangle
	GLuint indicesSword[] = {
		0, 1, 2,
		0, 2, 3
	};


	// Mountains buffers
	GLuint vboM, vaoM, iboM;
	glGenVertexArrays(1, &vaoM);
	glGenBuffers(1, &vboM);
	glGenBuffers(1, &iboM);

	//bind VAO
	glBindVertexArray(vaoM);

	//bind VBO
	glBindBuffer(GL_ARRAY_BUFFER, vboM);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verticesMountains), verticesMountains, GL_STATIC_DRAW);


	//bind IBO
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboM);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indicesMountains), indicesMountains, GL_STATIC_DRAW);

	//set attribute pointers
	glVertexAttribPointer(
		0,                  // attribute 0, must match the layout in the shader.
		3,                  // size of each attribute
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		3 * sizeof(float),  // stride
		(void*)0            // array buffer offset
	);
	glEnableVertexAttribArray(0);

	//Player Buffer

	GLuint vboP, vaoP, iboP;
	glGenVertexArrays(1, &vaoP);
	glGenBuffers(1, &vboP);
	glGenBuffers(1, &iboP);

	//bind VAO
	glBindVertexArray(vaoP);

	//bind VBO
	glBindBuffer(GL_ARRAY_BUFFER, vboP);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verticesPlayer), verticesPlayer, GL_STATIC_DRAW);


	//bind IBO
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboP);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indicesPlayer), indicesPlayer, GL_STATIC_DRAW);

	//set attribute pointers
	glVertexAttribPointer(
		0,                  // attribute 0, must match the layout in the shader.
		3,                  // size of each attribute
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		3 * sizeof(float),  // stride
		(void*)0            // array buffer offset
	);
	glEnableVertexAttribArray(0);

	//Sword Buffer

	GLuint vboS, vaoS, iboS;
	glGenVertexArrays(1, &vaoS);
	glGenBuffers(1, &vboS);
	glGenBuffers(1, &iboS);

	//bind VAO
	glBindVertexArray(vaoS);

	//bind VBO
	glBindBuffer(GL_ARRAY_BUFFER, vboS);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verticesSword), verticesSword, GL_STATIC_DRAW);


	//bind IBO
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboS);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indicesSword), indicesSword, GL_STATIC_DRAW);

	//set attribute pointers
	glVertexAttribPointer(
		0,                  // attribute 0, must match the layout in the shader.
		3,                  // size of each attribute
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		3 * sizeof(float),  // stride
		(void*)0            // array buffer offset
	);
	glEnableVertexAttribArray(0);


	
	//create transform for mountains
	glm::mat4 transMount(1.0f);
	transMount = glm::scale(transMount, glm::vec3(sx, sy, sz));
	glShadeModel(GL_SMOOTH);

	//create transform for player
	glm::mat4 transPlayer(1.0f);
	transPlayer = glm::scale(transPlayer, glm::vec3(sx, sy, sz));

	//create transform for sword
	glm::mat4 transSword(1.0f);
	transSword = glm::scale(transSword, glm::vec3(sx, sy, sz));
	
	
	glfwSetCursorPosCallback(window, cursor_position_callback);
	glfwSetFramebufferSizeCallback(window, window_callback);


	// Check if the window was closed
	while (!glfwWindowShouldClose(window))
	{
		// Swap buffers
		glfwSwapBuffers(window);

		// Check for events
		glfwPollEvents();

		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT);

		// Player boundary checks
		if (playerPos.x + playerSizeX > 1.5f) // Adjusted right boundary to 1.5f
			playerPos.x = 1.5f - playerSizeX;
		if (playerPos.x - playerSizeX < -0.1f)
			playerPos.x = -0.1f + playerSizeX;

		// Check for input
		if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
			playerPos.x += playerSpeed;
		if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
			playerPos.x -= playerSpeed;
		if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && !isJumping)
		{
			playerVelocityY = jumpStrength;
			isJumping = true;
		}

		// Apply gravity
		if (isJumping)
		{
			playerPos.y += playerVelocityY; // Update player's vertical position
			playerVelocityY -= gravity; // Apply gravity to vertical velocity
		}

		// Simulate ground collision
		if (playerPos.y < 0.0f) // Assuming ground is at y = 0.0
		{
			playerPos.y = 0.0f;
			isJumping = false;
			playerVelocityY = 0.0f;
		}


		// Update player transformation matrix
		transPlayer = glm::mat4(1.0f);
		transPlayer = glm::scale(transPlayer, glm::vec3(sx, sy, sz));
		transPlayer = glm::translate(transPlayer, playerPos);
		transPlayer = glm::rotate(transPlayer, 108.0f, glm::vec3(sx, sy, 1.5f)); // Rotate around the Z-axis

		// Update sword transformation matrix
		transSword = glm::mat4(1.0f);
		transSword = glm::translate(transSword, playerPos);
		transSword = glm::scale(transSword, glm::vec3(sx, sy, sz));
		transSword = glm::rotate(transSword, 108.0f, glm::vec3(sx, sy, 1.5f)); // Rotate around the Z-axis

		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
			// Increase the swing angle by a certain amount
			swingAngle += 0.04f;

			// Apply rotation to the sword && move with the player
			transSword = glm::rotate(transSword, swingAngle, glm::vec3(sx, sy, 1.5f)); // Rotate around the Z-axis
			
		}

		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_RELEASE) {
			// Return to 0
			swingAngle = 0.0f;
		}


		// Use our shader
		glUseProgram(programID);
		unsigned int transformLocMount = glGetUniformLocation(programID, "transform");
		glUniformMatrix4fv(transformLocMount, 1, GL_FALSE, glm::value_ptr(transMount));
		glBindVertexArray(vaoM);
		glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, 0);

		glUseProgram(programID2);
		unsigned int transformLocPlayer = glGetUniformLocation(programID2, "transform");
		glUniformMatrix4fv(transformLocPlayer, 1, GL_FALSE, glm::value_ptr(transPlayer));
		glBindVertexArray(vaoP);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		glUseProgram(programID3);
		unsigned int transformLocSword = glGetUniformLocation(programID3, "transform");
		glUniformMatrix4fv(transformLocSword, 1, GL_FALSE, glm::value_ptr(transSword));
		glBindVertexArray(vaoS);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		
	} 
	
	// Cleanup
	glDeleteBuffers(1, &vboM);
	glDeleteBuffers(1, &vboP);
	glDeleteBuffers(1, &vboS);
	glDeleteBuffers(1, &iboM);
	glDeleteBuffers(1, &iboP);
	glDeleteBuffers(1, &iboS);
	glDeleteVertexArrays(1, &vaoM);
	glDeleteVertexArrays(1, &vaoP);
	glDeleteVertexArrays(1, &vaoS);
	glDeleteProgram(programID);
	glDeleteProgram(programID2);
	glDeleteProgram(programID3);

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}


