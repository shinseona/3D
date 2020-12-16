#include "Controller.h"
#include "Renderer.h"

Renderer* renderer = Renderer::instance();

void Controller::MatrixInput()
{
	static double lastTime = glfwGetTime();

	double currentTime = glfwGetTime();
	float deltaTime = float(currentTime - lastTime);

	glm::vec3 direction(
		cos(verticalAngle) * sin(horizontalAngle),
		sin(verticalAngle),
		cos(verticalAngle) * cos(horizontalAngle)
	);

	glm::vec3 right = glm::vec3(
		sin(horizontalAngle - 3.14f / 2.0f),
		0,
		cos(horizontalAngle - 3.14f / 2.0f)
	);

	glm::vec3 up = glm::cross(right, direction);


	if (glfwGetKey(renderer->window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
		position -= right * deltaTime * speed;
	}

	if (glfwGetKey(renderer->window, GLFW_KEY_LEFT) == GLFW_PRESS) {
		position += right * deltaTime * speed;
	}

	float FoV = initialFoV;

	ProjectionMatrix = glm::perspective(glm::radians(FoV), 4.0f / 3.0f, 0.1f, 100.0f);
	
	ViewMatrix = glm::lookAt(
		position,           
		position + direction,
		up           
	);

	lastTime = currentTime;
}