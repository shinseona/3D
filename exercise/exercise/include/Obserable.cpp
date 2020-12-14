#include "Observable.h"
#include "Observer.h"

void Observable::addObserver(Observer& o) {
	observers.push_back(o);
}

void Observable::removeObserver(const Observer& o) {
	observers.remove(o);
}