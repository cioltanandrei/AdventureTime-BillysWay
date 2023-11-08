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
	GLuint indicesPlayer[] = {
		0, 1, 2,
		0, 2, 3
	};

	// Sword vertices: 2 rectangles forming a cross
	// Sword vertices updated to position the sword to the right of the player
	float verticesSword[] = {
		// Vertical rectangle (blade of the sword)
		-0.6f, -0.5f, 0.0f,  // Bottom left (now 0.01f to the right of the player's top-right vertex)
		-0.57f, -0.5f, 0.0f,  // Bottom right
		-0.6f,  0.1f, 0.0f,  // Top left
		-0.57f,  0.1f, 0.0f,  // Top right

		// Horizontal rectangle (cross-guard of the sword)
		-0.67f, -0.33f, 0.0f,  // Left bottom (now to the right, with a 0.1f gap between player and guard)
		-0.49f, -0.33f, 0.0f,  // Right bottom
		-0.67f, -0.28f, 0.0f,  // Left top
		-0.49f, -0.28f, 0.0f,  // Right top
	};


	// Sword indices
	GLuint indicesSword[] = {
		// Indices for the vertical rectangle (blade)
		0, 1, 2,  // First triangle of the blade
		1, 2, 3,  // Second triangle of the blade

		// Indices for the horizontal rectangle (cross-guard)
		4, 5, 6,  // First triangle of the cross-guard
		5, 6, 7,  // Second triangle of the cross-guard
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
	// Unbind VAO
	glBindVertexArray(0);

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
	// Unbind VAO
	glBindVertexArray(0);


	// Sword Buffers

	GLuint vboS, vaoS, iboS;
	glGenVertexArrays(1, &vaoS);
	glGenBuffers(1, &vboS);
	glGenBuffers(1, &iboS);

	// Bind VAO for sword
	glBindVertexArray(vaoS);

	// Bind VBO for sword
	glBindBuffer(GL_ARRAY_BUFFER, vboS);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verticesSword), verticesSword, GL_STATIC_DRAW);

	// Bind IBO for sword
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
	// Unbind VAO
	glBindVertexArray(0);

	//create transform for mountains
	glm::mat4 transMount(1.0f);
	transMount = glm::scale(transMount, glm::vec3(sx, sy, sz));
	glShadeModel(GL_SMOOTH);

	//create transform for player
	glm::mat4 transPlayer(1.0f);
	transPlayer = glm::scale(transPlayer, glm::vec3(sx, sy, sz));

	//create transform for sword
	glm::mat4 transSword(1.0f);
	glm::vec3 swordOffset = glm::vec3(-0.3f, -0.35f, 0.0f);
	float angle = glm::radians(-45.0f); // Convert 45 degrees to radians
	double lastFrameTime = glfwGetTime();
	float currentSwordAngle = 0.0f; // Current angle of the sword
	float targetSwordAngle = 0.0f; // Target angle for the sword when 'A' is pressed
	float swordRotationSpeed = 5.0f; // Speed at which the sword will rotate
	float swordBaseX = (-0.6f + -0.57f) / 2; // Average x position of the bottom vertices
	glm::vec3 pivotPoint = glm::vec3(swordBaseX, -0.5f, 0.0f); // Pivot is at the bottom center of the blade
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

		double currentFrameTime = glfwGetTime();
		double deltaTime = currentFrameTime - lastFrameTime;
		lastFrameTime = currentFrameTime;

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
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
			targetSwordAngle = glm::radians(-360.0f); // Desired target angle when 'A' is pressed
		}
		else if (glfwGetKey(window, GLFW_KEY_A) == GLFW_RELEASE) {
			targetSwordAngle = 0.0f; // Back to initial position
		}

		// Update currentSwordAngle towards targetSwordAngle smoothly
		if (currentSwordAngle < targetSwordAngle) {
			currentSwordAngle += swordRotationSpeed * deltaTime; // Increment angle smoothly
			if (currentSwordAngle > targetSwordAngle) // Avoid overshooting
				currentSwordAngle = targetSwordAngle;
		}
		else if (currentSwordAngle > targetSwordAngle) {
			currentSwordAngle -= swordRotationSpeed * deltaTime; // Decrement angle smoothly
			if (currentSwordAngle < targetSwordAngle) // Avoid overshooting
				currentSwordAngle = targetSwordAngle;
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
		transPlayer = glm::translate(transPlayer, playerPos);
		transPlayer = glm::scale(transPlayer, glm::vec3(sx, sy, sz));

		transSword = glm::mat4(1.0f);

		transSword = glm::translate(transSword, playerPos + swordOffset); // Position sword relative to player
		transSword = glm::translate(transSword, pivotPoint); // Move pivot to base of the sword
		transSword = glm::rotate(transSword, currentSwordAngle, glm::vec3(0.0f, 0.0f, 1.0f)); // Rotate around the base
		transSword = glm::translate(transSword, -pivotPoint); // Move pivot back to original position
		transSword = glm::scale(transSword, glm::vec3(0.5, 0.5, 0.5)); // Scale if necessary

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
		unsigned int transformLocSword = glGetUniformLocation(programID3, "transSword");
		glUniformMatrix4fv(transformLocSword, 1, GL_FALSE, glm::value_ptr(transSword));
		glBindVertexArray(vaoS);
		glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, 0);
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


