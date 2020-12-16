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
	PlaySound(TEXT("bgm.wav"), NULL, SND_ASYNC | SND_FILENAME | SND_LOOP);
	Renderer* renderer = Renderer::instance();
	NonRenderableObject* non_render_obj = new NonRenderableObject();
	
	Time* time = new Time();
	Ground* ground = new Ground();
	Human* human = new Human();
	
	Star* star = new Star();
	BlueStar* blue_star = new BlueStar();
	RedStar* red_star = new RedStar();
	
	Star* star1 = new Star();
	BlueStar* blue_star1 = new BlueStar();
	RedStar* red_star1 = new RedStar();

	Star* star2 = new Star();
	BlueStar* blue_star2 = new BlueStar();
	RedStar* red_star2 = new RedStar();

	star->setPosition(0,0,0);
	blue_star->setPosition(0, 0, 10);
	red_star->setPosition(10, 0, 0);

	star1->setPosition(20, 0, 0);
	blue_star1->setPosition(10, 0, 10);
	red_star1->setPosition(0, 0, 20);
	
	star2->setPosition(-10, 0, 0);
	blue_star2->setPosition(-5, 0, -5);
	red_star2->setPosition(0, 0, -10);

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
	blue_star->shutDown();
	red_star->shutDown();
	star1->shutDown();
	blue_star1->shutDown();
	red_star1->shutDown();
	star2->shutDown();
	blue_star2->shutDown();
	red_star2->shutDown();
	human->shutDown();
	ground->shutDown();
	renderer->shutDown();

	delete human;
	delete star;
	delete blue_star;
	delete red_star;
	delete star1;
	delete blue_star1;
	delete red_star1;
	delete star2;
	delete blue_star2;
	delete red_star2;
	delete non_render_obj;
	delete ground;

	return 0;
}