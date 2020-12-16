#ifndef __SPHERE_H__
#define __SPHERE_H__

#include "RenderableObject.h"

class Sphere : public RenderableObject
{
public : 
	Sphere(FileManager* fileManager);
	~Sphere() {}
	
	virtual void InitData() override;

	virtual void ShutDown() override;

};
#endif // !__SPHERE_H__