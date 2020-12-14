#include <iostream>
#include "Ammo.h"
#include "Helper.h"


Ammo::Ammo(StaticModel& ammoModel, AmmoProperty& ammoProperty, vec3 origin, vec3 direction, vec3 rotationDegrees) {
	cout << "Ammo::Ammo1\n";
	distance = direction;
	time = length(distance);
	speed = (distance / time);
	speed *= 0.01;

	currentPosition = origin;
	currentTime = 0.0f;
	this->origin = origin;
	this->ammoProperty = ammoProperty;
	model = ammoModel;
	model.goTo(currentPosition);
	//	model.rotateDegree(90.0f, 2);// <-y->, vx^, z
	model.rotateDegree(rotationDegrees.y, 2);
	model.rotateDegree(rotationDegrees.z, 3);
	///model.rotateDegree(rotationDegrees.y, 2);
	///model.rotateDegree(rotationDegrees.z, 3);
	//model.rotateDegree(rotationDegrees.z, 3);
	//	model.rotateRad(rotationDegrees.x, 1);
	status = 1;//alive

	Helper h;
	h.showMatrix(model.getRotationMatrix());
}
Ammo::~Ammo() {
	cout << "Ammo::~Ammo\n";
}

//getters
StaticModel& Ammo::getModel() {
	return model;
}
vec3 Ammo::getSpeed() {
	return speed;
}
vec3 Ammo::getOrigin() {
	return origin;
}
int Ammo::getType() {
	return type;
}
int Ammo::getStatus() { return status; }

//setters
void Ammo::setModel(StaticModel& model) {}

//functions
void Ammo::update(double time) {//in milliseconds
	float currPosLen = length(currentPosition - origin);
	if (currPosLen >= radius) {
		status = 2;
		return;
	}

//	time /= 100.0f;
	currentTime += time;


	currentPosition = origin+speed*currentTime;
	model.goTo(currentPosition);

 	//tmp
	/*Helper h;
	cout << "-----START-----"<< endl;
	cout << "Time: " << time << endl;
	cout << "currentTime: " << currentTime << endl;
	cout << "origin: \n";
	h.showVector(origin);
	cout << "speed: \n";
	h.showVector(speed);
	cout << "currentTime*speed: \n";
	h.showVector(currentTime*speed);
	cout << "currentPosition: \n";
	h.showVector(currentPosition);
	cout << "-----END-----" << endl;*/
}
AmmoProperty Ammo::getProperty() {
	return ammoProperty;
}