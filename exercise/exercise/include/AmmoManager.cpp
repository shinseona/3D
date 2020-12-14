#include "AmmoManager.h"
#include <iostream>

AmmoManager::AmmoManager(map<int, AmmoProperty> properties) {
	this->properties = properties;

	for (pair<int, AmmoProperty> prop : properties) {
		prop.second.modelPath;
		ModelProperty modelProperty;
		modelProperty.animated = false;
		modelProperty.name = prop.second.type;
		modelProperty.path = prop.second.modelPath;
		StaticModel model(modelProperty);
		models.insert(make_pair(prop.first, model));
	}
	this->models = models;
}
AmmoManager::~AmmoManager() {
	models.clear();
	properties.clear();
	for (Ammo* ammo : ammos) {
		delete ammo;
	}
	ammos.clear();
}

void AmmoManager::addAmmo(Ammo*& ammo, vec3 rotationDegrees) {
	ammos.push_back(ammo);
}
void AmmoManager::addAmmo(int type, vec3 origin, vec3 direction, vec3 rotationDegrees) {
	if (models.find(type) == models.end()) {
		cout << "AmmoManager::addAmmo: Model with type " << type<<" not found" << endl;
		return;
	}
	if (properties.find(type) == properties.end()) {
		cout << "AmmoManager::addAmmo: Property with type " << type << " not found" << endl;
		return;
	}

	Ammo* ammo = new Ammo(models[type], properties[type], origin, direction,rotationDegrees);
	ammos.push_back(ammo);
}
void AmmoManager::addProperty(int type, AmmoProperty prop) {
	properties.insert(properties.end(),pair<int, AmmoProperty>(type, prop));
}
void AmmoManager::addModel(int type, StaticModel& model) {
	models.insert(models.end(), pair<int, StaticModel>(type, model));
}
//
void AmmoManager::clearAmmos() {
	ammos.clear();
}
void AmmoManager::update(float time) {
	vector<Ammo*> toDelete;
	//find ammos out of radius
	for (Ammo*& ammo : ammos) {
		if (ammo->getStatus() == 2)
			toDelete.push_back(ammo);
	}
	//remove ammos out of radius
	for (Ammo*& ammo : toDelete) {
		ammos.remove(ammo);
	}
	//update positions
	for (Ammo*& ammo : ammos) {
		ammo->update(time);
	}
}

list<Ammo*>& AmmoManager::getAmmos() {
	return ammos;
}

void AmmoManager::removeAmmo(Ammo* ammo) {
	ammos.remove(ammo);
}