#ifndef __OBJECT_H__
#define __OBJECT_H__

#include "ICleanUp.h"

#include <vector>
#include<string>

#include "include/GL/glew.h"		
#include "glm/gtc/matrix_transform.hpp"

class Object : public ICleanUp
{
public : 
	Object();
	virtual ~Object() {}

	virtual void ShutDown() override;

	glm::mat4 GetTranslate() const { return translate; }
	void SetPosition(glm::vec3 src_position);
	

private:
	glm::mat4 translate;
};


#endif // !__OBJECT_H__