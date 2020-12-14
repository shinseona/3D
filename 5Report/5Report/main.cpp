#include <stdio.h>
#include <algorithm>
#include <stdlib.h>

#include "FileManager.h"
#include "Renderer.h"
#include "Star.h"
#include "Star2.h"
#include "UserObject.h"
#include "NonRenderObject.h"
#include "Character.h"
#include "controls.h"

int main()
{
	FileManager* filemgr = FileManager::instance();
	Renderer* renderer = Renderer::instance();

	renderer->Init();

	UserObject* cube = new UserObject(
		filemgr,
		"cube.obj",
		"20171189_vs.shader", "20171189_fs.shader",
		"uvtemplate.dds");

	cube->SetPosition(glm::vec3(20.0f, 8.0f, 0.0f));
	cube->SetScale(glm::vec3(3.0f, 3.0f, 3.0f));
	cube->SetRotation(180.0f, glm::vec3(0.0f, 1.0f, 0.0f));

	UserObject* sphere = new UserObject(
		filemgr,
		"sphere.obj",
		"20171189_vs.shader", "20171189_fs.shader",
		"Sun.dds");

	sphere->SetPosition(glm::vec3(0.0f, 10.0f, 0.0f));
	sphere->SetScale(glm::vec3(10.0f, 10.0f, 10.0f));
	sphere->SetRotation(180.0f, glm::vec3(0.0f, 1.0f, 0.0f));

	//green
		Character* star1 = new Character(filemgr);
		star1->SetPosition(glm::vec3(0.0f, -10.0f, 30.0f));
		star1->SetScale(glm::vec3(0.5f, 0.5f, 0.5f));

		Character* star2 = new Character(filemgr);
		star2->SetPosition(glm::vec3(0.0f, 0.0f, 0.0f));
		star2->SetScale(glm::vec3(0.5f, 0.5f, 0.5f));

		Character* star3 = new Character(filemgr);
		star3->SetPosition(glm::vec3(-30.0f, 0.0f, 0.0f));
		star3->SetScale(glm::vec3(0.5f, 0.5f, 0.5f));

		Character* star4 = new Character(filemgr);
		star4->SetPosition(glm::vec3(30.0f, 0.0f, 0.0f));
		star4->SetScale(glm::vec3(0.5f, 0.5f, 0.5f));

		Character* star5 = new Character(filemgr);
		star5->SetPosition(glm::vec3(0.0f, 0.0f, -30.0f));
		star5->SetScale(glm::vec3(0.5f, 0.5f, 0.5f));
	
	//red
	
		Star* star6 = new Star(filemgr);
		star6->SetPosition(glm::vec3(0.0f, 30.0f, 30.0f));
		star6->SetScale(glm::vec3(0.5f, 0.5f, 0.5f));

		Star* star7 = new Star(filemgr);
		star7->SetPosition(glm::vec3(0.0f, 30.0f, 0.0f));
		star7->SetScale(glm::vec3(0.5f, 0.5f, 0.5f));

		Star* star8 = new Star(filemgr);
		star8->SetPosition(glm::vec3(-30.0f, 30.0f, 0.0f));
		star8->SetScale(glm::vec3(0.5f, 0.5f, 0.5f));

		Star* star9 = new Star(filemgr);
		star9->SetPosition(glm::vec3(30.0f, 30.0f, 0.0f));
		star9->SetScale(glm::vec3(0.5f, 0.5f, 0.5f));

		Star* star10 = new Star(filemgr);
		star10->SetPosition(glm::vec3(0.0f, 30.0f, -30.0f));
		star10->SetScale(glm::vec3(0.5f, 0.5f, 0.5f));
	
	//blue
		Star2* star11 = new Star2(filemgr);
		star11->SetPosition(glm::vec3(25.0f, 15.0f, 30.0f));
		star11->SetScale(glm::vec3(0.5f, 0.5f, 0.5f));

		Star2* star12 = new Star2(filemgr);
		star12->SetPosition(glm::vec3(-25.0f, 15.0f, -30.0f));
		star12->SetScale(glm::vec3(0.5f, 0.5f, 0.5f));

		Star2* star13 = new Star2(filemgr);
		star13->SetPosition(glm::vec3(-30.0f, 15.0f, 25.0f));
		star13->SetScale(glm::vec3(0.5f, 0.5f, 0.5f));

		Star2* star14 = new Star2(filemgr);
		star14->SetPosition(glm::vec3(30.0f, 15.0f, -25.0f));
		star14->SetScale(glm::vec3(0.5f, 0.5f, 0.5f));


	NonRenderObject* non_render_obj = new NonRenderObject();

	do
	{
		computeMatricesFromInputs();
		renderer->SetViewMatrix(getViewMatrix());
		renderer->SetProjectionMatrix(getProjectionMatrix());
		
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		renderer->Rotationdraw(cube);
		renderer->Rotationdraw(sphere);

		renderer->draw(star1);
		renderer->draw(star2);
		renderer->draw(star3);
		renderer->draw(star4);
		renderer->draw(star5);

		renderer->draw(star6);
		renderer->draw(star7);
		renderer->draw(star8);
		renderer->draw(star9);
		renderer->draw(star10);

		renderer->draw(star11);
		renderer->draw(star12);
		renderer->draw(star13);
		renderer->draw(star14);

		renderer->Update(non_render_obj);
		glfwSwapBuffers(renderer->GetWindow());
		glfwPollEvents();
	

	} while (glfwGetKey(renderer->GetWindow(), GLFW_KEY_ESCAPE) != GLFW_PRESS &&
		glfwWindowShouldClose(renderer->GetWindow()) == 0);

	delete cube;
	delete sphere;
	delete star1;
	delete star2;
	delete star3;
	delete star4;
	delete star5;
	delete star6;
	delete star7;
	delete star8;
	delete star9;
	delete star10;
	delete star11;
	delete star12;
	delete star13;
	delete star14;
	glfwTerminate();



	return 0;

}