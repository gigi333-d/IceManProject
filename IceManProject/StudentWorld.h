#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "GameConstants.h"
#include <string>
#include <vector>
#include "Actor.h"

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
    Iceman* m_iceman = nullptr;
    std::vector<Ice*> m_ice;
};

#endif 
