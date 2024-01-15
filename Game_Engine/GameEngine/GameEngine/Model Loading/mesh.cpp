#include "mesh.h"

Mesh::Mesh(){}

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<int> indices, bool isHeld, bool shouldBeDrawn, glm::vec3 offsetFromCamera, glm::vec3 position)
	: isHeld(isHeld), shouldBeDrawn(shouldBeDrawn), offsetFromCamera(offsetFromCamera), position(position)
{
	this->vertices = vertices;
	this->indices = indices;

	setup2();
}


Mesh::Mesh(std::vector<Vertex> vertices, std::vector<int> indices, std::vector<Texture> textures)
{
	this->vertices = vertices;
	this->indices = indices;
	this->textures = textures;

	setup();
}


// render the mesh
void Mesh::draw(Shader shader)
{
	unsigned int diffuseNr = 1;
	unsigned int specularNr = 1;
	unsigned int normalNr = 1;
	unsigned int heightNr = 1;

	for (unsigned int i = 0; i < textures.size(); i++)
	{
		glActiveTexture(GL_TEXTURE0 + i); 
											
		std::string number;
		std::string name = textures[i].type;
		if (name == "texture_diffuse")
			number = std::to_string(diffuseNr++);
		else if (name == "texture_specular")
			number = std::to_string(specularNr++); 
		else if (name == "texture_normal")
			number = std::to_string(normalNr++);
		else if (name == "texture_height")
			number = std::to_string(heightNr++); 

		glUniform1i(glGetUniformLocation(shader.getId(), (name + number).c_str()), i);
		glBindTexture(GL_TEXTURE_2D, textures[i].id);
	}

	glBindVertexArray(vao);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	glActiveTexture(GL_TEXTURE0);
}

void Mesh::draw2(Shader shader, GLuint cubemapTexture)
{
	// Use the shader program
	shader.use();

	// Bind the cubemap texture to texture unit 0
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);

	// Set the samplerCube uniform to texture unit 0
	glUniform1i(glGetUniformLocation(shader.getId(), "skybox"), 0);

	// Draw mesh
	glBindVertexArray(vao);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	// Unbind the texture and set active texture back to default
	glActiveTexture(GL_TEXTURE0);
}



void Mesh::setup()
{
	//create buffers
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &ibo);

	//bind buffers
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normals));

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, textureCoords));

	glBindVertexArray(0);
}

//no textures yet
void Mesh::setup2()
{
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &ibo);

	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

	glBindVertexArray(0);
}

void Mesh::setTextures(std::vector<Texture> textures)
{
	this->textures = textures;
	setup();
}
// Method to "hold" the mesh at a certain offset from the camera
void Mesh::hold() {
	isHeld = true;
}

// Method to "release" the mesh
void Mesh::release() {
	isHeld = false;
}

// Method to check if the mesh is held
bool Mesh::getIsHeld() const {
	return isHeld;
}

// Method to get the offset from the camera when the mesh is held
glm::vec3 Mesh::getHeldPositionOffset() const {
	return offsetFromCamera;
}

// Method to set the position of the mesh
void Mesh::setPosition(const glm::vec3& newPosition) {
	position = newPosition;
}

// Method to get the current position of the mesh
glm::vec3 Mesh::getPosition() const {
	return position;
}

void Mesh::updatePositionBasedOnCamera(Camera& camera) {
	if (isHeld) {
		position = camera.getCameraPosition()
			+ camera.getCameraViewDirection() * offsetFromCamera.z
			+ camera.getCameraUp() * offsetFromCamera.y
			+ camera.getCameraRight() * offsetFromCamera.x;
		// Here you can set the orientation if needed

	}
}
void Mesh::drawBool(bool state)
{
	shouldBeDrawn = state;
}
bool Mesh::isDrawable()  { return shouldBeDrawn; }

Mesh::~Mesh() {}


