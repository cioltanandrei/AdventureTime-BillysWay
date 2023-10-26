#include <iostream>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GLFW/glfw3.h>

// Vertex Shader source code
const char* vertexShaderSource = R"(
    #version 330 core
    layout(location = 0) in vec3 vertexPos;
    out float gradientFactor;
    uniform mat4 transform;
    void main() {
        gl_Position = transform * vec4(vertexPos, 1.0);
        gradientFactor = (vertexPos.y + 0.5) * 2.0;
    }
)";

// Fragment Shader source code
const char* fragmentShaderSource = R"(
    #version 330 core
    out vec4 fragColor;
    in float gradientFactor;
    void main() {
        vec3 darkBlue = vec3(0.0, 0.5, 1.0);
        vec3 lightBlue = vec3(0.8, 0.8, 1.0);
        vec3 color = mix(darkBlue, lightBlue, gradientFactor);
        fragColor = vec4(color, 1.0);
    }
)";

// Red Vertex Shader source code
const char* redVertexShaderSource = R"(
    #version 330 core
    layout(location = 0) in vec3 vertexPos;
    uniform mat4 transform;
    void main() {
        gl_Position = transform * vec4(vertexPos, 1.0);
    }
)";

// Red Fragment Shader source code
const char* redFragmentShaderSource = R"(
    #version 330 core
    out vec4 fragColor;
    void main() {
        fragColor = vec4(1.0, 0.0, 0.0, 1.0); // Red color
    }
)";

//mountaines
float sx = 1.0f, sy = 1.0f, sz = 0.0f;
//red triangle
//float rx = 0.8f, ry = 0.6f, rz = 0.0f;

//width and height for the window
int width = 1080, height = 1920;

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

int main() {
	// Initialize GLFW
	if (!glfwInit()) {
		std::cerr << "Failed to initialize GLFW" << std::endl;
		return -1;
	}

	// Set OpenGL version and profile
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Create a GLFW window
	GLFWwindow* window = glfwCreateWindow(height,width, "Gradient Triangles", NULL, NULL);
	if (!window) {
		std::cerr << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	// Load OpenGL functions using GLAD
	if (!gladLoadGL()) {
		std::cerr << "Failed to initialize GLAD" << std::endl;
		glfwTerminate();
		return -1;
	}

	glViewport(0, 0, height, width);



	//vertex shader, fragment shader and shader program for mountaines
	// Create Vertex Shader Object and compile it
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);

	// Create Fragment Shader Object and compile it
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);

	// Create Shader Program, attach shaders, and link
	GLuint shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	//redvertex shader, redfragment shader and redshader program for the red triangle
	// Create redVertex Shader Object and compile it
	GLuint redVertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(redVertexShader, 1, &redVertexShaderSource, NULL);
	glCompileShader(redVertexShader);

	// Create Red Fragment Shader Object and compile it
	GLuint redFragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(redFragmentShader, 1, &redFragmentShaderSource, NULL);
	glCompileShader(redFragmentShader);

	// Create Red Shader Program, attach shaders, and link
	GLuint redShaderProgram = glCreateProgram();
	glAttachShader(redShaderProgram, redVertexShader);
	glAttachShader(redShaderProgram, redFragmentShader);
	glLinkProgram(redShaderProgram);

	glDeleteShader(redVertexShader);
	glDeleteShader(redFragmentShader);

	//vertices, indices and VAO, VBO, EBO for the mountaines
	float vertices[] = {
		0.0f,  0.9f, 0.0f,
	   -0.5f, -0.5f, 0.0f,
		0.5f, -0.5f, 0.0f,

		0.25f, -0.5f, 0.0f,
		0.5f,  0.6f, 0.0f,
		1.0f, -0.5f, 0.0f,

	   -0.2f, -0.5f, 0.0f,
	   -0.5f, 0.6f, 0.0f,
	   -1.0f, -0.5f, 0.0f
	};

	GLuint indices[] = {
		0, 1, 2,
		3, 4, 5,
		6, 7, 8
	};

	GLuint VAO, VBO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	glm::mat4 trans(1.0f);
	trans = glm::scale(trans, glm::vec3(sx, sy, sz));

	glfwSetCursorPosCallback(window, cursor_position_callback);

	//redvertices, redindices and redVAO, redVBO, redEBO for the red triangle
	float redVertices[] = {
	-0.8f, -0.4f, 0.0f,    // Top-left vertex
	-0.6f, -0.4f, 0.0f,    // Top-right vertex
	-0.6f, -0.6f, 0.0f,    // Bottom-right vertex
	-0.8f, -0.6f, 0.0f     // Bottom-left vertex
	};
	// Define the indices for the square
	GLuint redIndices[] = { 0, 1, 2, 0, 2, 3 };

	// Create a new VAO, VBO, and EBO for the red triangle
	GLuint redVAO, redVBO, redEBO;
	glGenVertexArrays(1, &redVAO);
	glGenBuffers(1, &redVBO);
	glGenBuffers(1, &redEBO);

	// Bind the VAO and VBO for the red triangle
	glBindVertexArray(redVAO);
	glBindBuffer(GL_ARRAY_BUFFER, redVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(redVertices), redVertices, GL_STATIC_DRAW);

	// Bind the EBO for the red triangle
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, redEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(redIndices), redIndices, GL_STATIC_DRAW); 

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	//trans for the mountaines
	glm::mat4 redtrans(1.0f);
	redtrans = glm::scale(redtrans, glm::vec3(sx, sy, sz));
	redtrans = glm::translate(redtrans, glm::vec3(-0.9f, -0.9f, 0.0f)); // Adjust the translation for positioning

	glfwSetFramebufferSizeCallback(window, window_callback);

	while (!glfwWindowShouldClose(window)) {
		glClearColor(0.8f, 0.9f, 1.0f, 0.8f);
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(shaderProgram);
		glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "transform"), 1, GL_FALSE, glm::value_ptr(trans));

		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 9, GL_UNSIGNED_INT, 0);

		glUseProgram(redShaderProgram); // Assuming you have a shader program for red color
		glUniformMatrix4fv(glGetUniformLocation(redShaderProgram, "transform"), 1, GL_FALSE, glm::value_ptr(trans));

		glBindVertexArray(redVAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		glfwSwapBuffers(window);
		glfwPollEvents();

		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		{
			glfwSetWindowShouldClose(window, GLFW_TRUE);
			//glfwTerminate();
		}

	}

	//delete vao,vbo,ebo for the mountaines
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	glDeleteProgram(shaderProgram);

	//delete redvao,redvbo,red ebo for the red triangle
	glDeleteVertexArrays(1, &redVAO);
	glDeleteBuffers(1, &redVBO);
	glDeleteBuffers(1, &redEBO);
	glDeleteProgram(redShaderProgram);


	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}