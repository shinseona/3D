#ifndef NONRENDEROBJECT_H
#define NONRENDEROBJECT_H

#include "Object.h"
#include "IUpdate.h"

class NonRenderObject : public Object,public IUpdate
{
public : 
	virtual void Update(IUpdate* iupdate)override;
	virtual void ShutDown() override {}
};

#endif // !NONRENDEROBJECT_H
