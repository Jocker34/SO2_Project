#include "Shared.h"
class Window {
public:
	Window();
	~Window();
	void initWindow();
	void scene(int nPM, int nCar);
	void human(int id, int dir, int x, int y, int packs);
	void clearHuman(int x, int y);
	void plane(int id);
	void firm();
	void update(Shared *shared);
};
