#ifndef ACTOR_H
#define ACTOR_H

#include "GraphObject.h"      //framework base
#include "GameConstants.h"    //official image IDs and key codes
#include <string>
#include <vector>
#include <algorithm>

using namespace std;

class StudentWorld;


//abstract base for every game object
class Actor : public GraphObject {
public:
    Actor(int imageID, int startX, int startY,
        Direction dir = right, double size = 1.0, unsigned depth = 0) : 
        GraphObject(imageID, startX, startY, dir, size, depth) {
        setVisible(true);
    }

    virtual ~Actor() {}

};


class Ice : public Actor {
public:
    Ice(int startX, int startY)
        : Actor(IID_ICE, startX, startY, right, .25, 3) {
        setVisible(true);
    }
    ~Ice() {}
};

class Iceman : public Actor {
public:
	Iceman(StudentWorld* w)
		: Actor(IID_PLAYER, 30, 60, right, 1.0, 0), m_world(w) {
		setVisible(true);
	}
	~Iceman() {}

    bool boulderCheck(Direction dir);

	void doSomething();

	StudentWorld* getWorld() { return m_world; }

	void setBarrels(int b) { barrelsLeft = b; } 

	void addGold() { gold++;}

	void decGold() { gold--; }

	void addSonar() { sonar++;}

    void decSonar() { sonar--; }

	void addSquirt() { squirt += 5; }

	void decSquirt() { squirt--; }

	void annoy(unsigned int x) { health -= x; }

	int getBarrels() { return barrelsLeft; }

	int getHealth() { return health; }
	int getWater() { return squirt; }
	int getSonar() { return sonar; }
	int getGold() { return gold; }

private:
	StudentWorld* m_world;
	int health = 10;
	int squirt = 5;
	int sonar = 1;
	int gold = 0;
	int barrelsLeft = 0;
	bool alive = true;
};

class Protestor : public Actor {
public:
	Protestor(int imageID = IID_PROTESTER, int startX = 60, int startY = 60, Direction d = left, double size = 1.0, unsigned int depth = 0)
		: Actor(imageID, 60, 60, left, 1.0, 0) {
		setVisible(true);
	}

	virtual ~Protestor() {}

	virtual void doSomething(Iceman* p);

	virtual bool pathCheck(Iceman* p, int X, int Y);

	virtual void annoyed(Iceman* p, int damage);

	virtual void foundGold(Iceman* p);

	virtual void leave(Iceman* p);

	virtual bool isAlive() { return alive; }

private:
	int health = 5;
	int numSquares = (rand() % 53) + 8;
	bool leaving = false;
	bool alive = true;
	int annoyance = 2;
	int restTicks = 0;
	int shoutTicks = 15;
	int perpTicks = 200;
	std::vector<std::vector<int>> path;
};

class HardcoreProtestor : public Protestor {
public:
    HardcoreProtestor()
        : Protestor(IID_HARD_CORE_PROTESTER, 60, 60, left, 1.0, 0) {
        setVisible(true);
    }
    virtual ~HardcoreProtestor() {}

    void doSomething(Iceman* p);
    void annoyed(Iceman* p, int damage);
    void findPath(Iceman* p, bool isLeaving);
    void foundGold(Iceman* p);
    bool isAlive() { return alive; }

private:
    int numSquares = (rand() % 53) + 8;
    int health = 20;
    int restTicks = 0;
    int shoutTicks = 15;
    int perpTicks = 200;

    bool alive = true;
    bool leaving = false;

    struct Coord {
        int x, y, distance;
        Coord(int X = 0, int Y = 0, int d = 0) : x(X), y(Y), distance(d) {}
    };

    std::vector<std::vector<int>> path;
    std::set<std::pair<int, int>> visited;
};


// Water projectile
class Squirt : public Actor {
public:
    Squirt(int startX, int startY, Direction dir)
        : Actor(IID_WATER_SPURT, startX, startY, dir, 1.0, 1){
            setVisible(true);
        }
    ~Squirt() {}

    void doSomething(Iceman* p);
    bool isAlive() { return alive; }

private:
    bool alive = true;
    double radius = 3.0;
    int distance = 4;
    int annoyance = 2;
    int travel = 0;
    int ticks = 0;
};


class ActivatingObject : public Actor {
public:
    ActivatingObject(int imageID, int startX, int startY, Direction dir, double size, unsigned int depth)
        : Actor(imageID, startX, startY, dir, size, depth) {
    }
    virtual ~ActivatingObject() {}

    virtual void doSomething(Iceman* p) = 0;
    virtual bool isAlive() = 0;
    virtual bool isBoulder() = 0;
};


class Boulder : public ActivatingObject {
public:
    Boulder(int startX, int startY)
        : ActivatingObject(IID_BOULDER, startX, startY, down, 1.0, 1) {
        setVisible(true);
    }
    ~Boulder() {}

    void doSomething(Iceman* p);
    bool isAlive() { return alive; }
    bool isBoulder() { return true; }

private:
    enum State { stable, waiting, falling };
    State state = stable;
    bool alive = true;
    int ticks = 0;
};


class OilBarrel : public ActivatingObject {
public:
    OilBarrel(int startX, int startY)
        : ActivatingObject(IID_BARREL, startX, startY, right, 1.0, 2) {
        setVisible(false);
    }
	virtual ~OilBarrel() {}

    void doSomething(Iceman* p);
    bool isAlive() { return alive; }
    bool isBoulder() { return false; }

private:
    bool alive = true;
    int score = 1000;
};

class GoldNugget : public ActivatingObject {
public:
    GoldNugget(int startX, int startY, bool userMade)
        : ActivatingObject(IID_GOLD, startX, startY, right, 1.0, 2) {
        setVisible(userMade);
        dropped = userMade;
        state = userMade ? temporary : permanent;
    }

	virtual ~GoldNugget() {}

    void doSomething(Iceman* p);
    bool isAlive() { return alive; }
    bool isBoulder() { return false; }

private:
    enum State { permanent, temporary };
    State state;
    bool dropped;
    bool alive = true;
    int ticks = 0;
};


class SonarKit : public ActivatingObject {
public:
    SonarKit() : ActivatingObject(IID_SONAR, 0, 60, right, 1.0, 2) {
        setVisible(true);
    }
	virtual ~SonarKit() {}

    void doSomething(Iceman* p);
    bool isAlive() { return alive; }
    bool isBoulder() { return false; }

private:
    bool alive = true;
    int ticks = 0;
    int score = 75;
};


class WaterPool : public ActivatingObject {
public:
    WaterPool(int startX, int startY)
        : ActivatingObject(IID_WATER_POOL, startX, startY, right, 1.0, 2) {
        setVisible(true);
    }
	virtual ~WaterPool() {}

    void doSomething(Iceman* p);
    bool isAlive() { return alive; }
    bool isBoulder() { return false; }

private:
    bool alive = true;
    int ticks = 0;
    int score = 100;
};

#endif
