#include "Observer.h"
#include "Observable.h"
#include <iostream>


Observer::Observer(string name) {
	this->name = name;
}

void Observer::update(Observable o, pair<double,double> value) {
	std::cout << name << ": Window size updated" << std::endl;
}


bool Observer::operator ==(const Observer &b) const {
	return name == b.name;
}