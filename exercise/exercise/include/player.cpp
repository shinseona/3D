#include "player.h"
#include "Helper.h"
#include "Tile.h"
#include "Unit.h"
#include <iostream>
Player::Player() {}
Player::Player(const Player& p) {}
Player::Player(string modelPath, string name, int tileX, int tileY):Unit(1,name, modelPath, tileX, tileY) {

}
Player::Player(UnitModel*& playerModel, int tileX, int tileY):Unit(1,playerModel,tileX,tileY) {
//	std::cout << "Player::Player.nrMeshes: "<< playerModel.meshes.size() <<std::endl;
	std::cout << "Player::Player2\n";
}
Player::~Player() { std::cout << "Player::~Player2\n"; }
Player& Player::operator=(Player& e) {
	std::cout << "Player::Player1\n";
	return Player(e.model, e.tileX, e.tileY);
}
