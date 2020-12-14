#pragma once
#ifndef TEXTMANAGER_H_
#define TEXTMANAGER_H_

#include <map>
#include <list>
#include "Text.h"
using std::map;
using std::list;

class TextManager {
private:
	map<string, Text*> texts;
	map<string, bool> drawText;
	map<string, SimpleModel*> fonts;
public:
	~TextManager();

	void addText(string name, Text* text);
	void addText(string name, string text, string fontType, 
		double screenX, double screenY, 
		double fontSizeX, double fontSizeY, 
		double SCR_WIDTH, double SCR_HEIGHT);
	void addText(string name, string text, SimpleModel* model, 
		double screenX, double screenY, 
		double fontSizeX, double fontSizeY, 
		double SCR_WIDTH, double SCR_HEIGHT);

	void addFont(string type, SimpleModel* model);
	void updateText(string textName, string text, double scrWidth, double scrHeight);

	void removeText(string name);

	const map<string, Text*>& getTexts();

	void blockDraw(string textName);
	void allowDraw(string textName);
	bool canDraw(string textName);
};

#endif 
