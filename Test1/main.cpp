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

float sx = 1.0f, sy = 1.0f, sz = 0.0f;

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
	GLFWwindow* window = glfwCreateWindow(800, 800, "Gradient Triangles", NULL, NULL);
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

	glViewport(0, 0, 800, 800);

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

	float vertices[] = {
		0.0f,  0.5f, 0.0f,
	   -0.5f, -0.5f, 0.0f,
		0.5f, -0.5f, 0.0f,

		0.25f, -0.5f, 0.0f,
		0.5f, 0.25f, 0.0f,
		1.0f, -0.5f, 0.0f,

	   -0.25f, -0.5f, 0.0f,
	   -0.5f, 0.25f, 0.0f,
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

	while (!glfwWindowShouldClose(window)) {
		glClearColor(0.8f, 0.9f, 1.0f, 0.8f);
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(shaderProgram);
		glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "transform"), 1, GL_FALSE, glm::value_ptr(trans));

		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 9, GL_UNSIGNED_INT, 0);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	glDeleteProgram(shaderProgram);

	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}
