class Human {
	int checkDirection();
public:
	int id;
	int currX, currY;
	int lastX, lastY;
	int humanSpeed;
	int nPacks;
	int destX;
	int destY;
	bool inQ;
	void moveFun();
	Human(int id, long int speed);
	~Human();
};
