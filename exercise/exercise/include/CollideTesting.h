#pragma once
#include "AlgBoxBoxAABB.h"
#include "Ammo.h"
#include "AmmoManager.h"
#include "Enemy.h"
#include <vector>
#include <list>
using namespace std;


class CollideTesting {
private:
public:
	void test(AmmoManager* ammoManager, vector<Enemy> enemies);
};