#include "Human.h"
#include <vector>
class Shared { 
public:
	std::vector<Human*> humans;
	int *packs;
	int nPM;
	int humanCounter;
	int clientCounter;
	Shared(int n);
	~Shared();
	void addHuman(long int speed);
	void addClient(long int speed);
};
