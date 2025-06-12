#include "Actor.h"
#include "StudentWorld.h"
#include "GameConstants.h"      //for key codes

// actor 
Actor::Actor(int imageID, int x, int y, Direction dir, double size, unsigned depth)
    : GraphObject(imageID, x, y, dir, size, depth)
{
    setVisible(true);
}
Actor::~Actor() {}

//ice
Ice::Ice(int x, int y) : Actor(IID_ICE, x, y, GraphObject::right, 1.0, 3) {}
Ice::~Ice() {}
void Ice::doSomething() { /* ice is inert */ }

// iceman
Iceman::Iceman(int x, int y, StudentWorld* w)
    : Actor(IID_PLAYER, x, y, GraphObject::right, 1.0, 0), m_world(w) {
}
Iceman::~Iceman() {}

void Iceman::doSomething()
{
    int ch;
    if (!m_world->getKey(ch)) return;

    int newX = getX();
    int newY = getY();

    switch (ch) {
    case KEY_PRESS_LEFT:  newX -= 1; setDirection(left);  break;
    case KEY_PRESS_RIGHT: newX += 1; setDirection(right); break;
    case KEY_PRESS_UP:    newY += 1; setDirection(up);    break;
    case KEY_PRESS_DOWN:  newY -= 1; setDirection(down);  break;
    default: return;                                       // not a movement key
    }

    // stay inside field (iceman is 4×4)
    if (newX < 0 || newX > VIEW_WIDTH - 4 || newY < 0 || newY > VIEW_HEIGHT - 4)
        return;

    moveTo(newX, newY);
    m_world->digIceAt(newX, newY);   //clear any overlapping ice
}
