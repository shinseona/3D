#pragma once
#ifndef AMMOMANAGER_H_
#define AMMOMANAGER_H_

#include "IModel.h"
#include "Ammo.h"
#include "Shader.h"
#include "Properties.h"
#include <list>
#include <map>
#include <glm/vec3.hpp>
using glm::vec3;
using std::map;
using std::pair;
using std::list;

class GameWorld;
class AmmoManager {
private:
	map<int, StaticModel> models;//{type,model}
	map<int, AmmoProperty> properties;//{type,property}
	list<Ammo*> ammos;
public:
	AmmoManager(map<int, AmmoProperty> properties);
	~AmmoManager();
	void addAmmo(int type, vec3 origin, vec3 direction, vec3 rotationDegrees);
	void addAmmo(Ammo*& ammo, vec3 rotationDegrees);
	void addProperty(int type, AmmoProperty property);
	void addModel(int type, StaticModel& model);
	void clearAmmos();
	void update(float time);//miliseconds
	list<Ammo*>& AmmoManager::getAmmos();
	void removeAmmo(Ammo* ammo);
};

#endif