#pragma once

#include <vector>

#include "ICleanUp.h"
#include "IUpdater.h"
#include "IInit.h"
#include "IRender.h"

#include "glm/gtc/matrix_transform.hpp"
#include "glm/glm.hpp"
#include "include/GL/glew.h"		
#include "include/GLFW/glfw3.h" 

class Object: public ICleanUp, public IUpdater, public IInit, public IRender
{
public:
	virtual ~Object() {}
};