#pragma once
#ifndef PROPERTIES_H_
#define PROPERTIES_H_

#include <string>
using std::string;

struct ModelProperty {
	bool animated;
	string path;
	string texFolder;
	string name;//unique

	ModelProperty(string name, string path, string texFolder, bool animated);
	ModelProperty();
};

class Ammo;
struct AmmoProperty {
	int type;//unique
	string name;
	string modelPath;
	string desc;
	float damage;
	float speed;
	float penetration;

	AmmoProperty(int type, string name, string desc, float damage, float speed, float penetration);
	AmmoProperty();
};

struct ArmorProperty {
	string type;
	string desc;
	string path;
	string name;//unique

	ArmorProperty(string type, string desc, string path, string name);
	ArmorProperty();
};

struct UnitProperty {
	int life;
	int armor;
};
#endif