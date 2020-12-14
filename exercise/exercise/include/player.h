#pragma once
#ifndef PLAYER_H_
#define PLAYER_H_
#include "IModel.h"
#include "Tile.h"
#include "Unit.h"
#include "UnitModel.h"
#include <vector>
using std::vector;

class Player : public Unit{
private:
	Player();
	Player(const Player& p);
public:
	Player(string playerModelPath, string name, int tileX, int tileY);
	Player(UnitModel*& playerModel, int tileX, int tileY);
	~Player();
	Player& Player::operator=(Player& e);
};

#endif