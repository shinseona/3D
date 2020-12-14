#ifndef __USEROBJECT_H__
#define __USEROBJECT_H__

#include "RenderableObject.h"

class UserObject : public RenderableObject
{
public : 
	UserObject(
		FileManager* fileManager, 
		const char* objfilePath,
		const char* vertexfile_path,
		const char* fragmentfile_path,
		const char* texturefile_path);

	~UserObject(){}

	virtual void ShutDown() override;

};


#endif // !__USEROBJECT_H__