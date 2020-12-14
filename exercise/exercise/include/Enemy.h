#pragma once
#ifndef ENEMY_H_
#define ENEMY_H_
#include "UnitModel.h"
#include "Unit.h"
using std::vector;

class Enemy : public Unit {
private:
	Enemy();
public:
	Enemy(string playerModelPath, string name, int tileX, int tileY);
	Enemy(UnitModel*& playerModel, int tileX, int tileY);
	Enemy(const Enemy& e);
	~Enemy();
};

#endif