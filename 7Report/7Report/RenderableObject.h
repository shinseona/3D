#ifndef __RENDERABLEOBJECT_H__
#define __RENDERABLEOBJECT_H__

#include "Object.h"
#include "IRenderer.h"

class Object;
class FileManager;

class RenderableObject : public Object, public IRenderer
{
public:
	RenderableObject();
	~RenderableObject(){}

	void SetBuffer();

	virtual void ShutDown() override;

	virtual glm::mat4  GetTranslate() override { return Object::GetTranslate();  }

	virtual void InitData() override;

	FileManager* fileManager;

	const char* objfilePath;
	const char* texturefile_path;
	
};


#endif // !__OBJECT_H__#pragma once
