#pragma once
#ifndef MAP_H_
#define MAP_H_
#include "Tile.h"
#include <vector>
#include "StaticModel.h"
#include <glm/mat4x4.hpp>
using std::vector;

class Map {
private:
	StaticModel* terrain;
	vector<StaticModel*> objects;
	vector<Tile> tiles;
	int width, height;
	int** gridPatency;
public:
	Map(int width, int height);
	~Map();

	//getters
	vector<Tile> getNeighbors(Tile t);
	int getWidth();
	int getHeight();
	mat4 getMatrix(string objName);
	mat4 getTerrainMatrix();
	vector<StaticModel*>& getModels();
	vec3 getTerrainVolume();
	vec3 getTerrainCoords();
	int** getGridPatency();
	StaticModel*& const getTerrain();

	//other
	void addObject(StaticModel*& model);
	bool isBlocked(int x, int y);

};

#endif MAP_H_