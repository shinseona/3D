#ifndef STAR2_H_
#define STAR2_H_


#include "RenderObject.h"
#include "IUpdate.h"

class Star2 : public RenderObject
{
public:

	Star2(FileManager* fileManager);
	~Star2() {}

	virtual void ShutDown();

};




#endif // !STAR2_H_

