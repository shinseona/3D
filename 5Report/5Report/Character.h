#ifndef CHARACTER_H
#define CHARACTER_H

#include "RenderObject.h"
#include "IUpdate.h"

class Character : public RenderObject
{
public : 

	Character(FileManager* fileManager);
	~Character(){}

	virtual void ShutDown();
};


#endif // !CHARACTER_H