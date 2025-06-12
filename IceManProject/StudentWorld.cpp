#include "StudentWorld.h"
#include <string>
#include "Actor.h"
#include <vector>
#include "GameConstants.h"
#include <algorithm>

GameWorld* createStudentWorld(std::string assetDir)
{
    return new StudentWorld(assetDir);
}

StudentWorld::StudentWorld(std::string assetDir)
    : GameWorld(assetDir), m_iceman(nullptr) {
}

StudentWorld::~StudentWorld() { 
    delete m_iceman; m_iceman = nullptr;

    for (auto p : m_ice) delete p;
    m_ice.clear();
}

// init
int StudentWorld::init()
{
    // build full ice field except central 4-column shaft (30..33)
    for (int c = 0; c < 64; ++c)
        for (int r = 0; r < 60; ++r)
            if (c < 30 || c > 33)           // skip the shaft
                m_ice.push_back(new Ice(c, r));

    // place iceman at top of shaft
    m_iceman = new Iceman(30, 60, this);

    return GWSTATUS_CONTINUE_GAME;
}

// move
int StudentWorld::move()
{
    m_iceman->doSomething();      // only actor that does anything in part 1
    return GWSTATUS_CONTINUE_GAME;
}

//cleanUp
void StudentWorld::cleanUp()
{
    delete m_iceman; m_iceman = nullptr;

    for (auto p : m_ice) delete p;
    m_ice.clear();
}

// dig ice at
void StudentWorld::digIceAt(int x, int y)
{
    std::vector<Ice*> keep;
    keep.reserve(m_ice.size());

    for (auto p : m_ice) {
        int ix = p->getX(), iy = p->getY();
        bool overlap = (ix >= x && ix <= x + 3 && iy >= y && iy <= y + 3);
        if (overlap)
            delete p;
        else
            keep.push_back(p);
    }
    m_ice.swap(keep);
}
