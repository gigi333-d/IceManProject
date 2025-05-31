#ifndef ACTOR_H
#define ACTOR_H

#include "GraphObject.h"      //framework base
#include "GameConstants.h"    //official image IDs and key codes
#include <string>

//-----------------------------------------------------------------
//abstract base for every game object
class Actor : public GraphObject{
public:
    Actor(int imageID, int startX, int startY,
          int dir = right, double size = 1.0, unsigned depth = 0);
    virtual ~Actor();

    virtual void doSomething() = 0;          //one tick of behaviour
};

// -----------------------------------------------------------------
//Ice doesnt do anything each tick
class Ice : public Actor{
public:
    Ice(int startX, int startY);
    virtual ~Ice();
    virtual void doSomething() override;     // does nothing
};

// -----------------------------------------------------------------
//Iceman can move and dig
class StudentWorld;   

class Iceman : public Actor {
public:
    Iceman(int startX, int startY, StudentWorld* world);
    virtual ~Iceman();
    virtual void doSomething() override;

private:
    StudentWorld* m_world; //to call digIceAt
};

#endif
