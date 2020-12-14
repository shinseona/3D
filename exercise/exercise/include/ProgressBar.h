#pragma once
#ifndef PROGRESSBAR_H_
#define PROGRESSBAR_H_
#include "SimpleModel.h"

class ProgressBar {
private:
	double currProcent;
	double maxValue;
	double currValue;
	double x, y;
	double maxWidth, currWidth;
	double scrWidth,scrHeight;
	double height;
	SimpleModel* model;
	bool checked;

	void init();
	void update();
public:
	ProgressBar(double x, double y,
		double width, double height, 
		double scrWidth, double scrHeight, double maxValue);
	~ProgressBar();

	SimpleModel* getModel();
	void add(double value);
};

#endif