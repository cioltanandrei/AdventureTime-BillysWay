#include "Graphics\window.h"
#include "Camera\camera.h"
#include "Shaders\shader.h"
#include "Model Loading\mesh.h"
#include "Model Loading\texture.h"
#include "Model Loading\meshLoaderObj.h"
#include "object.h"
#include <format>
#include <cstdlib>
#include <ctime>
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include <unordered_map>
#include <string>

#include <windows.h>

const int MAX_SCENES = 3;

bool mainQuestCompleted = false;
bool aliveInThisWorld = false;
int realmCounter = 0;

// Function to generate a random float within a given range
float getRandomFloat(float min, float max) {
	return min + static_cast<float>(rand()) / RAND_MAX * (max - min);
}

void processKeyboardInput(Scene& scene);

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
std::vector<vector<Mesh>> realmMeshesOfStaticObjects;

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
	GLuint tex6 = loadBMP("Resources/Textures/ChocoSanta.bmp");
	GLuint tex7 = loadBMP("Resources/Textures/CandyCorn.bmp");
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
	std::vector<std::string> faces1
	{
		"Resources/Textures/right.jpg",
		"Resources/Textures/left.jpg",
		"Resources/Textures/top.jpg",
		"Resources/Textures/bottom.jpg",
		"Resources/Textures/front.jpg",
		"Resources/Textures/back.jpg"
	};

	GLuint cubemapTexture = loadCubemap(faces);
	GLuint cubemapTexture1 = loadCubemap(faces1);
	std::vector<GLuint> cubemapTextures;
	cubemapTextures.push_back(cubemapTexture);
	cubemapTextures.push_back(cubemapTexture1);
	cubemapTextures.push_back(cubemapTexture);
	cubemapTextures.push_back(cubemapTexture1);

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

	std::vector<Texture> texturesCubeMap1;
	texturesCubeMap1.push_back(Texture());
	texturesCubeMap1[0].id = cubemapTexture1;
	texturesCubeMap1[0].type = "texture_diffuse";

	std::vector<Texture> textures4;
	textures4.push_back(Texture());
	textures4[0].id = tex4;
	textures4[0].type = "texture_diffuse";

	std::vector<Texture> textures5;
	textures5.push_back(Texture());
	textures5[0].id = tex9;
	textures5[0].type = "texture_diffuse";

	std::vector<Texture> textures6;
	textures6.push_back(Texture());
	textures6[0].id = tex6;
	textures6[0].type = "texture_diffuse";

	std::vector<Texture> textures7;
	textures7.push_back(Texture());
	textures7[0].id = tex7;
	textures7[0].type = "texture_diffuse";

	Mesh mesh(vert, ind, textures3);


	// Create Obj files - easier :)
	// we can add here our textures :)
	MeshLoaderObj loader;
	Mesh sun = loader.loadObj("Resources/Models/sphere.obj");
	Mesh box = loader.loadObj("Resources/Models/cube.obj", textures);
	
	Mesh plane = loader.loadObj("Resources/Models/plane.obj", textures3);
	Mesh skybox = loader.loadObj("Resources/Models/sphere.obj", texturesCubeMap);
	Mesh skybox1 = loader.loadObj("Resources/Models/sphere.obj", texturesCubeMap1);
	//Mesh tree = loader.loadObj("Resources/Models/t1.obj",textures4);
	//Mesh sword = loader.loadObj("Resources/Models/Fantasy Sword Weapon OBJ.obj", textures5);
	//Mesh santa = loader.loadObj("Resources/Models/ChocoSantaClaus06.obj", textures6);
	skybox.setup();

	//// SCENE ______________________________
	//std::vector<Scene> scenes; 
	std::vector<Scene*> scenes; 
	std::vector<Object> inventory;

	//int realmCounter = 0;
	
	/////////////// CREATING THE FIRST SCENE ///////////////

	scenes.push_back(new Scene(&window, &camera));
	scenes[0]->AddMesh("tree", loader.loadObj("Resources/Models/t1.obj", textures4));
	scenes[0]->AddShader("shader", &shader);

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
		//treePositions.push_back(glm::vec3(x, y, z));
		auto collider = new CylinderCollider(3.5, 100);
		//auto interact1 = new InteractNone(new CylinderCollider(7, 100)); // Interaction that does nothing 
		auto interact = new InteractPickup(new CylinderCollider(7, 100), scenes[0]->GetObjects(), &inventory); //Interaction that makes the object disappear

		scenes[0]->AddObject(Object("shader", "tree", glm::vec3(x, y, z), glm::vec3(0.05f), collider, interact));
	}

	scenes[0]->AddMesh("sword", loader.loadObj("Resources/Models/Fantasy Sword Weapon OBJ.obj", textures5));
	auto collider = new CylinderCollider(3.5, 100);
	auto interact1 = new InteractNone(new CylinderCollider(7, 100));
	//auto interact = new InteractPickup(new CylinderCollider(7, 100), scenes[0]->GetObjects(), &inventory);

	scenes[0]->AddObject(Object("shader", "sword", glm::vec3(10.0f, -3.0f, 0.0f), glm::vec3(1.0f), collider, interact1));

	//Scene* currentScene = &scenes[0];
	/////////////// END OF THE FIRST SCENE ///////////////
	


	/////////////// CREATING THE SECOND SCENE ///////////////

	scenes.push_back(new Scene(&window, &camera));
	scenes[1]->AddMesh("sword", loader.loadObj("Resources/Models/Fantasy Sword Weapon OBJ.obj", textures5));
	scenes[1]->AddShader("shader", &shader);
	collider = new CylinderCollider(3.5, 100);
	//auto interact = new InteractNone(new CylinderCollider(7, 100));
	auto interact = new InteractPickup(new CylinderCollider(7, 100), scenes[1]->GetObjects(), &inventory);

	scenes[1]->AddObject(Object("shader", "sword", glm::vec3(10.0f, -3.0f, 0.0f), glm::vec3(1.0f), collider, interact));

	/////////////// END OF THE SECOND SCENE ///////////////



	/////////////// CREATING THE THIRD SCENE ///////////////
	
	scenes.push_back(new Scene(&window, &camera));
	scenes[2]->AddMesh("tree", loader.loadObj("Resources/Models/t1.obj", textures4));
	scenes[2]->AddShader("shader", &shader);
	collider = new CylinderCollider(3.5, 100);
	//auto interact = new InteractNone(new CylinderCollider(7, 100));
	interact = new InteractPickup(new CylinderCollider(7, 100), scenes[2]->GetObjects(), &inventory);

	scenes[2]->AddObject(Object("shader", "tree", glm::vec3(10.0f, -3.0f, 0.0f), glm::vec3(0.05f), collider, interact));

	/////////////// END OF THE THIRD SCENE ///////////////



	/////////////// CREATING THE FOURTH SCENE ///////////////

	scenes.push_back(new Scene(&window, &camera));
	scenes[3]->AddMesh("tree", loader.loadObj("Resources/Models/t1.obj", textures4));
	scenes[3]->AddShader("shader", &shader);
	collider = new CylinderCollider(3.5, 100);
	//auto interact = new InteractNone(new CylinderCollider(7, 100));
	interact = new InteractPickup(new CylinderCollider(7, 100), scenes[3]->GetObjects(), &inventory);

	scenes[3]->AddObject(Object("shader", "tree", glm::vec3(20.0f, -3.0f, 0.0f), glm::vec3(0.05f), collider, interact));
	
	//scenes[3]->AddMesh("CandyCorn", loader.loadObj("Resources/Models/CandyCornOBJ.obj", textures7));
	//scenes[3]->AddShader("shader", &shader);
	//collider = new CylinderCollider(3.5, 100);
	//auto interact = new InteractNone(new CylinderCollider(7, 100));
	//interact = new InteractPickup(new CylinderCollider(7, 100), scenes[3]->GetObjects(), &inventory);

	//scenes[3]->AddObject(Object("shader", "CandyCorn", glm::vec3(10.0f, 0.0f, 0.0f), glm::vec3(50.0f), collider, interact));

	/////////////// END OF THE FOURTH SCENE ///////////////



	Scene* currentScene = scenes[realmCounter];
	//check if we close the window or press the escape button
	while (!window.isPressed(GLFW_KEY_ESCAPE) &&
		glfwWindowShouldClose(window.getWindow()) == 0)
	{
		currentScene = scenes[realmCounter];
		window.clear();
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		processKeyboardInput(*currentScene);

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

		currentScene->Draw();

		///// Test Obj files for box ////
		
		//Test for multiple Obj loading = trees
		// Render trees
		//bool test = true;
		/*if (realmCounter == 1 && test == true) {
			for(dataAboutStaticObj currentObj : testMeshes_WithSruct) {
			//for(i = 1; i < testMeshes_WithSruct.size(); i++ ) {
			//for(int i = 0; i < testMeshes.size(); i++) {
			//for (Mesh& mesh : testMeshes) {
				//Mesh mesh = testMeshes[i];
				Mesh mesh = currentObj.meshOfStaticObj;

				for(int j = 0; j < currentObj.noOfOccurences; j++) {
				//for (int j = 1; j < noOfStaticObj[i]; j++) {
					shader.use();
					GLuint MatrixID4 = glGetUniformLocation(shader.getId(), "MVP");
					GLuint ModelMatrixID3 = glGetUniformLocation(shader.getId(), "model");
					ModelMatrix = glm::mat4(1.0);
					//ModelMatrix = glm::translate(ModelMatrix, glm::vec3(10.0f, -3.0f, 0.0f));
					ModelMatrix = glm::translate(ModelMatrix, glm::vec3(currentObj.spawnCoord[j]));
					ModelMatrix = glm::scale(ModelMatrix, glm::vec3(1.0f));
					if (mesh.isDrawable()) {
						MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
						glUniformMatrix4fv(MatrixID4, 1, GL_FALSE, &MVP[0][0]);
						glUniformMatrix4fv(ModelMatrixID3, 1, GL_FALSE, &ModelMatrix[0][0]);
						glUniformMatrix4fv(ModelMatrixID3, 1, GL_FALSE, &MVP[0][0]);
						glUniform3f(glGetUniformLocation(shader.getId(), "lightColor"), lightColor.x, lightColor.y, lightColor.z);
						glUniform3f(glGetUniformLocation(shader.getId(), "lightPos"), lightPos.x, lightPos.y, lightPos.z);
						glUniform3f(glGetUniformLocation(shader.getId(), "viewPos"), camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);

						mesh.draw(shader);
					}
				}
			}
		}*/


		//if (realmCounter == 0) {
			/*shader.use(); // Assuming you have a shader that's appropriate for the trees
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
			}*/

			//currentScene->Draw();
		//}
		///// Test Obj files for sword ////
		//shader.use();
		/*if (realmCounter == 1 && test == false) {
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
		}
		*/
		//// Test SantaClaus ////
		/*shader.use();
		GLuint MatrixID5 = glGetUniformLocation(shader.getId(), "MVP");
		GLuint ModelMatrixID4 = glGetUniformLocation(shader.getId(), "model");
		ModelMatrix = glm::mat4(1.0);
		ModelMatrix = glm::translate(ModelMatrix, glm::vec3(10.0f, 5.0f, 0.0f));
		ModelMatrix = glm::scale(ModelMatrix, glm::vec3(1.0f));

		MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
		glUniformMatrix4fv(MatrixID5, 1, GL_FALSE, &MVP[0][0]);
		glUniformMatrix4fv(ModelMatrixID4, 1, GL_FALSE, &ModelMatrix[0][0]);
		glUniformMatrix4fv(ModelMatrixID4, 1, GL_FALSE, &MVP[0][0]);
		glUniform3f(glGetUniformLocation(shader.getId(), "lightColor"), lightColor.x, lightColor.y, lightColor.z);
		glUniform3f(glGetUniformLocation(shader.getId(), "lightPos"), lightPos.x, lightPos.y, lightPos.z);
		glUniform3f(glGetUniformLocation(shader.getId(), "viewPos"), camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);

		santa.draw(shader);*/


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
		//skybox.draw2(skyboxShader, cubemapTexture);  // Assuming this method correctly sets up and draws the skybox
		skybox.draw2(skyboxShader, cubemapTextures[realmCounter]);

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

void processKeyboardInput(Scene &scene)
{
	Camera oldPos = camera;
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
		if(realmCounter < MAX_SCENES)
			realmCounter++;
		Sleep(400);
	}

	if (window.isPressed(GLFW_KEY_Z)) {
		aliveInThisWorld = false;
		mainQuestCompleted = false;
		if (realmCounter > 0)
			realmCounter--;
		Sleep(400);
	}

	if (scene.IsColliding(camera.getCameraPosition())) {
		camera = oldPos;
	}

	// Interact
	if (window.isPressed(GLFW_KEY_E)) {
		scene.Interact(camera.getCameraPosition());
	}
}