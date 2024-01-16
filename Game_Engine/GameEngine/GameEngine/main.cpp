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
#include <cstdio>

#include <windows.h>

const int MAX_SCENES = 5;

bool mainQuestCompleted = false;
bool aliveInThisWorld = false;
bool gameStarted = false;
bool levelEnded = false;
int gameStage = 0;
int mainQuestItemsFound = 0;

int realmCounter = 0;

const int MAX_GAME_STAGES = 15;

std::vector<bool> hasInteractedWithSceneNPC;
std::vector<Object> interactedObjects;
//std::vector<string> Collectables;
//std::unordered_map<std::string, int> Collectables;

struct dataAboutCollectables {
	std::string name;
	int howMany;
};
std::vector<dataAboutCollectables> Collectables;

glm::vec2 mousePos = glm::vec2(0.0f);
glm::vec2 lastMousePos = glm::vec2(0.0f);
// Function to generate a random float within a given range
float getRandomFloat(float min, float max) {
	return min + static_cast<float>(rand()) / RAND_MAX * (max - min);
}

void processKeyboardInput(Scene& scene);

//void processKeyboardInput();
void processMouseMove();
void processMouseMove2(GLFWwindow* window, Camera& camera, glm::vec2& lastMousePos);

//std::unordered_map<int, FILE*> files;
//FILE* files[];
std::vector<FILE*> files;

FILE* stream;
errno_t err = fopen_s(&stream, "Resources/Text/introduction.txt", "r");


FILE* stream1;
errno_t err1 = fopen_s(&stream1, "Resources/Text/test.txt", "r");

FILE* stream2;
errno_t err2 = fopen_s(&stream2, "Resources/Text/test1.txt", "r");

FILE* stream3;
errno_t err3 = fopen_s(&stream3, "Resources/Text/test2.txt", "r");

// Open for read (will fail if file "test.in" doesn't exist)
//err = fopen_s(&stream, "Resources/test.in", "r");
char myString[100001];

void emptyMyString() {
	strcpy_s(myString, " ");
}

void readFromFile(FILE* stream) {
	char line[300 + 1] = "";  /* initialize all to 0 ('\0') */

	while (fgets(line, sizeof(line), stream)) { 
		strcat_s(myString, line);
	}
}

void closeMyFile(FILE* stream) {
	fclose(stream);
}


void RenderQuestUI()
{
	//readFromFile(files[gameStage]);
	ImGui::Begin("Quests & Story");

	//if (gameStarted == true && gameStage == 2) {
	if (gameStarted == false) {
		readFromFile(files[0]);
		//ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%s", text);
		ImGui::TextColored(ImVec4(0.5f, 1.0f, 0.5f, 1.0f), "%s", myString);
		//printf("%s", myString);

		if (ImGui::Button("Thanks bro!")) {
			gameStarted = true;
			closeMyFile(files[0]);
			gameStage++;
			emptyMyString();
			//hasInteractedWithSceneNPC[realmCounter] = false;
			//interactedObjects.clear();
			Sleep(400);
		}
	}
	else {
		if (hasInteractedWithSceneNPC[realmCounter] == false && levelEnded == false) {
			// Main Quest
			ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Main Quest - %s", mainQuestCompleted ? "Completed" : "Not Completed");
			//ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 1.0f), "Status: %s", mainQuestCompleted ? "Completed" : "Not Completed");

			if (ImGui::IsItemHovered())
				ImGui::SetTooltip("Main Quest: Find the lost pages of Enchiridion. - %d / 5", mainQuestItemsFound);

			// Side quests
			ImGui::TextColored(ImVec4(0.5, 0.5f, 0.5f, 1.0f), "Side quests");

			if (ImGui::IsItemHovered())
				ImGui::SetTooltip("Side Quest: Search for maggots under the box.");

			if (aliveInThisWorld == false)
				ImGui::TextColored(ImVec4(0.5, 0.5f, 0.5f, 1.0f), "Press Enter to make \nyour presence felt.");
		}
		else {
			if (hasInteractedWithSceneNPC[realmCounter] == true) {
				int fileNo = realmCounter * 2 + 1;
				readFromFile(files[fileNo]);

				ImGui::TextColored(ImVec4(0.5f, 1.0f, 0.5f, 1.0f), "%s \n Press Enter to cotinue", myString);
				/*if (ImGui::Button("Ok, bro!")) {
					hasInteractedWithSceneNPC[realmCounter] = false;
					interactedObjects.clear();
					Sleep(400);
				}*/
			}
			else {
				if (levelEnded == true) {
					//emptyMyString();
					int fileNo = realmCounter * 2 + 2;
					readFromFile(files[fileNo]);
					char testt[10001];
					ImGui::TextColored(ImVec4(0.5f, 1.0f, 0.5f, 1.0f), "%s \n Press Enter to cotinue", myString);
					//ImGui::InputText("", myString, size(myString));
					printf("%s", myString);
					//ImGui::TextColored(ImVec4(0.5f, 1.0f, 0.5f, 1.0f), "%s", myString);
					/*if (ImGui::Button("Ok, bro!")) {
						hasInteractedWithSceneNPC[realmCounter] = false;
						interactedObjects.clear();
						Sleep(400);
						levelEnded = false;
						gameStage = 1;
						if (realmCounter < MAX_SCENES)
							realmCounter++;
					}*/
				}
			}
			/*int fileNo = 0;
			//if (hasInteractedWithSceneNPC[realmCounter] == true)
				fileNo = realmCounter * 2 + 1;
			//else {
			if (levelEnded == true) {
					emptyMyString();
					fileNo++;
				}
			printf("FILE %d ",fileNo);
			//}
			//if (gameStage == 2 && levelEnded == false)
				//gameStage = 1;
			
			//readFromFile(files[realmCounter + 1]);
			//if(sizeof(myString) / sizeof(char) == 0)
			if ((fileNo % 2  == 1 && firstTime == true) || fileNo % 2 == 0) {
				readFromFile(files[fileNo]);
				firstTime = false;
			}
			//ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%s", text);
			ImGui::TextColored(ImVec4(0.5f, 1.0f, 0.5f, 1.0f), "%s", myString);
			//printf("%s", myString);
			//if (hasInteractedWithSceneNPC[realmCounter] == true)
				//gameStage--;

			if (ImGui::Button("Ok, bro!")) {
				//gameStarted = true;
				//closeMyFile(files[realmCounter + 1]);
				//if(gameStage % 2 != 0)
					//gameStage++;
				//emptyMyString();
				hasInteractedWithSceneNPC[realmCounter] = false;
				interactedObjects.clear();
				Sleep(400);
				if (levelEnded == true) {
					closeMyFile(files[fileNo]);
					emptyMyString();
					levelEnded = false;
					gameStage = 1;
					if(realmCounter < MAX_SCENES)
						realmCounter++;
				}
			}
			*/
		}
	}
	//make text bigger
	ImGui::SetWindowFontScale(1.24);
	// End ImGui window
	ImGui::End();
}
bool isSprinting = false;
//void processKeyboardInput();

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
	files.push_back(stream);
	files.push_back(stream1);
	files.push_back(stream2);
	files.push_back(stream3);

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
	GLuint tex10 = loadBMP("Resources/Textures/snowflake (2).bmp");


	//declare a vector of faces
	std::vector<std::string> faces_ice
	{
		"Resources/Textures/right_ice.jpg",
		"Resources/Textures/left_ice.jpg",
		"Resources/Textures/top_ice.jpg",
		"Resources/Textures/bottom_ice.jpg",
		"Resources/Textures/front_ice.jpg",
		"Resources/Textures/back_ice.jpg"
	};
	std::vector<std::string> faces_slime
	{
		"Resources/Textures/right_slime2.jpg",
		"Resources/Textures/left_slime2.jpg",
		"Resources/Textures/top_slime2.jpg",
		"Resources/Textures/bottom_slime2.jpg",
		"Resources/Textures/front_slime2.jpg",
		"Resources/Textures/back_slime2.jpg"
	};
	std::vector<std::string> faces_finn
	{
	"Resources/Textures/right_vf.jpg",
		"Resources/Textures/left_vf.jpg",
		"Resources/Textures/top_vf.jpg",
		"Resources/Textures/bottom_vf.jpg",
		"Resources/Textures/front_vf.jpg",
		"Resources/Textures/back_vf.jpg"
	};
	std::vector<std::string> faces_candy
	{
		"Resources/Textures/right_candy2.jpg",
		"Resources/Textures/left_candy2.jpg",
		"Resources/Textures/top_candy2.jpg",
		"Resources/Textures/bottom_candy2.jpg",
		"Resources/Textures/front_candy2.jpg",
		"Resources/Textures/back_candy2.jpg"
	};
	std::vector<std::string> faces_fire
	{ "Resources/Textures/right_fire.jpg",
		"Resources/Textures/left_fire.jpg",
		"Resources/Textures/top_fire.jpg",
		"Resources/Textures/bottom_fire.jpg",
		"Resources/Textures/front_fire.jpg",
		"Resources/Textures/back_fire.jpg"
	};


	GLuint cubemapTexture = loadCubemap(faces_ice);
	GLuint cubemapTexture1 = loadCubemap(faces_slime);
	GLuint cubemapTexture2 = loadCubemap(faces_finn);
	GLuint cubemapTexture3 = loadCubemap(faces_candy);
	GLuint cubemapTexture4 = loadCubemap(faces_fire);
	std::vector<GLuint> cubemapTextures;
	cubemapTextures.push_back(cubemapTexture);
	cubemapTextures.push_back(cubemapTexture1);
	cubemapTextures.push_back(cubemapTexture2);
	cubemapTextures.push_back(cubemapTexture3);
	cubemapTextures.push_back(cubemapTexture4);

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

	std::vector<Texture> texturesCubeMap2;
	texturesCubeMap2.push_back(Texture());
	texturesCubeMap2[0].id = cubemapTexture2;
	texturesCubeMap2[0].type = "texture_diffuse";

	std::vector<Texture> texturesCubeMap3;
	texturesCubeMap3.push_back(Texture());
	texturesCubeMap3[0].id = cubemapTexture3;
	texturesCubeMap3[0].type = "texture_diffuse";

	std::vector<Texture> texturesCubeMap4;
	texturesCubeMap4.push_back(Texture());
	texturesCubeMap4[0].id = cubemapTexture4;
	texturesCubeMap4[0].type = "texture_diffuse";

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

	//texture for snowflake
	std::vector<Texture> textures8;
	textures8.push_back(Texture());
	textures8[0].id = tex10;
	textures8[0].type = "texture_diffuse";


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
	Mesh skybox_ice = loader.loadObj("Resources/Models/sphere.obj", texturesCubeMap);
	Mesh skybox_slime = loader.loadObj("Resources/Models/sphere.obj", texturesCubeMap1);
	Mesh skybox_finn = loader.loadObj("Resources/Models/sphere.obj", texturesCubeMap2);
	Mesh skybox_candy = loader.loadObj("Resources/Models/sphere.obj", texturesCubeMap3);
	Mesh skybox_fire = loader.loadObj("Resources/Models/sphere.obj", texturesCubeMap4);
	//Mesh tree = loader.loadObj("Resources/Models/t1.obj",textures4);
	Mesh sword = loader.loadObj("Resources/Models/Fantasy Sword Weapon OBJ.obj", textures5);
	Mesh snowflake = loader.loadObj("Resources/Models/snowflake.obj", textures8);
	//Mesh santa = loader.loadObj("Resources/Models/ChocoSantaClaus06.obj", textures6);
	skybox_ice.setup();

	std::vector<Mesh> planeMeshes;
	Mesh plane1 = loader.loadObj("Resources/Models/plane.obj", textures3);
	planeMeshes.push_back(plane1);
	planeMeshes.push_back(plane1);
	planeMeshes.push_back(plane1);
	planeMeshes.push_back(plane1);
	planeMeshes.push_back(plane1);

	// Animation time
	float snowflakeAnimationTime = 0.0f;
	//// SCENE ______________________________
	//std::vector<Scene> scenes; 
	std::vector<Scene*> scenes;
	std::vector<Object> inventory;
	std::vector<string> interactiveNPCSInScenes;

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
	auto interact1 = new InteractNone(new CylinderCollider(7, 100), scenes[0]->GetObjects(), &interactedObjects);
	//auto interact = new InteractPickup(new CylinderCollider(7, 100), scenes[0]->GetObjects(), &inventory);

	scenes[0]->AddObject(Object("shader", "sword", glm::vec3(10.0f, -3.0f, 0.0f), glm::vec3(1.0f), collider, interact1));

	interactiveNPCSInScenes.push_back("sword");
	hasInteractedWithSceneNPC.push_back(false);
	Collectables.push_back({"sword", 1});
	//Collectables["sword"] = 1; /// how many swords do i need to collect
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

	/////////////// CREATING THE FIFTH SCENE ///////////////
	scenes.push_back(new Scene(&window, &camera));
	scenes[4]->AddMesh("tree", loader.loadObj("Resources/Models/t1.obj", textures4));
	scenes[4]->AddShader("shader", &shader);
	collider = new CylinderCollider(3.5, 100);
	//auto interact = new InteractNone(new CylinderCollider(7, 100));
	interact = new InteractPickup(new CylinderCollider(7, 100), scenes[4]->GetObjects(), &inventory);

	scenes[4]->AddObject(Object("shader", "tree", glm::vec3(20.0f, -3.0f, 0.0f), glm::vec3(0.05f), collider, interact));

	/////////////// END OF THE FIFTH SCENE ///////////////


	Scene* currentScene = scenes[realmCounter];
	//check if we close the window or press the escape button
	while (!window.isPressed(GLFW_KEY_ESCAPE) &&
		glfwWindowShouldClose(window.getWindow()) == 0)
	{
		currentScene = scenes[realmCounter];
		for (auto obj : interactedObjects) {
			if (obj.meshName == interactiveNPCSInScenes[realmCounter]) {
				hasInteractedWithSceneNPC[realmCounter] = true;
			}
			/*if (gameStage <= MAX_GAME_STAGES) {
				if(gameStage >=1 && gameStage <= 3)
				if (obj.meshName == "sword" && gameStage >= 1 && gameStage <= 3) {
					gameStage++;
				}
				if (obj.meshName == "tree" && gameStage >= 1 && gameStage <= 3) {
					gameStage++;
				}
			}*/
			/*if (obj.interactable->GetCollider()->IsColliding(obj.pos, pos)) {
				obj.interactable->Interact(obj);
			}*/
		}
		/*
		for (auto obj : inventory) {
			if (obj.meshName == Collectables[realmCounter].name) {

			}
		}*/
		if (inventory.size() == Collectables[realmCounter].howMany) {
			levelEnded = true;
			mainQuestItemsFound++;
		}
			//Collectables.find(Collectables.begin(), Collectables.end(), obj.meshName);
			//std::find(Collectables.begin(), Collectables.end(), obj.meshName);
			//std::unordered_map<std::string, int>::const_iterator got = Collectables.find(obj.meshName);
			//if(got->second == )
			//if(obj.meshName == Collectables[realmCounter])
		//if (gameStage % 3 == 0)
			//interactedObjects.clear();
		window.clear();
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		if (gameStarted == true) {
			processKeyboardInput(*currentScene);
			processMouseMove2(window.getWindow(), camera, lastMousePos);
		}
		//processKeyboardInput(*currentScene);
		//processKeyboardInput();
		/*double xpos, ypos;
		window.getMousePos(xpos, ypos);
		mousePos = glm::vec2(xpos/ window.getWidth(), ypos/ window.getHeight());
		if(lastMousePos != mousePos)
		{ 
			processMouseMove();
			lastMousePos = mousePos;
		}*/
		//processMouseMove2(window.getWindow(), camera, lastMousePos);

		// Update camera position based on keyboard input
		//processKeyboardInput();

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
		//plane.draw(shader);
		planeMeshes[realmCounter].draw(shader);
		//skybox.draw2(skyboxShader, cubemapTextures[realmCounter]);

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

			tree.draw(shader); // Assuming your Mesh class has a draw method
		}
		*/
		///// Test Obj files for sword ////
		shader.use();
		// Camera parameters
		glm::vec3 cameraPos = camera.getCameraPosition();
		glm::vec3 cameraDir = camera.getCameraViewDirection();
		glm::vec3 cameraUp = camera.getCameraUp();
		glm::vec3 cameraRight = camera.getCameraRight();
		// Forward offset (how far in front of the camera)
		float forwardDistance = 15.0f; // Adjust this value as needed

		// Upward offset (how far above the camera's position)
		float upwardDistance = 1.0f; // Adjust this value as needed

		// Rightward offset (should be zero if directly in front)
		float rightwardDistance = 5.0f; // Adjust this if you need the object to be right of the camera

		// Calculate the new object's position
		glm::vec3 objectPosition = cameraPos + (cameraDir * forwardDistance) + (cameraUp * upwardDistance) + (cameraRight * rightwardDistance);

		// Apply the position to the object's model matrix
		GLuint MatrixID4 = glGetUniformLocation(shader.getId(), "MVP");
		GLuint ModelMatrixID3 = glGetUniformLocation(shader.getId(), "model");
		ModelMatrix = glm::mat4(1.0);
		//ModelMatrix = glm::translate(ModelMatrix, glm::vec3(10.0f, 0.0f, 0.0f));
		ModelMatrix = glm::translate(ModelMatrix, objectPosition);
		ModelMatrix = glm::scale(ModelMatrix, glm::vec3(0.3f));
		//rotate the sword
		ModelMatrix = glm::rotate(ModelMatrix, glm::radians(50000.0f), glm::vec3(1.0f, 1.0f, 1.0f));
		MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
		glUniformMatrix4fv(MatrixID4, 1, GL_FALSE, &MVP[0][0]);
		glUniformMatrix4fv(ModelMatrixID3, 1, GL_FALSE, &ModelMatrix[0][0]);
		glUniformMatrix4fv(ModelMatrixID3, 1, GL_FALSE, &MVP[0][0]);
		glUniform3f(glGetUniformLocation(shader.getId(), "lightColor"), lightColor.x, lightColor.y, lightColor.z);
		glUniform3f(glGetUniformLocation(shader.getId(), "lightPos"), lightPos.x, lightPos.y, lightPos.z);
		glUniform3f(glGetUniformLocation(shader.getId(), "viewPos"), camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);

		sword.draw(shader);

		

		shader.use();
		GLuint MatrixID5 = glGetUniformLocation(shader.getId(), "MVP");
		GLuint ModelMatrixID4 = glGetUniformLocation(shader.getId(), "model");

		ModelMatrix = glm::mat4(1.0);
		ModelMatrix = glm::translate(ModelMatrix, snowflake.getPosition());
		ModelMatrix = glm::scale(ModelMatrix, glm::vec3(1.0f));  // Adjust scale as needed

		 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
		glUniformMatrix4fv(MatrixID5, 1, GL_FALSE, &MVP[0][0]);
		glUniformMatrix4fv(ModelMatrixID4, 1, GL_FALSE, &ModelMatrix[0][0]);
		glUniformMatrix4fv(ModelMatrixID4, 1, GL_FALSE, &MVP[0][0]);
		glUniform3f(glGetUniformLocation(shader.getId(), "lightColor"), lightColor.x, lightColor.y, lightColor.z);
		glUniform3f(glGetUniformLocation(shader.getId(), "lightPos"), lightPos.x, lightPos.y, lightPos.z);
		glUniform3f(glGetUniformLocation(shader.getId(), "viewPos"), camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);

		// Draw the snowflake
		snowflake.draw(shader);
		snowflake.updatePositionBasedOnAnimation(snowflakeAnimationTime);

		// Update the animation time
		snowflakeAnimationTime += deltaTime;

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
		skybox_ice.draw2(skyboxShader, cubemapTextures[realmCounter]);

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
void processMouseMove()
{
	float xoffset = mousePos.x - lastMousePos.x;
	float yoffset = mousePos.y - lastMousePos.y;
	camera.rotateOx(-yoffset * 180);
	camera.rotateOy(-xoffset * 180);
}
void processMouseMove2(GLFWwindow* window, Camera& camera, glm::vec2& lastMousePos) {
	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);
	glm::vec2 mousePos = glm::vec2(xpos, ypos);

	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
		float xoffset = xpos - lastMousePos.x;
		float yoffset = lastMousePos.y - ypos;  // Reversed since y-coordinates range from bottom to top

		float sensitivity = 0.1f; // Change this value to your liking
		xoffset *= sensitivity;
		yoffset *= sensitivity;

		camera.rotateOx(yoffset);
		camera.rotateOy(-xoffset);
	}

	lastMousePos = mousePos;	
}
void processKeyboardInput(Scene &scene)
{
	Camera oldPos = camera;
	//float cameraSpeed = 30 * deltaTime;
	float cameraSpeed = camera.originalCameraSpeed * deltaTime;

	//translation
	if (window.isPressed(GLFW_KEY_W))
		camera.keyboardMoveFront(cameraSpeed);
	if (window.isPressed(GLFW_KEY_S))
		camera.keyboardMoveBack(cameraSpeed);
	if (window.isPressed(GLFW_KEY_A))
		camera.keyboardMoveLeft(cameraSpeed);
	if (window.isPressed(GLFW_KEY_D))
		camera.keyboardMoveRight(cameraSpeed);

	if (window.isPressed(GLFW_KEY_ENTER)) {
		emptyMyString();
		hasInteractedWithSceneNPC[realmCounter] = false;
		interactedObjects.clear();
		Sleep(400);
		if (levelEnded == true) {
			levelEnded = false;
			gameStage = 1;
			if (realmCounter < MAX_SCENES)
				realmCounter++;
		}
		/*aliveInThisWorld = true;
		mainQuestCompleted = true;
		if(realmCounter < MAX_SCENES)
			realmCounter++;*/
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
	//rotation
	if (window.isPressed(GLFW_KEY_LEFT))
		camera.rotateOy(cameraSpeed);
	if (window.isPressed(GLFW_KEY_RIGHT))
		camera.rotateOy(-cameraSpeed);
	if (window.isPressed(GLFW_KEY_UP))
		camera.rotateOx(cameraSpeed);
	if (window.isPressed(GLFW_KEY_DOWN))
		camera.rotateOx(-cameraSpeed);

	//Player's actions : jump, sprint, gravity
	if (window.isPressed(GLFW_KEY_SPACE)) {
		camera.jump();
	}

	if (window.isPressed(GLFW_KEY_LEFT_SHIFT)) {
		isSprinting = true;
	}
	else {
		isSprinting = false;
	}

	// Call the sprint function
	camera.sprint(isSprinting, cameraSpeed);

	// Apply gravity
	camera.UpdateCamera(deltaTime);
}