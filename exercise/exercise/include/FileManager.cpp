#include "FileManager.h"


FileManager::FileManager(string settingsDirectory) {
	string modelsFilePath = settingsDirectory + "/" + "models.lst";
	string ammosFilePath = settingsDirectory + "/" + "ammo.lst";
	string armorFilePath = settingsDirectory + "/" + "armor.lst";
	string settingsFilePath = settingsDirectory + "/" + "game.stg";

	loaded = readModels(modelsFilePath);
	if (loaded) {
		cout << "FileManager: Main models loaded." << endl;
		loaded = readAmmos(ammosFilePath);
	}
	if (loaded) {
		cout << "FileManager: Ammos loaded." << endl;
		loaded = readArmors(armorFilePath);

	}
	if (loaded) {
		cout << "FileManager: Armors loaded." << endl;
		loaded = readSettings(settingsFilePath);
	}
	if (loaded) {
		cout << "FileManager: Settings loaded." << endl;
	}

}
FileManager::~FileManager() {
	ammos.clear();
	armorFaces.clear();
	armorArmors.clear();
	armorBelows.clear();
	models.clear();
	gameSettings.clear();
}

bool FileManager::readModels(string filePath) {
	std::ifstream file;

	int intVal;
	bool boolVal;
	string strVal;
	string key, value, line, tmp;
	file.open(filePath);
	if (!file.is_open()) {
		stringstream ss;
		ss << "FileManager::models::(" << filePath << "): Can't open." << endl;
		cout << ss.str() << endl;
		error = ss.str();
		return false;
	}

	while (!file.eof()) {
		std::getline(file, line);
		if (line.substr(0, 2) == "//") continue; //comment
		if (line == "") continue; //no content on line
		if (line != "[Model]") continue;

		cout << "Line1: " << line << endl;
		//processing model attributes
		ModelProperty modelProperty;
		modelProperty.animated = false;
		modelProperty.path = "";
		modelProperty.name = "";
		while (!file.eof()) {
			//read attribute
			std::getline(file, line);
			if (line.size() == 0)
				break;

			stringstream ss;
			ss << line;
			cout << ss.str() << endl;
			//processing attribute
			ss >> key;
			if (key == "$Animated") {
				ss >> tmp;
				ss >> intVal;
				modelProperty.animated = intVal;
			}
			else if (key == "$Path") {
				ss >> tmp;
				ss >> strVal;
				modelProperty.path = strVal;
			}
			else if (key == "$Name") {
				ss >> tmp;
				ss >> strVal;
				modelProperty.name = strVal;
			}
			else if (key == "$TextureFolder") {
				ss >> tmp;
				ss >> strVal;
				modelProperty.texFolder = strVal;
			}
			else {
				file.close();
				ss.str("");
				ss.clear();
				ss << "FileManager::models::(" << filePath << "): Wrong attribute '" << key << "'." << endl;
				cout << ss.str() << endl;
				error = ss.str();
				return false;
			}
			line = "";
		}
		models.insert(make_pair(modelProperty.name, modelProperty));
	}

	for (pair<string, ModelProperty> prop : models) {
		cout << "- Model " << prop.first << endl;
		cout << ".prop.animated: " << prop.second.animated << endl;
		cout << ".prop.name: " << prop.second.name << endl;
		cout << ".prop.path: " << prop.second.path << endl;
	}

	file.close();
	return true;
}
bool FileManager::readAmmos(string filePath) {
	std::ifstream file;

	int intVal;
	bool boolVal;
	string strVal;
	double doubleVal;
	string key, value, line, tmp;
	file.open(filePath);
	if (!file.is_open()) {
		stringstream ss;
		ss << "FileManager::ammos::(" << filePath << "): Can't open." << endl;
		cout << ss.str() << endl;
		error = ss.str();
		return false;
	}

	while (!file.eof()) {
		std::getline(file, line);
		if (line.substr(0, 2) == "//") continue; //comment
		if (line == "") continue; //no content on line
		if (line != "[Ammo]") continue; //something else...why?!

										//processing ammo attributes
		AmmoProperty ammoProperty;
		cout << "---------Reading ammo attribs\n";
		while (!file.eof()) {
			//read attribute
			std::getline(file, line);
			if (line.size() == 0) break;

			stringstream ss;
			ss << line;
			//processing attribute
			ss >> key;

			//	ss >> tmp;
			//	ss >> strVal;
			//	cout << key << " : "<<tmp <<" : "<< strVal << endl;

			if (key == "$Type") {
				ss >> tmp;
				ss >> intVal;
				ammoProperty.type = intVal;
			}
			else if (key == "$Name") {
				ss >> tmp;
				getline(ss, strVal);
				strVal = strVal.substr(1, strVal.size() - 1);
				ammoProperty.name = strVal;
			}
			else if (key == "$Model") {
				ss >> tmp;
				ss >> strVal;
				ammoProperty.modelPath = strVal;
			}
			else if (key == "$Desc") {
				ss.str("");
				ss.clear();
				std::getline(file, line);
				while (!file.eof() && line != "}") {
					ss << line << endl;
					std::getline(file, line);
				}
				ammoProperty.desc = ss.str();
			}
			else if (key == "$Damage") {
				ss >> tmp;
				ss >> intVal;
				ammoProperty.damage = intVal;
			}
			else if (key == "$Speed") {
				ss >> tmp;
				ss >> doubleVal;
				ammoProperty.speed = doubleVal;
			}
			else if (key == "$Penetration") {
				ss >> tmp;
				ss >> intVal;
				ammoProperty.penetration = intVal;
			}
			else {
				file.close();
				ss.str("");
				ss.clear();
				ss << "FileManager::ammos::(" << filePath << "): Wrong attribute '" << key << "'." << endl;
				cout << ss.str() << endl;
				error = ss.str();
				return false;
			}
		}
		cout << "---------Ammo attribs readed.\n";
		ammos.insert(make_pair(ammoProperty.type, ammoProperty));
	}

	cout << "Ammos\n";
	for (pair<int, AmmoProperty> prop : ammos) {
		cout << "- Ammo " << prop.first << endl;
		cout << ".prop.damage: " << prop.second.damage << endl;
		cout << ".prop.desc: " << prop.second.desc << endl;
		cout << ".prop.modelPath: " << prop.second.modelPath << endl;
		cout << ".prop.name: " << prop.second.name << endl;
		cout << ".prop.penetration: " << prop.second.penetration << endl;
		cout << ".prop.speed: " << prop.second.speed << endl;
		cout << ".prop.type: " << prop.second.type << endl;
	}

	file.close();
	return true;
}
bool FileManager::readArmors(string filePath) {
	std::ifstream file;
	int intVal;
	bool boolVal;
	string strVal;
	double doubleVal;
	string key, value, line, tmp;
	file.open(filePath);
	if (!file.is_open()) {
		stringstream ss;
		ss << "FileManager::armors::(" << filePath << "): Can't open." << endl;
		cout << ss.str() << endl;
		error = ss.str();
		return false;
	}

	while (!file.eof()) {
		std::getline(file, line);
		if (line.substr(0, 2) == "//") continue; //comment
		if (line == "") continue; //no content on line
		if (line != "[Armor]") continue; //something else...why?!

		ArmorProperty armorProperty;
		while (!file.eof()) {
			//read attribute
			std::getline(file, line);
			if (line.size() == 0) break;

			stringstream ss;
			ss << line;
			//processing attribute
			ss >> key;
			if (key == "$Type") {
				ss >> tmp;
				ss >> strVal;
				armorProperty.type = strVal;
			}
			else if (key == "$Desc") { ///TODO
				ss >> tmp;
				ss >> strVal;
				ss.str("");
				ss.clear();
				std::getline(file, line);
				while (!file.eof() && line != "}") {
					ss << line << endl;
					std::getline(file, line);
				}
				armorProperty.desc = ss.str();
			}
			else if (key == "$Path") {
				ss >> tmp;
				ss >> strVal;
				armorProperty.path = strVal;
			}
			else if (key == "$Name") {
				ss >> tmp;
				getline(ss, strVal);
				strVal = strVal.substr(1, strVal.size() - 1);
				armorProperty.name = strVal;
			}
			else {
				file.close();
				ss.str("");
				ss.clear();
				ss << "FileManager::armors::(" << filePath << "): Wrong attribute '" << key << "'." << endl;
				cout << ss.str() << endl;
				error = ss.str();
				return false;
			}
			line = "";
		}

		if (armorProperty.type == "ARMOR")
			armorArmors.insert(make_pair(armorProperty.name, armorProperty));
		else if (armorProperty.type == "FACE")
			armorFaces.insert(make_pair(armorProperty.name, armorProperty));
		else if (armorProperty.type == "BELOW")
			armorBelows.insert(make_pair(armorProperty.name, armorProperty));
		else {
			stringstream ss;
			ss << "FileManager::armors::(" << filePath << ") Wrong armor type. Needs: ARMOR,FACE or BELOW, Found: " << armorProperty.type << endl;
			cout << ss.str() << endl;
			error = ss.str();
			return false;
		}

	}
	file.close();

	if (false) {
		cout << "Armors\n";
		for (pair<string, ArmorProperty> prop : armorArmors) {
			cout << "- Armor " << prop.first << endl;
			cout << ".prop.type: " << prop.second.type << endl;
			cout << ".prop.desk: " << prop.second.desc << endl;
			cout << ".prop.name: " << prop.second.name << endl;
			cout << ".prop.path: " << prop.second.path << endl;
		}
		cout << "Faces\n";
		for (pair<string, ArmorProperty> prop : armorFaces) {
			cout << "- Armor " << prop.first << endl;
			cout << ".prop.type: " << prop.second.type << endl;
			cout << ".prop.desk: " << prop.second.desc << endl;
			cout << ".prop.name: " << prop.second.name << endl;
			cout << ".prop.path: " << prop.second.path << endl;
		}
		cout << "Belows\n";
		for (pair<string, ArmorProperty> prop : armorBelows) {
			cout << "- Armor " << prop.first << endl;
			cout << ".prop.type: " << prop.second.type << endl;
			cout << ".prop.desk: " << prop.second.desc << endl;
			cout << ".prop.name: " << prop.second.name << endl;
			cout << ".prop.path: " << prop.second.path << endl;
		}
	}
	return true;
}
bool FileManager::readSettings(string filePath) {
	std::ifstream file;
	int intVal;
	bool boolVal;
	string strVal;
	double doubleVal;
	string key, value, line, tmp;
	file.open(filePath);
	if (!file.is_open()) {
		stringstream ss;
		ss << "FileManager::settings::(" << filePath << "): Can't open." << endl;
		cout << ss.str() << endl;
		error = ss.str();
		return false;
	}

	while (!file.eof()) {
		std::getline(file, line);
		if (line.substr(0, 2) == "//") continue; //comment
		if (line == "") continue; //no content on line

									//read line
		std::getline(file, line);
		stringstream ss;
		ss << line;

		//process atribute
		ss >> key;
		if (key.substr(0, 1) == "$") {
			ss >> tmp;
			ss >> strVal;
		}
		else {
			ss.str("");
			ss << "FileManager::settings::(" << filePath << "):" << "Wrong key '" << key << "'." << endl;
			cout << ss.str() << endl;
			error = ss.str();
			return false;
		}

	}
	file.close();
	return true;
}


ModelProperty FileManager::getModelProperty(string name) { return models.at(name); }
AmmoProperty FileManager::getAmmoProperty(int type) { return ammos.at(type); }
map<int, AmmoProperty> FileManager::getAmmoProperties() { return ammos; }
ArmorProperty FileManager::getFaceProperty(string faceName) { return armorFaces.at(faceName); }
ArmorProperty FileManager::getArmorProperty(string armorName) { return armorArmors.at(armorName); }
ArmorProperty FileManager::getBelowProperty(string beloweName) { return armorBelows.at(beloweName); }
string FileManager::getGameProperty(string name) { return gameSettings.at(name); }
string FileManager::getError() { return error; }
bool FileManager::isLoaded() { return loaded; }
