#pragma once
#include "RenderableObject.h"

class Star : public RenderableObject
{
public:
	virtual void init() override;
	virtual void render() override;
	virtual void update() override;
	virtual void shutDown() override;
	virtual void setPosition(float x, float y, float z) override;
	virtual void setScale(float x, float y, float z) override;
};