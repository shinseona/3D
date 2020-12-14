#include "UserObject.h"
#include "FileManager.h"

UserObject::UserObject(
	FileManager* fileManager,
	const char* objfile,
	const char* vertexfile,
	const char* fragmentfile,
	const char* texturefile)
{
	textureID = glGetUniformLocation(programID, "myTextureSampler");

	fileManager->LoadOBJ(objfile, vertices, uvs, normals);

	programID = fileManager->LoadShaders(vertexfile, fragmentfile);

	texture = fileManager->LoadDDS(texturefile);

	SetBuffer();
}


void UserObject::ShutDown()
{

}