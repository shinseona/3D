#define _CRT_SECURE_NO_WARINGS

#include "Renderer.h"
#include "RenderableObject.h"
#include "NonRenderableObject.h"
#include "star.h"
#include "Human.h"
#include "Time.h"
#include "Ground.h"

int main()
{
	Renderer* renderer = Renderer::instance();
	NonRenderableObject* non_render_obj = new NonRenderableObject();
	
	Time* time = new Time();
	
	Human* human = new Human();
	Star* star = new Star();
	Ground* ground = new Ground();

	renderer->init();

	while (true)
	{
		renderer->Clear();

		renderer->render();

		if (time->Rendering())
		{
			renderer->update();
		}

		glfwSwapBuffers(renderer->window);
		glfwPollEvents();
	}
	renderer->Exit();
	
	star->shutDown();
	human->shutDown();
	ground->shutDown();
	renderer->shutDown();

	delete human;
	delete star;
	delete non_render_obj;
	delete ground;

	return 0;
}