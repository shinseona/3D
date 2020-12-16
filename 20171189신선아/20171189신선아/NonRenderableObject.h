#pragma once

#include "Object.h"

class NonRenderableObject : public Object
{
public:
	NonRenderableObject();

	virtual void init();
	virtual void render() override;
	virtual void update() override;
	virtual void shutDown() override;
};