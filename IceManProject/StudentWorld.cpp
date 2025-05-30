#include "StudentWorld.h"
#include <string>
using namespace std;

GameWorld* createStudentWorld(string assetDir)
{
	return new StudentWorld(assetDir);
}

int StudentWorld::init() {
    m_player = new Iceman(this);

    for (int i = 0; i < VIEW_WIDTH; ++i) {
        for (int j = 0; j < VIEW_HEIGHT; ++j) {
            if (j > 3) {
		if (i >= 30 && i <= 33) {
		continue;
		}
	    }	
	Ice* iceField = new Ice(i, j);
	ice.push_back(iceField);
        }
    }
	return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move() {
	player->doSomething();
	return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::cleanUp() {
	auto i = ice.begin();
	while (i != ice.end()) {
		delete(*i);
		i = ice.erase(i);
	}
	delete(player);
}
