#include "Sphere.h"
#include "FileManager.h"

Sphere::Sphere(FileManager* fileManager)
{
	textureID = glGetUniformLocation(programID, "myTextureSampler");

	fileManager->LoadOBJ("sphere.obj", vertices, uvs, normals);

	programID = fileManager->LoadShaders("vs_w7.shader", "fs_w7.shader");

	texture = fileManager->LoadDDS("moon.dds");

	SetBuffer();
}

void Sphere::ShutDown()
{
	RenderableObject::ShutDown();

	//Sphere 렌더링 데이터 소멸처리
	//특별히 소멸처리 할 일이 없다고 해도 재정의
}