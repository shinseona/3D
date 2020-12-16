#include "UserObject.h"
#include "FileManager.h"

UserObject::UserObject(
	FileManager* fileManager,
	const char* objfilePath,
	const char* vertexfile_path,
	const char* fragmentfile_path,
	const char* texturefile_path)
{
	textureID = glGetUniformLocation(programID, "myTextureSampler");

	fileManager->LoadOBJ(objfilePath, vertices, uvs, normals);

	programID = fileManager->LoadShaders(vertexfile_path, fragmentfile_path);

	texture = fileManager->LoadDDS(texturefile_path);

	SetBuffer();
}

void UserObject::ShutDown()
{

}