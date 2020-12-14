#pragma once
#ifndef FILEMANAGER_H_
#define FILEMANAGER_H_

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <map>
#include <regex>
#include "Properties.h"
using namespace std;

class FileManager {
private:
	map<int, AmmoProperty> ammos;
	map<string, ArmorProperty> armorFaces;
	map<string, ArmorProperty> armorArmors;
	map<string, ArmorProperty> armorBelows;
	map<string, ModelProperty> models;
	map<string, string> gameSettings;
	bool loaded;
	string error;

	bool readModels(string filePath);
	bool readAmmos(string filePath);
	bool readArmors(string filePath);
	bool readSettings(string filePath);
public:
	FileManager(string settingsDirectory);
	~FileManager();

	ModelProperty getModelProperty(string name);
	AmmoProperty getAmmoProperty(int type);
	map<int, AmmoProperty> getAmmoProperties();
	ArmorProperty getFaceProperty(string faceName);
	ArmorProperty getArmorProperty(string armorName);
	ArmorProperty getBelowProperty(string beloweName);
	string getGameProperty(string name);
	string getError();
	bool isLoaded();
};

#endif
