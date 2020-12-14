#include <map>
#include <list>
#include "TextManager.h"

TextManager::~TextManager() {
	map<string, Text*> texts;
	map<string, bool> drawText;
	map<string, SimpleModel*> fonts;

	for (pair<string, Text*> text : texts)
		delete text.second;
	texts.clear();
	for (pair<string, SimpleModel*> font : fonts)
		delete font.second;
	fonts.clear();
	drawText.clear();
}

void TextManager::addText(string name, Text* text) {
	texts.insert(make_pair(name, text));
	drawText.insert(make_pair(name, true));
}
void TextManager::addText(string name, string text, string fontType, double screenX, double screenY, double fontSizeX, double fontSizeY, double SCR_WIDTH, double SCR_HEIGHT) {
	for (pair<string, SimpleModel*> font : fonts) {
		if (font.first.compare(fontType) == 0) {
			addText(name, new Text(text, font.second, screenX, screenY, fontSizeX, fontSizeY, SCR_WIDTH, SCR_HEIGHT));
			drawText.insert(make_pair(name, true));
			break;
		}
	}
}
void TextManager::addText(string name, string text, SimpleModel* model, double screenX, double screenY, double fontSizeX, double fontSizeY, double SCR_WIDTH, double SCR_HEIGHT) {
	addText(name, new Text(text, model, screenX, screenY, fontSizeX, fontSizeY, SCR_WIDTH, SCR_HEIGHT));
	drawText.insert(make_pair(name, true));
}
void TextManager::addFont(string type, SimpleModel* model) {
	fonts.insert(make_pair(type, model));
}
void TextManager::updateText(string textName, string text, double scrWidth, double scrHeight) {
	if (texts.find(textName) != texts.end()) {
		texts.at(textName)->updateText(text, scrWidth, scrHeight);
	}
}
void TextManager::removeText(string name) {
	if (texts.find(name) != texts.end()) {
		texts.erase(name);
	}
}
const map<string, Text*>& TextManager::getTexts() {
	return texts;
}

void TextManager::blockDraw(string textName) {
	drawText.at(textName) = false;
}
void TextManager::allowDraw(string textName) {
	drawText.at(textName) = true;
}
bool TextManager::canDraw(string textName) {
	return drawText.at(textName);
}
