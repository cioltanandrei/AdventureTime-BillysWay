#pragma once

#include <glm.hpp>
#include <gtx\transform.hpp>
#include <gtc\matrix_transform.hpp>
#include <gtc\type_ptr.hpp>
#include "..\Graphics\window.h"

class Camera
{
	private:
		glm::vec3 cameraPosition;
		glm::vec3 cameraViewDirection;
		glm::vec3 cameraUp;
		glm::vec3 cameraRight;

		//rotation - to be removed
		float rotationOx;
		float rotationOy;
		float planeLevelY;
		bool isJumping = false;
		float jumpHeight = 15.0f; // Adjust as needed
		float gravity = 20.8f; // gravity
		

	public:
		float sprintMultiplier = 1500.0f; // Adjust as needed
		float originalCameraSpeed;
		Camera();
		Camera(glm::vec3 cameraPosition);
		Camera(glm::vec3 cameraPosition, glm::vec3 cameraViewDirection, glm::vec3 cameraUp);
		~Camera();
		glm::mat4 getViewMatrix();
		glm::vec3 getCameraPosition();
		glm::vec3 getCameraViewDirection();
		glm::vec3 getCameraUp();
		glm::vec3 getCameraRight();
		glm::vec3 setCameraPosition(glm::vec3 newPosition);

		void keyboardMoveFront(float cameraSpeed);
		void keyboardMoveBack(float cameraSpeed);
		void keyboardMoveLeft(float cameraSpeed);
		void keyboardMoveRight(float cameraSpeed);
		void keyboardMoveUp(float cameraSpeed);
		void keyboardMoveDown(float cameraSpeed);
		void Camera::UpdateCamera(float deltaTime);
		void Camera::jump();
		void sprint(bool isSprinting, float& cameraSpeed);

		void rotateOx(float angle);
		void rotateOy(float angle);
};

