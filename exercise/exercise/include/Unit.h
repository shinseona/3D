#include<vector>
#include "UnitModel.h"
#include "Tile.h"

#pragma once
#ifndef UNIT_H_
#define UNIT_H_

class Unit {
protected:
	string name = "UNIT";
	//movement
	vector<Tile> route;//current unit path
	int currFrame, totalFrames;
	double currFrameTime, frameTime;
	bool returnToStartPos;
	int movementSide;
	//unit attributes
	int currLife = 300;
	int maxLife = 500;
	int maxArmor = 100;
	int currArmor = 50;

	int tileX, tileY;//current coords
	float worldOriginX, worldOriginY,worldOriginZ;//world origins
	UnitModel* model;//unit model
	int type;

	Unit();
	Unit(const Unit& u);

	void goToTile(int tileX, int tileY);
	void goToTile(Tile t);
	void move(double time);
	float calculateRotationDegree(Tile nextTile);

public:
	Unit(int type, string name, string playerModelPath, int tileX, int tileY);
	Unit(int type, UnitModel*& playerModel, int tileX, int tileY);
	~Unit();

	//getters
	int getTileX();
	int getTileY();
	Tile getTile();
	UnitModel& getModel();
	string getName();
	int getMaxLife();
	int getCurrLife();
	int getMaxArmor();
	int getCurrArmor();

	//setters
	void setTileX(int x);
	void setTileY(int y);
	void setTile(Tile t);
	void setModel(UnitModel& model);
	void setRoute(vector<Tile> route, bool returnToStartPos);
	void setName(string name);
	void setMaxLife(int life);
	void setCurrLife(int life);
	void setCurrArmor(int currArmor);
	void setMaxArmor(int maxArmor);
	void damage(int dmg);
	void setSkin(ModelPart part, ModelProperty property);

	//functions
	void translate(float x, float y, float z);
	void translate(Tile tile);
	void update(double time);
	bool hasRoute();
	void rotate(float degree);
};

#endif