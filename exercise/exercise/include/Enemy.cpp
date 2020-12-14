#include <iostream>
#include "Enemy.h"
#include "IModel.h"
#include "Unit.h"
Enemy::Enemy() {}
Enemy::Enemy(const Enemy& e){
	this->route=route;//current unit path
	this->currFrame = e.currFrame;
	this->totalFrames = e.totalFrames;
	this->currFrameTime = e.currFrameTime;
	this->frameTime = e.frameTime;
	this->returnToStartPos= e.returnToStartPos;
	this->movementSide = e.movementSide;

	this->tileX = e.tileX;
	this->tileY = e.tileY;
	this->worldOriginX = e.worldOriginX;
	this->worldOriginY = e.worldOriginY;
	this->worldOriginZ = e.worldOriginZ;
	this->model = e.model;
	this->type= e.type;
}
Enemy::Enemy(string modelPath, string name, int tileX, int tileY):Unit(2,name,modelPath,tileX,tileY) {
}
Enemy::Enemy(UnitModel*& model, int tileX, int tileY):Unit(2, model, tileX, tileY) {
	cout << "Enemy::Enemy: " << &this->model << endl;
}
Enemy::~Enemy() {
	std::cout << "Enemy::~Enemy\n";
}

