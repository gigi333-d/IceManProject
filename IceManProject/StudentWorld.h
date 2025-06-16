#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "GameConstants.h"
#include <string>
#include <vector>
#include "Actor.h"

using namespace std;

class Ice;
class Iceman;

class StudentWorld : public GameWorld
{
public:
    StudentWorld::StudentWorld(std::string assetDir)
        : GameWorld(assetDir), m_iceman(nullptr) {}

    virtual ~StudentWorld();

    int  init() override;
    int  move() override;
    void cleanUp() override;
    
	int protestorCount() { return protestorsCount; }

    void decProtestors() {protestorsCount--;}

	vector<Ice*> getIce() { return m_ice; }
	vector<ActivatingObject*> getActivatingObjects() { return ActivatingObjects; }
	vector<Protestor*> getProtestors() { return Protestors; }
	vector<Squirt*> getSquirts() { return Squirts; }

	void addActivatingObject(ActivatingObject* a) { ActivatingObjects.push_back(a); }

	double distanceCheck(double aX, double aY, double bX, double bY);

    bool iceCheck(int x, int y);

	void addSquirt(Squirt* s) { Squirts.push_back(s); }

    void digIceAt(int x, int y);

private:
    int protestorsCount = 0;
    int pTicks = 200;
    vector<Ice*> m_ice;
    vector<ActivatingObject*> ActivatingObjects;
    vector<Protestor*> Protestors;
    vector<Squirt*> Squirts;
	Iceman* m_iceman = nullptr;
};

#endif 
