#include "Text.h"

Text::Text(){}
Text::Text(string text, string fontType, double screenX, double screenY, double fontSizeX, double fontSizeY, double SCR_WIDTH, double SCR_HEIGHT) {
	init(text, screenX, screenY, SCR_WIDTH, SCR_HEIGHT, fontSizeX, fontSizeY);
	model = new SimpleModel(vertices, indices, "fonts/" + fontType + "Font.png");
}
Text::Text(string text, SimpleModel* model, double screenX, double screenY, double fontSizeX, double fontSizeY, double SCR_WIDTH, double SCR_HEIGHT) {
	init(text, screenX, screenY, SCR_WIDTH, SCR_HEIGHT, fontSizeX, fontSizeY);
	model->updateIndices(indices);
	model->updateVertices(vertices);
	this->model = model;
}
Text::~Text() {
	if(model!=NULL)
		delete model;
	vertices.clear();
	indices.clear();
}


void Text::init(string text, double screenX, double screenY, double SCR_WIDTH, double SCR_HEIGHT, double fontSizeX, double fontSizeY) {
	//save original screen X and Y
	originScreenX = screenX;
	originScreenY = screenY;
	//font size in window space: [0-winWidth][0-winHeight], ex: 800x600
	this->fontSizeX = fontSizeX;// 20.0;
	this->fontSizeY = fontSizeY;// 23.0;
								//font size in GLFW window space: [0,-1][0,-1]
	screenFontSizeX = ((fontSizeX) / (SCR_WIDTH / 2.0f) - 1.0f) + 1;//end-start
	screenFontSizeY = ((fontSizeY) / (SCR_HEIGHT / 2.0f) - 1.0f) + 1;//end-start
																		//text origin in GLFW window space: [0,-1][0,-1]
	textOriginX = screenX / (SCR_WIDTH / 2.0f) - 1.0f;
	textOriginY = screenY / (SCR_HEIGHT / 2.0f) - 1.0f;
	//letter size in texture space: [0-1][0-1]
	textureLetterSizeX = 23.0 / 512.0;//letter size / texture width
	textureLetterSizeY = 32.0 / 256.0;//letter size / texture height
										// coordinate of current letter in GLFW window space: [0,-1][0,-1]
	textCurrentX = textOriginX;
	textCurrentY = textOriginY;

	Vertex vert1, vert2, vert3, vert4;
	for (char c : text) {
		if (c == '\n') {
			screenY -= fontSizeY;
			textCurrentY = screenY / (SCR_HEIGHT / 2.0f) - 1.0f;
			screenX = originScreenX;
			textCurrentX = screenX / (SCR_WIDTH / 2.0f) - 1.0f;
			continue;
		}
		int charCode = (int)c;

		//calculate vertex positions
		vec3 min = vec3(textCurrentX, textCurrentY, 0.0f);
		vec3 max = vec3(textCurrentX + screenFontSizeX, textCurrentY + screenFontSizeY, 0.0f);
		vert1.Position = vec3(min.x, min.y, min.z);
		vert2.Position = vec3(max.x, min.y, min.z);
		vert3.Position = vec3(max.x, max.y, min.z);
		vert4.Position = vec3(min.x, max.y, min.z);

		//calculate texture positions
		/*int line = (charCode - 32.0) / 16.0;
		int col = (charCode - (32.0 + line * 16.0));
		double minCoordX = col*32.0 / 512.0;
		double minCoordY = line*32.0 / 256.0;*/

		int line = (charCode - 32.0) / 22.0;
		int col = (charCode - (32.0 + line * 22.0));
		double minCoordX = col*23.0 / 512.0;
		double minCoordY = line*32.0 / 256.0;

		vert4.TexCoords = vec2(minCoordX, minCoordY);
		vert3.TexCoords = vec2(minCoordX + textureLetterSizeX, minCoordY);
		vert2.TexCoords = vec2(minCoordX + textureLetterSizeX, minCoordY + textureLetterSizeY);
		vert1.TexCoords = vec2(minCoordX, minCoordY + textureLetterSizeY);

		//add indices info
		int nrVertices = vertices.size();
		indices.push_back(nrVertices + 0);
		indices.push_back(nrVertices + 1);
		indices.push_back(nrVertices + 2);
		indices.push_back(nrVertices + 2);
		indices.push_back(nrVertices + 3);
		indices.push_back(nrVertices + 0);

		//add vertices info
		vertices.push_back(vert1);
		vertices.push_back(vert2);
		vertices.push_back(vert3);
		vertices.push_back(vert4);


		//iterate word to draw next letter
		screenX += fontSizeX*0.65;
		textCurrentX = screenX / (SCR_WIDTH / 2.0f) - 1.0f;
	}
}
void Text::update(string text, double SCR_WIDTH, double SCR_HEIGHT) {
	indices.clear();
	vertices.clear();
	double screenX = originScreenX;
	double screenY = originScreenY;
	textCurrentX = textOriginX;
	textCurrentY = textOriginY;

	Vertex vert1, vert2, vert3, vert4;
	for (char c : text) {
		if (c == '\n') {
			screenY -= fontSizeY;
			textCurrentY = screenY / (SCR_HEIGHT / 2.0f) - 1.0f;
			screenX = originScreenX;
			textCurrentX = screenX / (SCR_WIDTH / 2.0f) - 1.0f;
			continue;
		}
		int charCode = (int)c;

		//calculate vertex positions
		vec3 min = vec3(textCurrentX, textCurrentY, 0.0f);
		vec3 max = vec3(textCurrentX + screenFontSizeX, textCurrentY + screenFontSizeY, 0.0f);
		vert1.Position = vec3(min.x, min.y, min.z);
		vert2.Position = vec3(max.x, min.y, min.z);
		vert3.Position = vec3(max.x, max.y, min.z);
		vert4.Position = vec3(min.x, max.y, min.z);

		//calculate texture positions

		int line = (charCode - 32.0) / 22.0;
		int col = (charCode - (32.0 + line * 22.0));
		double minCoordX = col*23.0 / 512.0;
		double minCoordY = line*32.0 / 256.0;

		vert4.TexCoords = vec2(minCoordX, minCoordY);
		vert3.TexCoords = vec2(minCoordX + textureLetterSizeX, minCoordY);
		vert2.TexCoords = vec2(minCoordX + textureLetterSizeX, minCoordY + textureLetterSizeY);
		vert1.TexCoords = vec2(minCoordX, minCoordY + textureLetterSizeY);

		//add indices info
		int nrVertices = vertices.size();
		indices.push_back(nrVertices + 0);
		indices.push_back(nrVertices + 1);
		indices.push_back(nrVertices + 2);
		indices.push_back(nrVertices + 2);
		indices.push_back(nrVertices + 3);
		indices.push_back(nrVertices + 0);

		//add vertices info
		vertices.push_back(vert1);
		vertices.push_back(vert2);
		vertices.push_back(vert3);
		vertices.push_back(vert4);


		//iterate word to draw next letter
		screenX += fontSizeX*0.6;
		textCurrentX = screenX / (SCR_WIDTH / 2.0f) - 1.0f;
	}
}
SimpleModel* Text::getModel() {
	model->updateIndices(indices);
	model->updateVertices(vertices);
	return model;
}
void Text::updateText(string text, double SCR_WIDTH, double SCR_HEIGHT) {
	//	update(text, SCR_WIDTH, SCR_HEIGHT);
	vertices.clear();
	indices.clear();
	init(text, originScreenX, originScreenY, SCR_WIDTH, SCR_HEIGHT, fontSizeX, fontSizeY);
	model->updateIndices(indices);
	model->updateVertices(vertices);
}
