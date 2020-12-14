#pragma once
#ifndef OBSERVER_H_
#define OBSERVER_H_
#include <string>
#include <map>
using std::pair;
using std::string;

class Observable;
class Observer {
protected:
	string name;
	double windowWidth=800, windowHeight=600;
public:

	Observer(string name);
	void update(Observable o, pair<double,double> value);

	bool operator ==(const Observer &b) const;
};

#endif