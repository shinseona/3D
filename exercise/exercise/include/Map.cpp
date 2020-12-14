#include "Tile.h"
#include "Map.h"
#include "Helper.h"
#include <vector>
#include <iostream>
using std::vector;

Map::Map(int mapWidth, int mapHeight) {
	this->width = mapWidth;
	this->height = mapHeight;

	gridPatency = new int*[mapHeight];
	for (int a = 0; a < mapHeight; ++a) {
		gridPatency[a] = new int[mapWidth];
		for (int b = 0; b < mapWidth; ++b) {
			gridPatency[a][b] = 0;
			tiles.push_back(Tile(a, b));
		}
	}
}
Map::~Map() {
	for (StaticModel* obj: objects) {
		delete obj;
	}
	objects.clear();
	tiles.clear();
	if (gridPatency != NULL) {
		for (int a = 0; a < height; ++a) {
			delete[] gridPatency[a];
		}
		delete[] gridPatency;
	}
}

//getters
int Map::getWidth() { return width; }
int Map::getHeight() { return height; }
vector<StaticModel*>& Map::getModels() {
	return objects;
}
vec3 Map::getTerrainVolume() {
	return terrain->getVolume();
}
vec3 Map::getTerrainCoords() {
	return terrain->getCoords();
}
mat4 Map::getMatrix(string objName) {
	for (StaticModel* model : objects) {
		if (model->getProperty().name.compare(objName) == 0)
			return model->getMatrix();
	}
	return mat4(1.0f);
}
mat4 Map::getTerrainMatrix() {
	return terrain->getMatrix();
}
vector<Tile> Map::getNeighbors(Tile t) {
	int x = t.getX();
	int y = t.getY();
	vector<Tile> result;
	int x1, x2, x3, x4, x5, x6, x7, x8;
	int y1, y2, y3, y4, y5, y6, y7, y8;
	//top left corner
	x1 = x - 1;
	y1 = y + 1;
	//top medium
	x2 = x;
	y2 = y + 1;
	//top right corner
	x3 = x + 1;
	y3 = y + 1;
	//medium left
	x4 = x - 1;
	y4 = y;
	//medium right
	x5 = x + 1;
	y5 = y;
	//bottom left corner
	x6 = x - 1;
	y6 = y - 1;
	//bottom medium
	x7 = x;
	y7 = y - 1;
	//bottom right corber
	x8 = x + 1;
	y8 = y - 1;

	if ((x1 >= 0 && y1 >= 0) && (x1 < width && y1 < height))
		result.push_back(Tile(x1, y1));
	if ((x2 >= 0 && y2 >= 0) && (x2 < width && y2 < height))
		result.push_back(Tile(x2, y2));
	if ((x3 >= 0 && y3 >= 0) && (x3 < width && y3 < height))
		result.push_back(Tile(x3, y3));
	if ((x4 >= 0 && y4 >= 0) && (x4 < width && y4 < height))
		result.push_back(Tile(x4, y4));
	if ((x5 >= 0 && y5 >= 0) && (x5 < width && y5 < height))
		result.push_back(Tile(x5, y5));
	if ((x6 >= 0 && y6 >= 0) && (x6 < width && y6 < height))
		result.push_back(Tile(x6, y6));
	if ((x7 >= 0 && y7 >= 0) && (x7 < width && y7 < height))
		result.push_back(Tile(x7, y7));
	if ((x8 >= 0 && y8 >= 0) && (x8 < width && y8 < height))
		result.push_back(Tile(x8, y8));

	return result;
}
int** Map::getGridPatency() {
	return gridPatency;
}

//other
bool Map::isBlocked(int x, int y) {
	if (x < 0 || x >= width || y < 0 || y >= height) return true;
	return gridPatency[x][y] == 1;
}

void Map::addObject(StaticModel*& model) {
	cout << "ObjectName: " <<model->getProperty().name << endl;
	if (model->getProperty().name.compare("Terrain") == 0)
		terrain = model;
	else {
		objects.push_back(model);
		vec3 coords = model->getCoords();
		vec3 params = model->getVolume();
		Helper helper;
		vector<Tile> tiles = helper.getModelTiles(*model);
		for (Tile t : tiles) {
			gridPatency[t.getX()][t.getY()] = 1;
		}
	}
}

StaticModel*& const Map::getTerrain() { return terrain; }



