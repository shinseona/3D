#include "Time.h"
#include <iostream>

Time::Time()
{
	targetFrame = 60;

	QueryPerformanceFrequency(&frameInfo);
	QueryPerformanceCounter(&prevFrameCounter);

	perFrame = frameInfo.QuadPart / (double)targetFrame;
	
}

bool Time::Rendering()
{
	QueryPerformanceCounter(&nowFrameCounter);

	double time_distance = nowFrameCounter.QuadPart - prevFrameCounter.QuadPart;

	if (time_distance > perFrame)
	{
		prevFrameCounter = nowFrameCounter;

		static int count = 0;
		if (count++ > targetFrame)
		{
			std::cout << "1" << std::endl;

			count = 0;
		}

		return true;
	}

	return false;
}