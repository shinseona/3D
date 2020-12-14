#pragma once
#ifndef TILE_H_
#define TILE_H_
#include <glm/vec2.hpp>

class Tile {
private:
	int x, y;
	Tile() {}//constructor without coords. x and y can't change!
public:
	Tile(int x, int y);
	Tile(glm::vec2 coord);
	int getX();
	int getY();

	bool operator==(Tile t);
};

#endif TILE_H_
