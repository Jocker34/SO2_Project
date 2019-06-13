#include "Shared.h"

Shared::Shared(int n) {
	this->humanCounter = 0;
	this->packs = new int[n];
	this->nPM = n;
}

Shared::~Shared() {
	delete packs;
}

void Shared::addHuman(long int speed) {
	Human *human = new Human(humanCounter++, speed);
	this->humans.push_back(human);
}

