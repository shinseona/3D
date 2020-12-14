#include <iostream>
#include "AlgAStarPathfinder.h"
#include <glm\vec2.hpp>
#include <vector>
#include "Tile.h"
#include "Map.h"
#include <climits>
using glm::vec2;
using std::vector;
using std::cout;
using std::endl;

AlgAStarPathfinder::AlgAStarPathfinder() {}
AlgAStarPathfinder::~AlgAStarPathfinder() {
	currPath.clear();
}

//functions
int AlgAStarPathfinder::getDistanceBetween(Node node1, Node node2) {
	int val1 = abs(node1.x - node2.x);
	int val2 = abs(node1.y - node2.y);
	return val1>val2 ? val1 : val2;
}

int AlgAStarPathfinder::f(Node start, Node curr, Node goal) {
	int h = getDistanceBetween(start, curr);
	int g = getDistanceBetween(curr, goal);
	return h + g;
}

vector<Node> AlgAStarPathfinder::getNeighbors(Node node, int** map, int rows, int cols) {
	vector<Node> neighbors;
	int x = node.x, y = node.y;
	/*
	x-1,y-1|x-1,y|x-1,y+1
	--------------
	x,y-1  |x,y  | x,y+1
	--------------
	x+1,y-1|x+1,y|x+1,y+1
	*/

	int x1 = x - 1, y1 = y - 1;
	int x2 = x - 1, y2 = y;
	int x3 = x - 1, y3 = y + 1;
	int x4 = x, y4 = y - 1;
	int x5 = x, y5 = y + 1;
	int x6 = x + 1, y6 = y - 1;
	int x7 = x + 1, y7 = y;
	int x8 = x + 1, y8 = y + 1;
	if ((x1 >= 0 && x1 < rows) && (y1 >= 0 && y1 < cols) && map[x1][y1] == 0) {
		Node newNeighbor(x1, y1);
		newNeighbor.parent = new Node(node);
		neighbors.push_back(Node(x1, y1));

	}
	if ((x2 >= 0 && x2 < rows) && (y2 >= 0 && y2 < cols) && map[x2][y2] == 0) {
		Node newNeighbor(x2, y2);
		newNeighbor.parent = new Node(node);
		neighbors.push_back(Node(x2, y2));

	}
	if ((x3 >= 0 && x3 < rows) && (y3 >= 0 && y3 < cols) && map[x3][y3] == 0) {
		Node newNeighbor(x3, y3);
		newNeighbor.parent = new Node(node);
		neighbors.push_back(Node(x3, y3));

	}
	if ((x4 >= 0 && x4 < rows) && (y4 >= 0 && y4 < cols) && map[x4][y4] == 0) {
		Node newNeighbor(x4, y4);
		newNeighbor.parent = new Node(node);
		neighbors.push_back(Node(x4, y4));

	}
	if ((x5 >= 0 && x5 < rows) && (y5 >= 0 && y5 < cols) && map[x5][y5] == 0) {
		Node newNeighbor(x5, y5);
		newNeighbor.parent = new Node(node);
		neighbors.push_back(Node(x5, y5));

	}
	if ((x6 >= 0 && x6 < rows) && (y6 >= 0 && y6 < cols) && map[x6][y6] == 0) {
		Node newNeighbor(x6, y6);
		newNeighbor.parent = new Node(node);
		neighbors.push_back(Node(x6, y6));

	}
	if ((x7 >= 0 && x7 < rows) && (y7 >= 0 && y7 < cols) && map[x7][y7] == 0) {
		Node newNeighbor(x7, y7);
		newNeighbor.parent = new Node(node);
		neighbors.push_back(Node(x7, y7));

	}
	if ((x8 >= 0 && x8 < rows) && (y8 >= 0 && y8 < cols) && map[x8][y8] == 0) {
		Node newNeighbor(x8, y8);
		newNeighbor.parent = new Node(node);
		neighbors.push_back(Node(x8, y8));

	}

	return neighbors;
}

void AlgAStarPathfinder::reconstructPath(int** cameFrom, Node curr, Node start, int rows, int cols) {
	int x = -1, y = -1;
	while (!(curr == start)) {
		//cout << curr.x << ", " << curr.y << endl;
		currPath.insert(currPath.begin(), Tile(curr.x, curr.y));
		x = cameFrom[curr.x][curr.y] / cols;
		y = cameFrom[curr.x][curr.y] % cols;
		curr.x = x;
		curr.y = y;
	}
	//cout << curr.x << ", " << curr.y << endl;
	currPath.insert(currPath.begin(), Tile(curr.x, curr.y));
}


void AlgAStarPathfinder::findPath(int** map, int rows, int cols, Node start, Node goal) {
	currPath.clear();

	if (map[start.x][start.y] != 0 || map[goal.x][goal.y] != 0) {
		//cout << "No path!\n";
		found = false;
		return;
	}

	const int INF = 9999;

	start.fValue = f(start, start, goal);
	start.parent = new Node(start);
	goal.fValue = f(start, goal, goal);
	vector<Node> closed;
	vector<Node> open;
	open.push_back(start);

	int** cameFrom;
	cameFrom = new int*[rows];
	for (int a = 0; a < rows; ++a) {
		cameFrom[a] = new int[cols];
		for (int b = 0; b < cols; ++b) {
			cameFrom[a][b] = 0;
		}
	}
	int** gScore = new int*[rows];
	int** fScore = new int*[rows];
	for (int a = 0; a < rows; ++a) {
		gScore[a] = new int[cols];
		fScore[a] = new int[cols];
		for (int b = 0; b < cols; ++b) {
			gScore[a][b] = INF;
			fScore[a][b] = INF;

		}
	}
	gScore[start.x][start.y] = 0;
	fScore[start.x][start.y] = getDistanceBetween(start, goal);

	//cout << "Test2!\n";
	while (!open.empty()) {
		int fValue = INF; int nodeIdx = 0;
		Node currNode(0, 0);
		for (int a = 0; a < open.size(); ++a) {
			Node n = open.at(a);
			if (n.fValue < fValue) {
				fValue = n.fValue;
				currNode = n;
				nodeIdx = a;
			}
		}
		//cout << "Test2-1!\n";

		if (currNode == goal) {
			reconstructPath(cameFrom, currNode, start, rows, cols);
			found = true;
			break;
		}


		closed.push_back(open.at(nodeIdx));
		open.erase(open.begin() + nodeIdx);

		//cout << "Test2-2!\n";
		vector<Node> neighbors = getNeighbors(currNode, map, rows, cols);
		for (Node neigh : neighbors) {
			found = false;//not exist in close
			for (Node node : closed) {
				if (neigh == node) {
					found = true;
					break;
				}
			}
			if (found) continue;

			found = false;//exists in open
			for (Node node : open) {
				if (node == neigh) {
					found = true;
					break;
				}
			}
			if (!found) {//discover if not exists
				neigh.fValue = f(start, neigh, goal);
				open.push_back(neigh);

			}
			//cout << "Test2-3!\n";

			//from start to neigh
			int tentativeGScore = gScore[currNode.x][currNode.y] + getDistanceBetween(currNode, neigh);
			if (tentativeGScore >= gScore[neigh.x][neigh.y])
				continue;//not better path
			//cout << "Test2-4!\n";
			gScore[neigh.x][neigh.y] = tentativeGScore;
			gScore[neigh.x][neigh.y] = gScore[neigh.x][neigh.y] + getDistanceBetween(neigh, goal);
			cameFrom[neigh.x][neigh.y] = currNode.x*cols + currNode.y;
			//cameFrom[currNode.x][currNode.y] = neigh.x*cols + neigh.y;
		}
		found = false;
	}
	//cout << "Test3!\n";
	if (!found)
		//cout << "No Path" << endl;

	
	//clearing data
	for (int a = 0; a < rows; ++a) {
		delete cameFrom[a];
		delete gScore[a];
		delete fScore[a];
	}
	delete cameFrom;
	delete gScore;
	delete fScore;
	//cout << "Test4!\n";
}

vector<Tile> AlgAStarPathfinder::searchPath(int** map, int rows, int cols, Tile start, Tile goal) {
	Node s(start.getX(), start.getY());
	Node g(goal.getX(), goal.getY());
	findPath(map, rows, cols, s, g);
	return currPath;
}