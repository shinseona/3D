//#pragma comment(lib, "glew32s.lib")
#include <GLEW/glew.h>
#include <GLFW/glfw3.h>
#include <gl/GL.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <iostream>
#include "camera.h"

using namespace std;
#include "Helper.h"


// An abstract camera class that processes input and calculates the corresponding Eular Angles, Vectors and Matrices for use in OpenGL
// Camera Attributes
//player

// Constructor with vectors
Camera::Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch) :
	front(glm::vec3(0.0f, 0.0f, -1.0f)), movementSpeed(SPEED),
	mouseSensitivity(SENSITIVTY), zoom(ZOOM)
{
	this->position = position;
	modelMatrix = glm::translate(modelMatrix, position);
	worldUp = up;
	worldRight = glm::vec3(1.0f, 0.0f, 0.0f);
	this->yaw = 0.0f;
	this->pitch = -24.0f;


	gameRight = glm::vec3(0.0f, 0.0f, 1.0f);
	gameUp = worldUp;
	gameFront = glm::vec3(1.0f, 0.0f, 0.0f);
	updateCameraVectors();
}
// Constructor with scalar values
Camera::Camera(float posX, float posY, float posZ, 
	float upX, float upY, float upZ, 
	float yaw, float pitch) : 
	front(glm::vec3(0.0f, 0.0f, -1.0f)), 
	movementSpeed(SPEED), 
	mouseSensitivity(SENSITIVTY),
	zoom(ZOOM)
{
	position = glm::vec3(posX, posY, posZ);
	worldUp = glm::vec3(upX, upY, upZ);
	this->yaw = yaw;
	this->pitch = pitch;
	modelMatrix = glm::translate(modelMatrix, position);
	updateCameraVectors();
}

// Returns the view matrix calculated using Eular Angles and the LookAt Matrix
glm::mat4 Camera::getViewMatrix()
{
	return glm::lookAt(position, position + front, up);
	/*return glm::lookAt(glm::vec3(0.0f, 10.0f, 0.0f),
	glm::vec3(0.0f, 1.0f, 0.0f),
	up*glm::vec3(5.0f, 3.0f, 5.0f));*/
}

void Camera::goTo(float x, float y, float z) {
	modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(x, y, z));
	position = glm::vec3(x, y, z);
}

void Camera::translate(float x, float y, float z) {
	modelMatrix = glm::translate(modelMatrix, glm::vec3(x, y, z));
	position += glm::vec3(x, y, z);
}

glm::vec3 Camera::getPosition() {
	return position;
}

// Processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
void  Camera::processKeyboard(Camera_Movement direction, float deltaTime)
{
	deltaTime /= 1000;
	float velocity = movementSpeed * deltaTime + 0.50f;
	glm::vec3 tmpVec;
	if (direction == FORWARD) {
		tmpVec = gameFront * velocity;
		position += tmpVec;
	}
	if (direction == BACKWARD) {
		tmpVec = -gameFront * velocity;
		position += tmpVec;
	}
	if (direction == LEFT) {
		tmpVec = -gameRight * velocity;
		position += tmpVec;
	}
	if (direction == RIGHT) {
		tmpVec = gameRight * velocity;
		position += tmpVec;
	}
	modelMatrix = glm::translate(modelMatrix, tmpVec);
	
	cout << "position: ";
	Helper h;
	h.showVector(position);
}

// Processes input received from a mouse input system. Expects the offset value in both the x and y direction.
void  Camera::processMouseMovement(float xoffset, float yoffset, GLboolean constrainpitch)
{
	xoffset *= mouseSensitivity;
	yoffset *= mouseSensitivity;

	yaw += xoffset;
	pitch += yoffset;

	// Make sure that when pitch is out of bounds, screen doesn't get flipped
	if (constrainpitch)
	{
		if (pitch > 89.0f)
			pitch = 89.0f;
		if (pitch < -89.0f)
			pitch = -89.0f;
	}
	// update front, right and up Vectors using the updated Eular angles
	updateCameraVectors();
}

// Processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
void  Camera::processMouseScroll(float yoffset)
{
	//	if (zoom >= 1.0f && zoom <= 90.0f)
	zoom -= yoffset;
	/*if (zoom <= 10.0f)
		zoom = 10.0f;
	if (zoom >= 45.0f)
		zoom = 45.0f;*/
}

// Calculates the front vector from the Camera's (updated) Eular Angles
void Camera::updateCameraVectors()
{
	// Calculate the new front vector
	glm::vec3 front;
	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	front.y = sin(glm::radians(pitch));
	front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	this->front = glm::normalize(front);

	// Also re-calculate the right and up vector
	right = glm::normalize(glm::cross(front, worldUp));  // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
	up = glm::normalize(glm::cross(right, front));
}

float Camera::getZoom(){ return zoom; }
