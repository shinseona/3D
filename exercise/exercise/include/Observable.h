#pragma once
#ifndef OBSERVABLE_H_
#define OBSERVABLE_H_
#include <list>
#include <map>
#include "Observer.h"
using std::list;
using std::pair;

class Observable{
protected:
	list<Observer> observers;
public:
	void addObserver(Observer& o);
	void removeObserver(const Observer& o);
};

#endif