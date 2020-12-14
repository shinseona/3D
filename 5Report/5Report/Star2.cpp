#include <iostream>

#include "Star2.h"
#include "FileManager.h"


#include "include/GLFW/glfw3.h" 


Star2::Star2(FileManager* fileManager)
{
	textureID = glGetUniformLocation(programID, "myTextureSampler");

	fileManager->LoadOBJ("star.obj", vertices, uvs, normals);

	programID = fileManager->LoadShaders("star_vs.shader", "blueStar.shader");

	SetBuffer();
}

void Star2::ShutDown()
{
	RenderObject::ShutDown();
}

