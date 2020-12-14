#pragma once
#ifndef TEXT_H_
#define TEXT_H_
#include "SimpleModel.h"
#include "mesh.h"
#include <vector>
using std::vector;

class Text {
private:
	double fontSizeX, fontSizeY;
	double screenFontSizeX, screenFontSizeY;
	double textOriginX, textOriginY;
	double originScreenX, originScreenY;
	double textureLetterSizeX, textureLetterSizeY;
	double textCurrentX, textCurrentY;

	SimpleModel* model;
	vector<Vertex> vertices;
	vector<unsigned int> indices;

	Text();

	void init(string text, double screenX, double screenY,
		double SCR_WIDTH, double SCR_HEIGHT,
		double fontSizeX, double fontSizeY);
	void update(string text, double SCR_WIDTH, double SCR_HEIGHT);
public:
	Text(string text, string fontType,
		double screenX, double screenY,
		double fontSizeX, double fontSizeY,
		double SCR_WIDTH, double SCR_HEIGHT);
	Text(string text, SimpleModel* model,
		double screenX, double screenY,
		double fontSizeX, double fontSizeY,
		double SCR_WIDTH, double SCR_HEIGHT);
	~Text();

	SimpleModel* getModel();
	void updateText(string text, double SCR_WIDTH, double SCR_HEIGHT);
};

#endif 
