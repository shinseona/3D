#include <stdio.h>
#include <algorithm>
#include <stdlib.h>

#include "FileManager.h"
#include "Renderer.h"
#include "Sphere.h"
#include "UserObject.h"
#include "SampleNonRender.h"


int main()
{
	FileManager* fileManager = new FileManager();

	Renderer* renderer = new Renderer();
	renderer->Initialize();

	//cube
	UserObject* cube = new UserObject(
		fileManager, 
		"cube.obj", 
		"uvtemplate.dds");

	cube->SetPosition(glm::vec3(2.0f, 2.0f, 0.0f));
	renderer->AddObject(cube);
	
	//Sphere
	Sphere* sphere = new  Sphere(fileManager);

	sphere->SetPosition(glm::vec3(0.0f, 0.0f, 0.0f));
	renderer->AddObject(sphere);

	//NonRenderableObject
	SampleNonRender* non_render_obj = new SampleNonRender();

	
	renderer->SetViewMatrix(glm::lookAt(
		glm::vec3(15, 4, 3), // Camera is at (4,3,3), in World Space
		glm::vec3(0, 0, 0), // and looks at the origin
		glm::vec3(0, -1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
	));

	while (true)
	{
		renderer->Update(non_render_obj);
		renderer->draw();
	}

	glfwTerminate();

	delete cube;
	delete sphere;

	return 0;

}