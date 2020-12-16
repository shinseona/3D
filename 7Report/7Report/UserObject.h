#ifndef __USEROBJECT_H__
#define __USEROBJECT_H__

#include "RenderableObject.h"

class UserObject : public RenderableObject
{
public : 
	UserObject(
		FileManager* fileManager, 
		const char* objfilePath,
		const char* texturefile_path);

	~UserObject(){}

	virtual void ShutDown() override;

	virtual void InitData() override;

};


#endif // !__USEROBJECT_H__