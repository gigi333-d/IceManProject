#include "StudentWorld.h"
#include <string>
#include "Actor.h"
#include <vector>
#include "GameConstants.h"
#include <algorithm>
#include <cstdlib>
#include <sstream>
#include <iomanip>

using namespace std;

GameWorld* createStudentWorld(std::string assetDir)
{
    return new StudentWorld(assetDir);
}

void StudentWorld::digIceAt(int x, int y) {
	bool found = false;

	for (auto e : m_ice) {
		int eX = e->getX() - x;
		int eY = e->getY() - y;
		for (int a = 0; a < 4; a++) {
			for (int b = 0; b < 4; b++) {
				if ((eX == a) && (eY == b) && (e->isVisible())) {
					e->setVisible(false);
					found = true;
				}
			}
		}
	}

	if (found) {
		playSound(SOUND_DIG);
	}
}


StudentWorld::~StudentWorld() { 
    delete m_iceman; m_iceman = nullptr;

    for (auto p : m_ice) delete p;
    m_ice.clear();
}

//checks distance between two points
inline double StudentWorld::distanceCheck(double aX, double aY, double bX, double bY) {
	double x = aX - bX;
	double y = aY - bY;
	return sqrt(x * x + y * y);
}

//checks for ice in 4x4 area based on bottom left corner coordinates
bool StudentWorld::iceCheck(int x, int y) {
	for(auto i : m_ice) {
		int iX = i->getX();
		int iY = i->getY();
		for(int a = 0; a < 4; a++) {
			for(int b = 0; b < 4; b++) {
				if((iX == (x + a)) && (iY == (y + b)) && (i->isVisible())) {
					return true;
				}
			}
		}
	}
	return false;
}

// init
int StudentWorld::init() {
	// Boulder, Gold, and OilBarrels Amounts
	int B = min(int(getLevel() / 2 + 2),9);
	int G = max(int(5 - getLevel() / 2),2);
	int L = min(int(getLevel() + 2),21);

	m_iceman = new Iceman(this);
	m_iceman->setBarrels(L);
	protestorsCount = 0;

	for (int c = 0; c < 64; ++c)
		for (int r = 0; r < 60; ++r)
			if (c < 30 || c > 33)           // skip the shaft
				m_ice.push_back(new Ice(c, r));


	for (int x = 0; x < B; x++) {
		int bX = 0;
		int bY = 0; 

		double low = 7;
		do {
			bX = rand() % 61;
			bY = (rand() % 37) + 20;
			for (auto v : ActivatingObjects) {
				low = distanceCheck(bX, bY, v->getX(), v->getY());
				if (low <= 6) {
					break;
				}
			}
		} while ((low <= 6) || (bX >= 30 && bX <= 33));

		Boulder* b = new Boulder(bX, bY);
		ActivatingObjects.push_back(b);
		digIceAt(bX, bY);
	}

	for (int x = 0; x < G; x++) {
		int gX = 0;
		int gY = 0;
		double low = 7;
		do {
			gX = rand() % 61;
			gY = rand() % 57;
			for (auto v : ActivatingObjects) {
				low = distanceCheck(gX, gY, v->getX(), v->getY());
				if (low <= 6) {
					break;
				}
			}
		} while ((low <= 6) || (gX >= 30 && gX <= 33));

		GoldNugget* n = new GoldNugget(gX, gY, false);
		ActivatingObjects.push_back(n);

	}

	for (int x = 0; x < L; x++) {
		int lX = 0;
		int lY = 0;

		double low = 7;
		do {
			lX = rand() % 61;
			lY = rand() % 57;
			for (auto v : ActivatingObjects) {
				low = distanceCheck(lX, lY, v->getX(), v->getY());
				if (low <= 6) {
					break;
				}
			}
		} while ((low <= 6) || (lX >= 30 && lX <= 33));

		OilBarrel* o = new OilBarrel(lX, lY);
		ActivatingObjects.push_back(o);

	}

	return GWSTATUS_CONTINUE_GAME;
}

// move
int StudentWorld::move()
{
	ostringstream gameStat;
	gameStat << "Lvl " << setw(2) << std::to_string(getLevel())
		<< " Lives: " << setw(2) << getLives()
		<< " Hlth: " << setw(3) << (m_iceman->getHealth() / double(10)) * 100
		<< "% Wtr: " << setw(2) << (m_iceman->getWater())
		<< " Gld: " << setw(2) << (m_iceman->getGold())
		<< " Oil Left: " << setw(2) << (m_iceman->getBarrels())
		<< " Sonar: " << setw(2) << (m_iceman->getSonar())
		<< " Scr: " << setw(6) << (getScore());

	setGameStatText(gameStat.str());

	int T = max(25, int(200 - getLevel()));

	int P = min(15, int(2 + getLevel() * 1.5));

	int probablityOfHardcore = min(90, int(getLevel() * 10 + 30));

	pTicks++;
	if (T <= pTicks && P > protestorCount()) {
			if (rand() % probablityOfHardcore == 1) {
				HardcoreProtestor* p = new HardcoreProtestor();
				Protestors.push_back(p);
				pTicks = 0;
				protestorsCount++;
			}
			else {
				Protestor* p = new Protestor();
				Protestors.push_back(p);
				pTicks = 0;
				protestorsCount++;
			}
	}

    int G = getLevel() * 30 + 290;

    if ((rand() % G) == 1) {
		if ((rand() % 5) == 1) {
			SonarKit* s = new SonarKit();
			ActivatingObjects.push_back(s);
		}
		else {
			int pX = rand() % 60;
			int pY = rand() % 56;

			while (iceCheck(pX, pY)) {
				pX = rand() % 60;
				pY = rand() % 56;
			}

			WaterPool* pool = new WaterPool(pX, pY);
			ActivatingObjects.push_back(pool);
		}
	}

    m_iceman->doSomething();   

	//checks if actors are alive and calls their doSomething methods
	for (auto v : ActivatingObjects) {
		v->doSomething(m_iceman);
		if (!v->isAlive()) {
			v->setVisible(false);
		}
	}

	for (auto v : Protestors) {
		v->doSomething(m_iceman);
		if (!v->isAlive()) {
			v->setVisible(false);
		}
	}

	for (auto v : Squirts) {
		v->doSomething(m_iceman);
		if (!v->isAlive()) {
			v->setVisible(false);
		}
	}

	if (m_iceman->getBarrels() == 0) {
		playSound(SOUND_FINISHED_LEVEL);
		return GWSTATUS_FINISHED_LEVEL;
	}


	if (m_iceman->getHealth() <= 0) {
		decLives();
		return GWSTATUS_PLAYER_DIED;
	}
	return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::cleanUp() {
	auto it = m_ice.begin();
	while (it != m_ice.end()) {
		delete (*it);
		it = m_ice.erase(it);
	}

	auto cons = ActivatingObjects.begin();
	while (cons != ActivatingObjects.end()) {
		delete (*cons);
		cons = ActivatingObjects.erase(cons);
	}

	auto pro = Protestors.begin();
	while (pro != Protestors.end()) {
		delete (*pro);
		pro = Protestors.erase(pro);
	}

	auto squ = Squirts.begin();
	while (squ != Squirts.end()) {
		delete (*squ);
		squ = Squirts.erase(squ);
	}

	delete m_iceman;
}
