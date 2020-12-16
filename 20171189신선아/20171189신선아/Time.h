#pragma once
#include <Windows.h>

class Time
{
public:
	Time();
	LARGE_INTEGER nowFrameCounter;
	LARGE_INTEGER prevFrameCounter;
	LARGE_INTEGER frameInfo;

	double perFrame;
	int count;
	int targetFrame;

	bool Rendering();
};