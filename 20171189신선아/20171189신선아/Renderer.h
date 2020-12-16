#pragma once

#include <vector>

#include "FileManager.h"
#include "ICleanUp.h"
#include "IUpdater.h"

#include "include/GL/glew.h"		
#include "include/GLFW/glfw3.h" 

class RenderableObject;
class NonRenderableObject;

class Renderer: public ICleanUp
{
private:
	std::vector<RenderableObject*> renderList;
	std::vector<NonRenderableObject*> nonrenderList;

public:
	static Renderer* instance()
	{
		static Renderer instance;

		return &instance;
	}

public:
	GLFWwindow* window;
	GLFWwindow* GetWindow() const { return window; }
	void init();
	void render();
	void update();
	virtual void shutDown() override;
	
	void addRenderableObject(RenderableObject* render_obj);
	void addNonRenderableObject(NonRenderableObject* nonrender_obj);

	void Clear();
	void Exit();
};