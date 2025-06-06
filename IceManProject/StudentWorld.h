#ifndef STUDENTWORLD_H
#define STUDENTWORLD_H

#include "GameWorld.h"
#include <vector>
#include <string>

class Ice;
class Iceman;

class StudentWorld : public GameWorld
{
public:
    explicit StudentWorld(std::string assetDir);
    virtual ~StudentWorld();

    // framework required overrides
    int  init() override;
    int  move() override;
    void cleanUp() override;

    // helper for Iceman
    void digIceAt(int x, int y);

private:
    Iceman*           m_iceman = nullptr;
    std::vector<Ice*> m_ice;
};

#endif 
