#include "Unit.h"
#include "Helper.h"
#include "Tile.h"
#include "Properties.h"
#include "IModel.h"
#include "GameWorld.h"

//constructors
Unit::Unit() {}
Unit::Unit(const Unit& u) {}
Unit::Unit(int type, string name, string modelPath, int tileX, int tileY) {
	cout << "Unit::Unit2\n";
	this->type = type;
	cout << "Im here1\n";
	GameWorld& g = GameWorld::getInstance();
	cout << "Im here2\n";
	worldOriginX = g.getWorldOriginX();
	worldOriginY = g.getWorldOriginY();
	worldOriginZ = g.getWorldOriginZ();
	ModelProperty property;
	property.animated = true;
	property.name = name;
	property.path = modelPath;
	model = new UnitModel(property);
	goToTile(tileX, tileY);
	cout << "Unit::Unit2-1\n";
}
Unit::Unit(int type, UnitModel*& model, int tileX, int tileY) {
	cout << "Unit::Unit(t,m,t,t)1\n";
//	std::cout << "Unit::Unit.nrMeshes: " << model.meshes->size() << "\n";
//	std::cout << "Unit::Unit.nrVertices: " << model.meshes.at(0)->getVertices().size() << "\n";
	this->type = type;
	GameWorld& g = GameWorld::getInstance();
	worldOriginX = g.getWorldOriginX();
	worldOriginY = g.getWorldOriginY();
	worldOriginZ = g.getWorldOriginZ();
	this->model = model; 
	goToTile(tileX, tileY);
	name = "ENEMY";
	cout << "Unit::Unit(t,m,t,t)2\n";
}
Unit::~Unit() {
	std::cout << "Unit::~Unit1\n";
	if(model!=NULL)
		delete model;
	std::cout << "Unit::~Unit2\n";
}

//getters
int Unit::getTileX() {
	return tileX;
}
int Unit::getTileY() {
	return tileY;
}
UnitModel& Unit::getModel() {
	return *model;
}
Tile Unit::getTile() {
	return Tile(tileX, tileY);
}
string Unit::getName() {
	return name;
}
int Unit::getMaxLife() {
	return maxLife;
}
int Unit::getCurrLife() { return currLife; }
int Unit::getMaxArmor() { return maxArmor; }
int Unit::getCurrArmor() { return currArmor; }

//setters
void Unit::setTileX(int x) {
	tileX = x;
	goToTile(this->tileX, this->tileY);
}
void Unit::setTileY(int y) {
	tileY = y;
	goToTile(this->tileX, this->tileY);
}
void Unit::setModel(UnitModel& model) {
	this->model = &model;
}
void Unit::setTile(Tile t) {
	this->tileX = t.getX();
	this->tileY = t.getY();
	goToTile(this->tileX, this->tileY);
}
void Unit::setRoute(vector<Tile> route, bool returnToStartPos) {
	this->route = route;
	currFrame = 0;
	totalFrames = (int)route.size();
	currFrameTime = 0.0f;
	frameTime = 500.0f;//miliseconds
	this->returnToStartPos = returnToStartPos;
	movementSide = 1;
}
void Unit::setName(string name)
{
	this->name = name;
}
void Unit::setMaxLife(int life) {
	this->maxLife = life;
}
void Unit::setCurrLife(int life) {
	this->currLife = life;
}
void Unit::setCurrArmor(int currArmor) {
	this->currArmor = currArmor;
}
void Unit::setMaxArmor(int maxArmor) {
	this->maxArmor = maxArmor;
}
void Unit::damage(int dmg) {
	setCurrLife(getCurrLife() - dmg);
}
void Unit::setSkin(ModelPart part, ModelProperty property) {
	model->changeTexture(part, property);
}

//functions
void Unit::translate(float x, float y, float z) {
	model->translate(x, y, z);
	vec3 coord = model->getCoords();
	Helper h;
	vec2 tile = h.toTileCoords(coord, vec3(worldOriginX, worldOriginY, worldOriginZ));
	cout << "Player tile: " << tile.x << ", " << tile.y << endl;
	tileX = tile.x;
	tileY = tile.y;
}
void Unit::translate(Tile t) {
	model->translate(t.getX(), worldOriginY, t.getY());
	this->tileX = tileX;
	this->tileY = tileY;
	vec3 coord = model->getCoords();
	Helper h;
	vec2 tile = h.toTileCoords(coord, vec3(worldOriginX, 0, worldOriginZ));
	cout << "Player tile: " << tile.x << ", " << tile.y << endl;

}
void Unit::goToTile(int tileX, int tileY) {
	this->tileX = tileX;
	this->tileY = tileY;
	model->goTo(worldOriginX+tileY * 2.0f, 0, worldOriginZ+tileX * 2.0f);
}
void Unit::goToTile(Tile t) {
	int tileX = t.getX();
	int tileY = t.getY();
	this->tileX = tileX;
	this->tileY = tileY;
	model->goTo(tileY * 2.0f-1, 0, tileX * 2.0f-1);
}
void Unit::move(double time) {
	currFrameTime += time;
	if (currFrameTime >= frameTime) {//let's try to move
		currFrameTime -= frameTime;

		if (movementSide == 1) {//running from start position to end position
			if (currFrame < totalFrames) {
				Tile currTile = route.at(currFrame);
				float degree = calculateRotationDegree(currTile);
				goToTile(currTile);
				rotate(degree);
				++currFrame;
			}
			else
				movementSide = 2;//change move direction
		}
		else if (movementSide == 2) { //end to start
			if (!returnToStartPos) { route.clear(); }//if we don't need to return to start position then route finished
			else {
				if (currFrame > 0) {
					--currFrame;
					Tile currTile = route.at(currFrame);
					goToTile(currTile);
				}
				else
					route.clear();//route finished
			}
		}
	}
}
void Unit::update(double time) {
	model->update(time);
	if (route.size() != 0) {//if player has route
		move(time);
	}
}
bool Unit::hasRoute() {
	return route.size() == 0 ? false : true;
}
float Unit::calculateRotationDegree(Tile nextTile) {
	int deltaX = tileX - nextTile.getX();
	int deltaY = tileY - nextTile.getY();

	if (deltaX == 1 && deltaY == 1) return -135.0f;//left up corner
	if (deltaX == 0 && deltaY == 1) return -90.0f;//up
	if (deltaX == -1 && deltaY == 1) return -45.0f;//right up corner
	if (deltaX == 1 && deltaY == 0) return 180.0f;//left
	if (deltaX == 0 && deltaY == 0) return 0.0f;//the same coord
	if (deltaX == -1 && deltaY == 0) return 0.0f;//right
	if (deltaX == 1 && deltaY == -1) return 135.0f;//left bottom corner 
	if (deltaX == 0 && deltaY == -1) return 90.0f;//bottom
	if (deltaX == -1 && deltaY == -1) return 45.0f;//right bottom corner


	return 0.0f;
}
void Unit::rotate(float degree) {
	model->rotateDegree(degree, 2);
}