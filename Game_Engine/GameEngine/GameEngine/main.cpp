#include "Graphics\window.h"
#include "Camera\camera.h"
#include "Shaders\shader.h"
#include "Model Loading\mesh.h"
#include "Model Loading\texture.h"
#include "Model Loading\meshLoaderObj.h"
#include <format>
#include <cstdlib>
#include <ctime>
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

bool mainQuestCompleted = false;
bool aliveInThisWorld = false;

// Function to generate a random float within a given range
float getRandomFloat(float min, float max) {
	return min + static_cast<float>(rand()) / RAND_MAX * (max - min);
}

void processKeyboardInput();

void RenderQuestUI()
{
	// Inside the game loop, after starting ImGui frame
	ImGui::Begin("Quests & Story");

	// Main Quest
	ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Main Quest - %s", mainQuestCompleted ? "Completed" : "Not Completed");
	//ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 1.0f), "Status: %s", mainQuestCompleted ? "Completed" : "Not Completed");

	if (ImGui::IsItemHovered())
		ImGui::SetTooltip("Main Quest: Find the lost pages of Enchiridion.");
	
	// Side quests
	ImGui::TextColored(ImVec4(0.5, 0.5f, 0.5f, 1.0f), "Side quests");
	
	if (ImGui::IsItemHovered())
		ImGui::SetTooltip("Side Quest: Search for maggots under the box.");

	if(aliveInThisWorld == false)
		ImGui::TextColored(ImVec4(0.5, 0.5f, 0.5f, 1.0f), "Press Enter to make \nyour presence felt.");

	//make text bigger
	ImGui::SetWindowFontScale(1.24);

	// End ImGui window
	ImGui::End();
}

float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

Window window("Game Engine", 800, 800);
Camera camera;

glm::vec3 lightColor = glm::vec3(1.0f);
glm::vec3 lightPos = glm::vec3(-180.0f, 300.0f, -200.0f);

const float PICKUP_DISTANCE = 20.0f;  //  units away
std::vector<Mesh> sceneMeshes;

bool isWithinPickupDistance(Camera& camera, Mesh& object, float pickupDistance) {
	glm::vec3 cameraPos = camera.getCameraPosition();
	glm::vec3 objectPos = object.getPosition();
	float distance = glm::length(cameraPos - objectPos);
	return distance <= pickupDistance;
}

int main()
{
	
	//Initialize ImGui
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	const char* fontPath = "imgui/arial.ttf";

	ImGuiIO& io = ImGui::GetIO();
	io.Fonts->AddFontFromFileTTF(fontPath, 20);

	//Setup ImGui style
	ImGui::StyleColorsDark();

	//Setup platform/Renderer bindings
	ImGui_ImplGlfw_InitForOpenGL(window.getWindow(), true);
	ImGui_ImplOpenGL3_Init("#version 330");

	glClearColor(0.2f, 0.8f, 1.0f, 1.0f);

	//building and compiling shader program
	Shader shader("Shaders/vertex_shader.glsl", "Shaders/fragment_shader.glsl");
	Shader sunShader("Shaders/sun_vertex_shader.glsl", "Shaders/sun_fragment_shader.glsl");
	Shader skyboxShader("Shaders/skybox_vertex_shader.glsl", "Shaders/skybox_fragment_shader.glsl");

	//Textures
	GLuint tex = loadBMP("Resources/Textures/wood.bmp");
	GLuint tex2 = loadBMP("Resources/Textures/rock.bmp");
	GLuint tex3 = loadBMP("Resources/Textures/snow.bmp");

	GLuint tex4 = loadBMP("Resources/Textures/t11_Diffuse.bmp");
	/*GLuint tex5 = loadBMP("Resources/Textures/t11_Normal.bmp");
	GLuint tex6 = loadBMP("Resources/Textures/t11_Specular.bmp");
	GLuint tex7 = loadBMP("Resources/Textures/t11_Glossiness.bmp");
	GLuint tex8 = loadBMP("Resources/Textures/t11_Height.bmp");*/

	GLuint tex9 = loadBMP("Resources/Textures/fantasy_sword.bmp");
	
	//declare a vector of faces
	std::vector<std::string> faces
	{
		"Resources/Textures/posx.jpg",
		"Resources/Textures/negx.jpg",
		"Resources/Textures/posy.jpg",
		"Resources/Textures/negy.jpg",
		"Resources/Textures/posz.jpg",
		"Resources/Textures/negz.jpg"
	};

	GLuint cubemapTexture = loadCubemap(faces);

	glEnable(GL_DEPTH_TEST);

	//Test custom mesh loading
	std::vector<Vertex> vert;
	vert.push_back(Vertex());
	vert[0].pos = glm::vec3(10.5f, 10.5f, 0.0f);
	vert[0].textureCoords = glm::vec2(1.0f, 1.0f);

	vert.push_back(Vertex());
	vert[1].pos = glm::vec3(10.5f, -10.5f, 0.0f);
	vert[1].textureCoords = glm::vec2(1.0f, 0.0f);

	vert.push_back(Vertex());
	vert[2].pos = glm::vec3(-10.5f, -10.5f, 0.0f);
	vert[2].textureCoords = glm::vec2(0.0f, 0.0f);

	vert.push_back(Vertex());
	vert[3].pos = glm::vec3(-10.5f, 10.5f, 0.0f);
	vert[3].textureCoords = glm::vec2(0.0f, 1.0f);

	vert[0].normals = glm::normalize(glm::cross(vert[1].pos - vert[0].pos, vert[3].pos - vert[0].pos));
	vert[1].normals = glm::normalize(glm::cross(vert[2].pos - vert[1].pos, vert[0].pos - vert[1].pos));
	vert[2].normals = glm::normalize(glm::cross(vert[3].pos - vert[2].pos, vert[1].pos - vert[2].pos));
	vert[3].normals = glm::normalize(glm::cross(vert[0].pos - vert[3].pos, vert[2].pos - vert[3].pos));

	std::vector<int> ind = { 0, 1, 3,
		1, 2, 3 };

	std::vector<Texture> textures;
	textures.push_back(Texture());
	textures[0].id = tex;
	textures[0].type = "texture_diffuse";

	std::vector<Texture> textures2;
	textures2.push_back(Texture());
	textures2[0].id = tex2;
	textures2[0].type = "texture_diffuse";

	std::vector<Texture> textures3;
	textures3.push_back(Texture());
	textures3[0].id = tex3;
	textures3[0].type = "texture_diffuse";

	std::vector<Texture> texturesCubeMap;
	texturesCubeMap.push_back(Texture());
	texturesCubeMap[0].id = cubemapTexture;
	texturesCubeMap[0].type = "texture_diffuse";

	std::vector<Texture> textures4;
	textures4.push_back(Texture());
	textures4[0].id = tex4;
	textures4[0].type = "texture_diffuse";

	std::vector<Texture> textures5;
	textures5.push_back(Texture());
	textures5[0].id = tex9;
	textures5[0].type = "texture_diffuse";



	Mesh mesh(vert, ind, textures3);


	// Create Obj files - easier :)
	// we can add here our textures :)
	MeshLoaderObj loader;
	Mesh sun = loader.loadObj("Resources/Models/sphere.obj");
	Mesh box = loader.loadObj("Resources/Models/cube.obj", textures);
	
	Mesh plane = loader.loadObj("Resources/Models/plane.obj", textures3);
	Mesh skybox = loader.loadObj("Resources/Models/sphere.obj", texturesCubeMap);
	Mesh tree = loader.loadObj("Resources/Models/t1.obj",textures4);
	Mesh sword = loader.loadObj("Resources/Models/Fantasy Sword Weapon OBJ.obj", textures5);
	skybox.setup();


	// Seed the random number generator
	srand(static_cast<unsigned int>(time(nullptr)));

	// Number of trees
	int numTrees = 50;

	// Range of positions
	float minX = -500.0f;
	float maxX = 500.0f;
	float minY = -8.0f;
	float maxY = -5.0f;
	float minZ = -500.0f;
	float maxZ = 500.0f;

	// Vector to store tree positions
	std::vector<glm::vec3> treePositions;


	// Generate random tree positions
	for (int i = 0; i < numTrees; ++i) {
		float x = getRandomFloat(minX, maxX);
		float y = getRandomFloat(minY, maxY);
		float z = getRandomFloat(minZ, maxZ);
		treePositions.push_back(glm::vec3(x, y, z));
	}


	//check if we close the window or press the escape button
	while (!window.isPressed(GLFW_KEY_ESCAPE) &&
		glfwWindowShouldClose(window.getWindow()) == 0)
	{
		window.clear();
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		processKeyboardInput();

		//test mouse input
		if (window.isMousePressed(GLFW_MOUSE_BUTTON_LEFT))
		{
			std::cout << "Pressing mouse button" << std::endl;
		}
		
		
		sceneMeshes.push_back(box);
		
		//std::cout << "Initial Position: " << mesh.getPosition().x << ", " << mesh.getPosition().y << ", " << mesh.getPosition().z << std::endl;
		//std::cout << "Camera Position: " << camera.getCameraPosition().x << ", " << camera.getCameraPosition().y << ", " << camera.getCameraPosition().z << std::endl;
		//End code for the box
		bool lKeyWasPressed = false;
		bool qKeyWasPressed = false;
		if (glfwGetKey(window.getWindow(), GLFW_KEY_L) == GLFW_PRESS && !lKeyWasPressed){  // If the pickup key is pressed
			for (Mesh& mesh : sceneMeshes) {
				if (!mesh.getIsHeld() && isWithinPickupDistance(camera, mesh, PICKUP_DISTANCE)) {
					mesh.hold();
					mesh.drawBool(false);				
					std::cout << "Object Picked" << std::endl;
					break; // Assuming you can only pick up one object at a time
				}
			}
			lKeyWasPressed = true;
		}
		else if (window.isPressed(GLFW_KEY_Q)) {  // If the release key is pressed
			for (Mesh& mesh : sceneMeshes) {
				if (mesh.getIsHeld()) {
					mesh.release();
					mesh.drawBool(true);
					std::cout << "Object Released" << std::endl;
					break; // Assuming you can only pick up one object at a time
				}
			}
			qKeyWasPressed = true;
		}
		//Code for the box
		glm::mat4 ViewMatrix = glm::lookAt(camera.getCameraPosition(), camera.getCameraPosition() + camera.getCameraViewDirection(), camera.getCameraUp());
		glm::mat4 ProjectionMatrix = glm::perspective(90.0f, window.getWidth() * 1.0f / window.getHeight(), 0.1f, 10000.0f);
		shader.use();
		GLuint MatrixID2 = glGetUniformLocation(shader.getId(), "MVP");
		GLuint ModelMatrixID = glGetUniformLocation(shader.getId(), "model");

		glm::mat4 ModelMatrix = glm::mat4(1.0);
		glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
		for (Mesh& mesh : sceneMeshes) {
			if (mesh.isDrawable()) {
				//translate the box close to the camera position
				//on the ground level
				mesh.updatePositionBasedOnCamera(camera);
				MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
				glUniformMatrix4fv(MatrixID2, 1, GL_FALSE, &MVP[0][0]);
				glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
				glUniform3f(glGetUniformLocation(shader.getId(), "lightColor"), lightColor.x, lightColor.y, lightColor.z);
				glUniform3f(glGetUniformLocation(shader.getId(), "lightPos"), lightPos.x, lightPos.y, lightPos.z);
				glUniform3f(glGetUniformLocation(shader.getId(), "viewPos"), camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);
				mesh.draw(shader); // Draw only if the mesh should be drawn
			}
			else
			{
				ModelMatrix = glm::translate(ModelMatrix, glm::vec3(0.0f, -10000.0f, -10000.0f));
				MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
				glUniformMatrix4fv(MatrixID2, 1, GL_FALSE, &MVP[0][0]);
				glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
				glUniform3f(glGetUniformLocation(shader.getId(), "lightColor"), lightColor.x, lightColor.y, lightColor.z);
				glUniform3f(glGetUniformLocation(shader.getId(), "lightPos"), lightPos.x, lightPos.y, lightPos.z);
				glUniform3f(glGetUniformLocation(shader.getId(), "viewPos"), camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);
				mesh.draw(shader);
			}
		}

		//// Code for the light ////

		sunShader.use();
		ProjectionMatrix = glm::perspective(90.0f, window.getWidth() * 1.0f / window.getHeight(), 0.1f, 10000.0f);
		ViewMatrix = glm::lookAt(camera.getCameraPosition(), camera.getCameraPosition() + camera.getCameraViewDirection(), camera.getCameraUp());
		GLuint MatrixID = glGetUniformLocation(sunShader.getId(), "MVP");

		//Test for one Obj loading = light source

		ModelMatrix = glm::mat4(1.0);
		ModelMatrix = glm::translate(ModelMatrix, lightPos);
		MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);

		sun.draw(sunShader);

		//// End code for the light ////
	


		///// Test plane Obj file //////
		shader.use();
		ModelMatrix = glm::mat4(1.0);
		ModelMatrix = glm::translate(ModelMatrix, glm::vec3(0.0f, 0.0f, 0.0f));
		ModelMatrix = glm::scale(ModelMatrix, glm::vec3(600.0f, 600.0f, 600.0f));
		MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
		glUniformMatrix4fv(MatrixID2, 1, GL_FALSE, &MVP[0][0]);
		glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
		glUniform3f(glGetUniformLocation(shader.getId(), "lightColor"), lightColor.x, lightColor.y, lightColor.z);
		glUniform3f(glGetUniformLocation(shader.getId(), "lightPos"), lightPos.x, lightPos.y, lightPos.z);
		glUniform3f(glGetUniformLocation(shader.getId(), "viewPos"), camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);
		plane.draw(shader);

		///// Test Obj files for box ////
		
		//Test for multiple Obj loading = trees
		// Render trees
		shader.use(); // Assuming you have a shader that's appropriate for the trees
		GLuint MatrixID3 = glGetUniformLocation(shader.getId(), "MVP");
		GLuint ModelMatrixID2 = glGetUniformLocation(shader.getId(), "model");
		ModelMatrix = glm::mat4(1.0);
		

		// Draw each tree
		for (const auto& position : treePositions)
		{
			ModelMatrix = glm::mat4(1.0);
			ModelMatrix = glm::translate(ModelMatrix, position);
			// Scale if necessary
			ModelMatrix = glm::scale(ModelMatrix, glm::vec3(0.05f)); // Scale uniformly, adjust as needed

			glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
			glUniformMatrix4fv(MatrixID3, 1, GL_FALSE, &MVP[0][0]);
			glUniformMatrix4fv(ModelMatrixID2, 1, GL_FALSE, &ModelMatrix[0][0]);
			glUniformMatrix4fv(ModelMatrixID2, 1, GL_FALSE, &MVP[0][0]);
			glUniform3f(glGetUniformLocation(shader.getId(), "lightColor"), lightColor.x, lightColor.y, lightColor.z);
			glUniform3f(glGetUniformLocation(shader.getId(), "lightPos"), lightPos.x, lightPos.y, lightPos.z);
			glUniform3f(glGetUniformLocation(shader.getId(), "viewPos"), camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);
			tree.draw(shader); // Assuming your Mesh class has a draw method
		}
		
		///// Test Obj files for sword ////
		shader.use();

		GLuint MatrixID4 = glGetUniformLocation(shader.getId(), "MVP");
		GLuint ModelMatrixID3 = glGetUniformLocation(shader.getId(), "model");
		ModelMatrix = glm::mat4(1.0);
		ModelMatrix = glm::translate(ModelMatrix, glm::vec3(10.0f, -3.0f, 0.0f));
		ModelMatrix = glm::scale(ModelMatrix, glm::vec3(1.0f));

		MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
		glUniformMatrix4fv(MatrixID4, 1, GL_FALSE, &MVP[0][0]);
		glUniformMatrix4fv(ModelMatrixID3, 1, GL_FALSE, &ModelMatrix[0][0]);
		glUniformMatrix4fv(ModelMatrixID3, 1, GL_FALSE, &MVP[0][0]);
		glUniform3f(glGetUniformLocation(shader.getId(), "lightColor"), lightColor.x, lightColor.y, lightColor.z);
		glUniform3f(glGetUniformLocation(shader.getId(), "lightPos"), lightPos.x, lightPos.y, lightPos.z);
		glUniform3f(glGetUniformLocation(shader.getId(), "viewPos"), camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);

		sword.draw(shader);



		//// Test skybox ////
		glDepthFunc(GL_LEQUAL);  // Change depth function to allow depth test pass when values are equal
		skyboxShader.use();  // Use the skybox shader

		// Set the projection matrix
		GLuint projectionLoc = glGetUniformLocation(skyboxShader.getId(), "projection");
		glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(ProjectionMatrix));

		// Set the view matrix (without translation)
		 ViewMatrix = glm::mat4(glm::mat3(camera.getViewMatrix()));
		GLuint viewLoc = glGetUniformLocation(skyboxShader.getId(), "view");
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(ViewMatrix));

		// Draw the skybox
		skybox.draw2(skyboxShader, cubemapTexture);  // Assuming this method correctly sets up and draws the skybox

		glDepthFunc(GL_LESS);  // Restore the default depth function
		
		//// End test skybox ////

		//gui
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		//Inside the game loop, after starting ImGui frame
		RenderQuestUI();

		//Rendering ImGui
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	
		window.update();
	}
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

void processKeyboardInput()
{
	float cameraSpeed = 30 * deltaTime;

	//translation
	if (window.isPressed(GLFW_KEY_W))
		camera.keyboardMoveFront(cameraSpeed);
	if (window.isPressed(GLFW_KEY_S))
		camera.keyboardMoveBack(cameraSpeed);
	if (window.isPressed(GLFW_KEY_A))
		camera.keyboardMoveLeft(cameraSpeed);
	if (window.isPressed(GLFW_KEY_D))
		camera.keyboardMoveRight(cameraSpeed);
	if (window.isPressed(GLFW_KEY_R))
		camera.keyboardMoveUp(cameraSpeed);
	if (window.isPressed(GLFW_KEY_F)) 
		camera.keyboardMoveDown(cameraSpeed);

	//rotation
	if (window.isPressed(GLFW_KEY_LEFT))
		camera.rotateOy(cameraSpeed);
	if (window.isPressed(GLFW_KEY_RIGHT))
		camera.rotateOy(-cameraSpeed);
	if (window.isPressed(GLFW_KEY_UP))
		camera.rotateOx(cameraSpeed);
	if (window.isPressed(GLFW_KEY_DOWN))
		camera.rotateOx(-cameraSpeed);

	if (window.isPressed(GLFW_KEY_ENTER)) {
		aliveInThisWorld = true;
		mainQuestCompleted = true;
	}
}