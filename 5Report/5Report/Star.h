#ifndef STAR_H_
#define STAR_H_


#include "RenderObject.h"
#include "IUpdate.h"

class Star : public RenderObject
{
public:

	Star(FileManager* fileManager);
	~Star() {}

	virtual void ShutDown();

};




#endif // !STAR_H_

