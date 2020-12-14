#include <iostream>

#include "Character.h"
#include "FileManager.h"

#include "include/GLFW/glfw3.h" 


Character::Character(FileManager* fileManager)
{

	textureID = glGetUniformLocation(programID, "myTextureSampler");

	fileManager->LoadOBJ("star.obj", vertices, uvs, normals);

	programID = fileManager->LoadShaders("star_vs.shader", "greemStar.shader");

	SetBuffer();
}

void Character::ShutDown()
{
	RenderObject::ShutDown();
}

