#include <stdio.h>
#include <algorithm>
#include <stdlib.h>

#include "FileManager.h"
#include "Renderer.h"
#include "Sphere.h"
#include "UserObject.h"


int main()
{
	FileManager* fileManager = new FileManager();

	Renderer* renderer = new Renderer();
	renderer->Initialize();

	UserObject* cube = new UserObject(
		fileManager, 
		"cube.obj", 
		"vs_w7.shader", "fs_w7.shader", 
		"uvtemplate.dds");

	cube->SetPosition(glm::vec3(3.0f, 3.0f, 0.0f));
	
	Sphere* sphere = new Sphere(fileManager);
	sphere->SetPosition(glm::vec3(0.0f, 0.0f, 0.0f));
	
	renderer->SetViewMatrix(glm::lookAt(
		glm::vec3(15, 4, 3), // Camera is at (4,3,3), in World Space
		glm::vec3(0, 0, 0), // and looks at the origin
		glm::vec3(0, -1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
	));

	do
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		renderer->draw(cube);
		renderer->draw(sphere);

		glfwSwapBuffers(renderer->GetWindow());
		glfwPollEvents();

	} while (glfwGetKey(renderer->GetWindow(), GLFW_KEY_ESCAPE) != GLFW_PRESS &&
		glfwWindowShouldClose(renderer->GetWindow()) == 0);

	glfwTerminate();

	delete cube;
	delete sphere;

	return 0;

}