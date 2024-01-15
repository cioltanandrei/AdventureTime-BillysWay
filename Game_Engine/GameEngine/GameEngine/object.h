#pragma once
#include <glm.hpp>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <unordered_map>
#include "Model Loading\mesh.h"

/*class Object {
public:
	Mesh meshOfStaticObj;
	int noOfOccurences;
	std::vector<glm::vec3> spawnCoord;
};*/

class Collider {
public:
	virtual bool IsColliding(glm::vec3 colliderPos, glm::vec3 cameraPos) = 0;
};

class CylinderCollider : public Collider {
private:
	float range;
	float height;

public:
	CylinderCollider(float range, float height): range(range),
		height(height){}

	virtual bool IsColliding(glm::vec3 colliderPos, glm::vec3 cameraPos) override {
		bool collides = false;
		if (cameraPos.y - colliderPos.y <= height)
			collides = true;

		cameraPos.y = 0;
		colliderPos.y = 0;
		auto distance = glm::length(cameraPos - colliderPos);
		if (distance <= range && collides) {
			return true;
		}

		return false;
	}
};

class Object;

class Interactable {
private:
	Collider* collider;
public:
	Interactable(Collider* collider) : collider(collider) {}

	virtual void Interact(Object obj) = 0;
	Collider* GetCollider() {
		return collider;
	}
};

class Object {
public:
	std::string shaderName;
	std::string meshName;
	glm::vec3 pos;
	glm::vec3 scale;
	Collider *collider;
	Interactable* interactable;

	Object(std::string shaderName,
		std::string meshName,
		glm::vec3 pos,
		glm::vec3 scale,
		Collider* collider,
		Interactable* interactable) : shaderName(shaderName),
		meshName(meshName), pos(pos), scale(scale), collider(collider), interactable(interactable) {}

	bool operator==(Object const& other) const {
		return shaderName == other.shaderName &&
			meshName == other.meshName &&
			pos == other.pos &&
			scale == other.scale &&
			collider == other.collider &&
			interactable == other.interactable;
	}
};

class InteractNone : public Interactable {
public:
	InteractNone(Collider* collider) : Interactable(collider) {}
	virtual void Interact(Object obj) override {
		printf("Object interacted");
	}
};

class InteractPickup : public Interactable {
private:
	std::vector<Object> *objects;
	std::vector<Object>* pickupLocation;

public:
	InteractPickup(Collider *collider, std::vector<Object>* objects,
		std::vector<Object>* pickupLocation) : Interactable(collider),
		objects(objects), pickupLocation(pickupLocation){}


	virtual void Interact(Object obj) override {
		auto pos = std::find(objects->begin(), objects->end(), obj);
		pickupLocation->push_back(obj);
		objects->erase(pos);
	}
};

class Scene {
private:
	std::vector<Object> objects;
	std::unordered_map<std::string, Mesh> meshes;
	std::unordered_map<std::string, Shader*> shaders;

	glm::vec3 lightColor = glm::vec3(1.0f);
	glm::vec3 lightPos = glm::vec3(-180.0f, 300.0f, -200.0f);
	Camera *camera;
	Window* window;

public:
	Scene(Window *window, Camera *camera, glm::vec3 lightColor = glm::vec3(1.0f),
		glm::vec3 lightPos = glm::vec3(-180.0f, 300.0f, -200.0f)):
		window(window), camera(camera), lightColor(lightColor), lightPos(lightPos) {}

	std::vector<Object> *GetObjects() {
		return &objects;
	}

	void AddObject(Object &obj);
	void SetObjects(std::vector<Object>& objects);

	void AddMesh(std::string name, Mesh &mesh);
	void AddShader(std::string name, Shader *shader);
	void Draw();

	void Interact(glm::vec3 pos);

	bool IsColliding(glm::vec3 pos);
};
