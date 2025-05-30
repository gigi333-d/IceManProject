#pragma once
#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "GameConstants.h"
#include <string>

// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp

class StudentWorld : public GameWorld
{
public:
	StudentWorld(std::string assetDir)
		: GameWorld(assetDir)
	{
	}

	virtual ~StudentWorld() {
		auto i = ice.begin();
		while (i != ice.end()) {
		delete(*i);
		i = ice.erase(i);
	}
	delete(player);
        }

	virtual int init();
	//{
	//	return GWSTATUS_CONTINUE_GAME;
	//}

	virtual int move();
	//{
		// This code is here merely to allow the game to build, run, and terminate after you hit enter a few times.
		// Notice that the return value GWSTATUS_PLAYER_DIED will cause our framework to end the current level.
		//decLives();
		//return GWSTATUS_PLAYER_DIED;
	//}

	virtual void cleanUp();

private:
	Iceman* player;
	std::vector<Ice*> ice;
};

#endif // STUDENTWORLD_H_
