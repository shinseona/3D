#include <iostream>

#include "Star.h"
#include "FileManager.h"

#include "include/GLFW/glfw3.h" 


Star::Star(FileManager* fileManager)
{
	textureID = glGetUniformLocation(programID, "myTextureSampler");

	fileManager->LoadOBJ("star.obj", vertices, uvs, normals);

	programID = fileManager->LoadShaders("star_vs.shader", "redStar.shader");

	SetBuffer();
}

void Star::ShutDown()
{
	RenderObject::ShutDown();
}

