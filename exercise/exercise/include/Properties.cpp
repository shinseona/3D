#include "Properties.h"



ModelProperty::ModelProperty(string name, string path, string texFolder, bool animated) {
	this->name = name;
	this->path = path;
	this->texFolder = texFolder;
	this->animated = animated;
}
ModelProperty::ModelProperty() {}

AmmoProperty::AmmoProperty(int type, string name, string desc, float damage, float speed, float penetration) {
	this->type = type;
	this->name = name;
	this->desc = desc;
	this->damage = damage;
	this->speed = speed;
	this->penetration = penetration;
}
AmmoProperty::AmmoProperty() {}



ArmorProperty::ArmorProperty(string type, string desc, string path, string name) {
	this->type = type;
	this->desc = desc;
	this->path = path;
	this->name = name;
}
ArmorProperty::ArmorProperty() {}
