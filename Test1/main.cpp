#include<iostream>
#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
//#include <shader.hpp>

// Vertex Shader source code
const char* vertexShaderSource = "#version 330 core\n"
// Input vertex data, different for all executions of this shader.
"layout(location = 0) in vec3 vertexPos;"

// Add a new output for the gradient factor
"out float gradientFactor;"

// Add uniform variable for the transform matrix
"uniform mat4 transform;"

"void main() {"

"gl_Position.xyz = vertexPos;"
"gl_Position.w = 1.0; //HOMOGENEOUS COORD"

// Apply the transform on the gl_Position => final position of each vertex
"gl_Position = transform * gl_Position;"

// Calculate the gradient factor using the y-coordinate.
// This assumes y goes from -0.5 (bottom) to 0.5 (top). Adjust as necessary.
"gradientFactor = vertexPos.y + 0.5;"
"}";

//Fragment Shader source code
const char* fragmentShaderSource = "#version 330 core\n"
// Output data
"out vec4 fragColor;"

// The input gradient factor from the vertex shader
"in float gradientFactor;"


"void main(){"
// Define the two colors for the gradient
"vec4 bottomColor = vec4(0.0, 0.5, 1.0, 1.0); // dark blue"
"vec4 topColor = vec4(0.8, 0.8, 1.0, 1.0);   // almost white"

// Interpolate between the two colors based on the gradientFactor
"fragColor = mix(bottomColor, topColor, gradientFactor);"

"}";

float sx = 1.0f, sy = 1.5f, sz = 0.0f;

int main()
{
	// Initialize GLFW
	glfwInit();



	// Tell GLFW what version of OpenGL we are using 
	// In this case we are using OpenGL 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	// Tell GLFW we are using the CORE profile
	// So that means we only have the modern functions
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);



	// Create a GLFWwindow object of 800 by 800 pixels, naming it "YoutubeOpenGL"
	GLFWwindow* window = glfwCreateWindow(2064, 1072, "YoutubeOpenGL", NULL, NULL);
	// Error check if the window fails to create
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	// Introduce the window into the current context
	glfwMakeContextCurrent(window);



	//Load GLAD so it configures OpenGL
	gladLoadGL();
	// Specify the viewport of OpenGL in the Window
	// In this case the viewport goes from x = 0, y = 0, to x = 800, y = 800
	glViewport(0, 0, 800, 800);

	// Create Vertex Shader Object and get its reference
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	// Attach Vertex Shader source to the Vertex Shader Object
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	// Compile the Vertex Shader into machine code
	glCompileShader(vertexShader);

	// Create Fragment Shader Object and get its reference
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	// Attach Fragment Shader source to the Fragment Shader Object
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	// Compile the Vertex Shader into machine code
	glCompileShader(fragmentShader);

	// Create Shader Program Object and get its reference
	GLuint shaderProgram = glCreateProgram();
	// Attach the Vertex and Fragment Shaders to the Shader Program
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	// Wrap-up/Link all the shaders together into the Shader Program
	glLinkProgram(shaderProgram);

	// Delete the now useless Vertex and Fragment Shader objects
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);



	// Vertices coordinates
	float vertices[] = {
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

	GLuint indices[] = { // note that we start from 0!
		0, 1, 2, // first triangle
		//1, 3, 2, // second triangle
		3,4,5,
		6,7,8,
		//1,4,2
	};

	// Create reference containers for the Vartex Array Object and the Vertex Buffer Object
	GLuint VAO, VBO;

	// Generate the VAO and VBO with only 1 object each
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	// Make the VAO the current Vertex Array Object by binding it
	glBindVertexArray(VAO);

	// Bind the VBO specifying it's a GL_ARRAY_BUFFER
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	// Introduce the vertices into the VBO
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	GLuint IBO_ID;

	glGenBuffers(1, &IBO_ID); //create buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO_ID); //bind it
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), &indices[0],
		GL_STATIC_DRAW); //upload data

	// Configure the Vertex Attribute so that OpenGL knows how to read the VBO
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	// Enable the Vertex Attribute so that OpenGL knows to use it
	glEnableVertexAttribArray(0);

	

	// Bind both the VBO and VAO to 0 so that we don't accidentally modify the VAO and VBO we created
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	
	glm::mat4 trans(1.0f);
	trans = glm::scale(trans, glm::vec3(sx, sy, sz));
	glShadeModel(GL_SMOOTH);


	// Main while loop
	while (!glfwWindowShouldClose(window))
	{
		// Specify the color of the background
		glClearColor(0.0f, 0.0f, 1.0f, 1.0f);
		// Clean the back buffer and assign the new color to it
		glClear(GL_COLOR_BUFFER_BIT);
		// Tell OpenGL which Shader Program we want to use
		glUseProgram(shaderProgram);
		// Bind the VAO so OpenGL knows to use it
		glBindVertexArray(VAO);
		// Draw the triangle using the GL_TRIANGLES primitive
		//glDrawArrays(GL_TRIANGLES, 0, 3);
		//glDrawArrays(GL_TRIANGLES, 2, 4);
		glDrawElements(GL_TRIANGLES, 9, GL_UNSIGNED_INT, 0);
		// Swap the back buffer with the front buffer
		glfwSwapBuffers(window);
		// Take care of all GLFW events
		glfwPollEvents();
	}

	// Use our shader
	glUseProgram(vertexShader);

	unsigned int transformLoc = glGetUniformLocation(vertexShader, "transform");
	glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(trans));

	// Delete all the objects we've created
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteProgram(shaderProgram);
	// Delete window before ending the program
	glfwDestroyWindow(window);
	// Terminate GLFW before ending the program
	glfwTerminate();
	return 0;
}