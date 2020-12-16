#include "UserObject.h"
#include "FileManager.h"

UserObject::UserObject(
	FileManager* fileManager,
	const char* objfilePath,
	const char* texturefile_path)
{
	this->fileManager = fileManager;
	this->objfilePath = objfilePath;
	this->texturefile_path = texturefile_path;	

	InitData();
}

void UserObject::InitData()
{
	RenderableObject::InitData();

	fileManager->LoadOBJ(objfilePath, vertices, uvs, normals);

	texture = fileManager->LoadDDS(texturefile_path);

	SetBuffer();

}

void UserObject::ShutDown()
{
	RenderableObject::ShutDown();

}