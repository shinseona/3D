#include "Tile.h"

Tile::Tile(int x, int y) {
	this->x = x;
	this->y = y;
}

Tile::Tile(glm::vec2 coord) {
	this->x = (int)coord.x;
	this->y = (int)coord.y;
}

int Tile::getX() { return x; }
int Tile::getY() { return y; }

bool Tile::operator==(Tile t) {
	return x == t.x && y == t.y;
}