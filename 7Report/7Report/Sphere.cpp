#include "Sphere.h"
#include "FileManager.h"
#include "IRenderer.h"

Sphere::Sphere(FileManager* filemgr)
{
	fileManager = filemgr;

	InitData();
}

void Sphere::InitData()
{
	RenderableObject::InitData();

	fileManager->LoadOBJ("sphere.obj", vertices, uvs, normals);

	texture = fileManager->LoadDDS("moon.dds");

	SetBuffer();
}

void Sphere::ShutDown()
{
	RenderableObject::ShutDown();

	//Sphere ������ ������ �Ҹ�ó��
	//Ư���� �Ҹ�ó�� �� ���� ���ٰ� �ص� ������
}