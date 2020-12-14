#ifndef TEST_H
#define TEST_H
#include "Renderer.h"

class Test:public Renderer
{
public:
	void Init();
	void ShutDown();
	void Update();
	void Render();

};

#endif // !TEST_H
