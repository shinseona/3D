#include "ProgressBar.h"


ProgressBar::ProgressBar(double x, double y, double width, double height, double scrWidth, double scrHeight, double maxValue) {

	this->scrWidth = scrWidth;
	this->scrHeight = scrHeight;
	this->x = ((x) / (scrWidth / 2.0f) - 1.0f);
	this->y = ((y) / (scrHeight / 2.0f) - 1.0f);
	this->maxWidth = this->currWidth = (((x + width) / (scrWidth / 2.0f) - 1.0f)) - this->x;
	this->height = (((y + height) / (scrHeight / 2.0f) - 1.0f)) - this->y;
	this->maxValue = currValue = maxValue;
	init();
	currProcent = 1.0;
}
ProgressBar::~ProgressBar() {
	delete model;
}


void ProgressBar::init() {
	vec3 min = vec3(x, y, 0.0f);
	vec3 max = vec3(x + currWidth, y + height, 0.0f);

	Vertex vert1, vert2, vert3, vert4;
	vector<Vertex> vertices;
	vector<unsigned int> indices;

	vert1.Position = vec3(min.x, max.y, min.z);
	vert2.Position = vec3(max.x, max.y, min.z);
	vert3.Position = vec3(max.x, min.y, min.z);
	vert4.Position = vec3(min.x, min.y, min.z);
	vert1.TexCoords = vec2(0.0f, 0.0f);
	vert2.TexCoords = vec2(1.0f, 0.0f);
	vert3.TexCoords = vec2(1.0f, 1.0f);
	vert4.TexCoords = vec2(0.0f, 1.0f);
	vertices.push_back(vert1);
	vertices.push_back(vert2);
	vertices.push_back(vert3);
	vertices.push_back(vert4);

	indices.push_back(0);
	indices.push_back(1);
	indices.push_back(2);
	indices.push_back(2);
	indices.push_back(3);
	indices.push_back(0);

	model = new SimpleModel(vertices, indices);
	checked = true;
}
void ProgressBar::update() {
	currProcent = currValue / maxValue;
	currWidth = maxWidth * currProcent;
	init();
}
SimpleModel* ProgressBar::getModel() { return model; }
void ProgressBar::add(double value) {
	double newValue = currValue + value;
	if (newValue > maxValue)
		currValue = maxValue;
	else if (newValue < 0)
		currValue = 0;
	else
		currValue = newValue;
	update();
}

