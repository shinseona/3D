#pragma once
#ifndef ALGASTARPATHFINDER_H_
#define ALGASTARPATHFINDER_H_
#include "Tile.h"
#include <glm/vec2.hpp>
#include <vector>
using glm::vec2;
using std::vector;

//structs
struct Node {
	int x, y;
	Node(int x, int y) { this->x = x; this->y = y; }
	Node(const Node& n) { this->x = n.x; this->y = n.y; }
	bool operator==(const Node& n) {
		return this->x == n.x && this->y == n.y;
	}
	int fValue;
	Node* parent;
};

class AlgAStarPathfinder {
private:
	//data
	vector<Tile> currPath;
	bool found;

	//functions
	int getDistanceBetween(Node node1, Node node2);
	int f(Node start, Node curr, Node goal);
	vector<Node> getNeighbors(Node node, int** map, int rows, int cols);
	void reconstructPath(int** cameFrom, Node curr, Node start, int rows, int cols);
	void findPath(int** map, int rows, int cols, Node start, Node goal);
public:
	AlgAStarPathfinder();
	~AlgAStarPathfinder();
	vector<Tile> searchPath(int** map, int rows, int cols, Tile start, Tile goal);
};

#endif ALGASTARPATHFINDER_H_
