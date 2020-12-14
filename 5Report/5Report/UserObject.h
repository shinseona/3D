#ifndef USEROBJECT_H
#define USEROBJECT_H

#include "RenderObject.h"

class UserObject : public RenderObject
{
public : 
	UserObject(
		FileManager* fileManager, 
		const char* objfile,
		const char* vertexfile,
		const char* fragmentfile,
		const char* texturefile);

	~UserObject(){}

	virtual void ShutDown() override;

};


#endif // !USEROBJECT_H