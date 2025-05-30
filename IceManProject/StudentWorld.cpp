#include "StudentWorld.h"
#include <string>
using namespace std;

GameWorld* createStudentWorld(string assetDir)
{
	return new StudentWorld(assetDir);
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
