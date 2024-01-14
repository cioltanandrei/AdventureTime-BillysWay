#include "object.h"


void Scene::AddObject(Object &obj) {
	objects.push_back(obj);
}

void Scene::SetObjects(std::vector<Object>& objects) {
	this->objects.insert(this->objects.end(), objects.begin(), objects.end());
}

void Scene::AddMesh(std::string name, Mesh& mesh) {
	meshes[name] = mesh;
}


void Scene::AddShader(std::string name, Shader *shader) {
	shaders[name] = shader;
}

void Scene::Draw() {
	glm::mat4 ViewMatrix = glm::lookAt(camera->getCameraPosition(), camera->getCameraPosition() + camera->getCameraViewDirection(), camera->getCameraUp());
	glm::mat4 ProjectionMatrix = glm::perspective(90.0f, window->getWidth() * 1.0f / window->getHeight(), 0.1f, 10000.0f);

	glm::mat4 ViewProjection = ProjectionMatrix * ViewMatrix;
	glm::mat4 ModelMatrix;
	for (auto obj : objects) {
		//meshes[obj.meshName].draw();

		Shader* shader = shaders[obj.shaderName];

		shader->use(); // Assuming you have a shader that's appropriate for the trees
		GLuint MatrixID3 = glGetUniformLocation(shader->getId(), "MVP");
		GLuint ModelMatrixID2 = glGetUniformLocation(shader->getId(), "model");
		ModelMatrix = glm::mat4(1.0);
		ModelMatrix = glm::translate(ModelMatrix, obj.pos);
		// Scale if necessary
		//ModelMatrix = glm::scale(ModelMatrix, glm::vec3(1.0f)); // Scale uniformly, adjust as needed
		ModelMatrix = glm::scale(ModelMatrix, obj.scale); // Scale uniformly, adjust as needed

		glm::mat4 MVP = ViewProjection * ModelMatrix;
		glUniformMatrix4fv(MatrixID3, 1, GL_FALSE, &MVP[0][0]);
		glUniformMatrix4fv(ModelMatrixID2, 1, GL_FALSE, &ModelMatrix[0][0]);
		glUniformMatrix4fv(ModelMatrixID2, 1, GL_FALSE, &MVP[0][0]);
		glUniform3f(glGetUniformLocation(shader->getId(), "lightColor"), lightColor.x, lightColor.y, lightColor.z);
		glUniform3f(glGetUniformLocation(shader->getId(), "lightPos"), lightPos.x, lightPos.y, lightPos.z);
		glUniform3f(glGetUniformLocation(shader->getId(), "viewPos"), camera->getCameraPosition().x, camera->getCameraPosition().y, camera->getCameraPosition().z);
		meshes[obj.meshName].draw(*shader); // Assuming your Mesh class has a draw method
	}
}
/*
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

						mesh.draw(shader);*/

bool Scene::IsColliding(glm::vec3 pos) {
	for (auto obj : objects) {
		if (obj.collider->IsColliding(obj.pos, pos)) {
			return true;
		}
	}

	return false;
}

void Scene::Interact(glm::vec3 pos) {
	for (auto obj : objects) {
		if (obj.interactable->GetCollider()->IsColliding(obj.pos, pos)) {
			obj.interactable->Interact(obj);
		}
	}
}