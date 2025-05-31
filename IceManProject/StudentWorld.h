#ifndef STUDENTWORLD_H
#define STUDENTWORLD_H

#include "GameWorld.h"
#include <vector>

class Ice;
class Iceman;

class StudentWorld : public GameWorld {
public:
    StudentWorld(std::string assetDir);
    virtual ~StudentWorld();

    virtual int init() override;   // build ice grid and iceman
    virtual int move() override;   // one game tick
    virtual void cleanUp() override;

    bool getKey(int& ch)                   { return getKeyFromController(ch); }
    void digIceAt(int x, int y);           // called by Iceman

private:
    std::vector<Ice*> m_ice;
    Iceman*           m_iceman;
};

#endif   // STUDENTWORLD_H


private:
	Iceman* player;
	std::vector<Ice*> ice;
};

#endif // STUDENTWORLD_H_
