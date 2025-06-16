#include "Actor.h"
#include "StudentWorld.h"
#include "GameConstants.h"  
#include <vector>
#include <set>
#include <queue>
using namespace std;

//checks if there is a boulder in the direction of movement
bool Iceman::boulderCheck(Direction dir) {
	vector<ActivatingObject*> a = getWorld()->getActivatingObjects();

	int newX = getX();
	int newY = getY();

	for (auto d : a) {
		if (d->isBoulder() && d->isAlive()) {
			if (dir == right) {
				if (((d->getX() - 4) == newX) && (sqrt((d->getY() - newY) * (d->getY() - newY)) < 4)) {
					return true;
				}
			}
			else if (dir == left) {
				if (((d->getX() + 4) == newX) && (sqrt((d->getY() - newY) * (d->getY() - newY)) < 4)) {
					return true;
				}
			}
			else if (dir == up) {
				if ((sqrt((d->getX() - newX) * (d->getX() - newX)) < 4) && ((d->getY() - 4) == newY)) {
					return true;
				}
			}
			else if (dir == down) {
				if ((sqrt((d->getX() - newX) * (d->getX() - newX)) < 4) && ((d->getY() + 4) == newY)) {
					return true;
				}
			}
		}
	}
	return false;
}

void Iceman::doSomething()
{
    if(!alive){
		return;
	}

	int key;
	int newX = getX();
	int newY = getY();

	getWorld()->digIceAt(newX, newY);

	if(getWorld()->getKey(key)){
		switch (key) {
		case KEY_PRESS_LEFT:
			if(newX > 0 && getDirection() == left && !boulderCheck(left)){
				moveTo(newX - 1, newY);
			}
			else{
				setDirection(left);
			}
			break;
		case KEY_PRESS_RIGHT:
			if(newX < 60 && getDirection() == right && !boulderCheck(right)){
				moveTo(newX + 1, newY);
			}
			else{
				setDirection(right);
			}
			break;
		case KEY_PRESS_UP:
			if(newY < 60 && getDirection() == up && !boulderCheck(up)){
				moveTo(newX, newY + 1);
			}
			else{
				setDirection(up);
			}
			break;
		case KEY_PRESS_DOWN:
			if(newY > 0 && getDirection() == down && !boulderCheck(down)){
				moveTo(newX, newY - 1);
			}
			else{
				setDirection(down);
			}
			break;
		case KEY_PRESS_SPACE:
			if(squirt == 0){
				break;
			}
			{
				--squirt;
				Squirt* s = new Squirt(newX, newY, getDirection());
				getWorld()->addSquirt(s);
				getWorld()->playSound(SOUND_PLAYER_SQUIRT);
			}
			break;
		case KEY_PRESS_ESCAPE:
			annoy(10);
			break;
		case KEY_PRESS_TAB:
			if(gold == 0) {
				break;
			}

			{
				gold--;
				GoldNugget* n = new GoldNugget(newX, newY, true);
				getWorld()->addActivatingObject(n);
			}
			break;
		case 'z':
			if(sonar <= 0){
				break;
			}
			sonar--;
			for(auto a : getWorld()->getActivatingObjects()) {
				if(!a->isBoulder() && a->isAlive() && !a->isVisible()) {
					double d = getWorld()->distanceCheck(newX, newY, a->getX(), a->getY());
					if(d <= 12) {
						a->setVisible(true);
					}
				}
			}
			break;
		}
	}
}

void Boulder::doSomething(Iceman* p) {
	if (!alive) {
		return;
	}

	if (state == stable) {
		int x = getX();
		int y = getY();

		vector<Ice*> v = p->getWorld()->getIce();

		for (auto i : v) {
			int iX = i->getX();
			int iY = i->getY();

			for (int a = 0; a < 4; a++) {
				if ((iX == (x + a)) && (iY == (y - 1)) && (i->isVisible())) {
					return;
				}
			}
		}

		state = waiting;

	}
	else if (state == waiting) {
		ticks++;
		if (ticks == 30) {
			state = falling;
			p->getWorld()->playSound(SOUND_FALLING_ROCK);
		}
	}
	else if (state == falling) {
		int x = getX();
		int y = getY();

		moveTo(x, y - 1);
		if(y == 0) {
			alive = false;
			return;
		}

		vector<ActivatingObject*> a = p->getWorld()->getActivatingObjects();
		for (auto d : a) {
			if(d->isBoulder() && (d != this)) {
				for (int k = 0; k < 4; k++) {
					if (((d->getX() + k) == x) && (d->getY() == y + 4)) {
						alive = false;
						return;
					}
				}
			}
		}

		vector<Ice*> v = p->getWorld()->getIce();

		for(auto i : v) {
			int iX = i->getX();
			int iY = i->getY();

			for(int a = 0; a < 4; a++) {
				if((iX == (x + a)) && (iY == (y - 1)) && (i->isVisible())) {
					alive = false;
					return;
				}
			}
		}

		double distance = p->getWorld()->distanceCheck(getX(), getY(), p->getX(), p->getY());

		if (distance <= 3) {
			p->annoy(100);
		}

		for (auto n : p->getWorld()->getProtestors()) {
			double d = p->getWorld()->distanceCheck(getX(), getY(), n->getX(), n->getY());
			if (d <= 3) {
				n->annoyed(p, 100);
			}
		}
	}
}

void Squirt::doSomething(Iceman* p) {
	if(!alive) {
		return;
	}
	int x = getX();
	int y = getY();

	if (travel > 8) {
		alive = false;
	}

	if (getDirection() == left) {
		if((p->getWorld()->iceCheck(x - 1, y)) || (p->boulderCheck(left))) {
			alive = false;
			return;
		}
		else
			moveTo(x - 1, y);
	}
	else if (getDirection() == right) {
		if((p->getWorld()->iceCheck(x + 1, y)) || (p->boulderCheck(right))) {
			alive = false;
			return;
		}
		else
			moveTo(x + 1, y);
	}
	else if(getDirection() == up) {
		if ((p->getWorld()->iceCheck(x, y + 1)) || (p->boulderCheck(up))) {
			alive = false;
			return;
		}
		else
			moveTo(x, y + 1);
	}
	else if(getDirection() == down) {
		if((p->getWorld()->iceCheck(x, y - 1)) || (p->boulderCheck(down))) {
			alive = false;
			return;
		}
		else
			moveTo(x, y - 1);
	}

	vector<Protestor*> vp = p->getWorld()->getProtestors();
	bool hit = false;

	for(auto it = vp.begin(); it != vp.end(); it++) {
		Protestor* pr = (*it);
		double distance = p->getWorld()->distanceCheck(getX(), getY(), (*it)->getX(), (*it)->getY());


		if(distance <= 3) {
			(pr)->annoyed(p, 2);
			hit = true;
		}
	}
	if(hit) {
		alive = false;
	}

	++travel;
}

void OilBarrel::doSomething(Iceman* p) {
	if (!alive) {
		return;
	}
	double distance = p->getWorld()->distanceCheck(getX(), getY(), p->getX(), p->getY());
	if(!isVisible() && (distance <= 4.0)) {
		setVisible(true);
		return;
	}
	else if(distance <= 3.0) {
		alive = false;
		p->getWorld()->playSound(SOUND_FOUND_OIL);
		p->getWorld()->increaseScore(score);
		p->setBarrels((p->getBarrels()) - 1);
	}
}

void GoldNugget::doSomething(Iceman* p) {
	if(!alive) {
		return;
	}

	double distance = p->getWorld()->distanceCheck(getX(), getY(), p->getX(), p->getY());
	ticks++;

	if(!isVisible() && (distance <= 4.0)) {
		setVisible(true);
		return;
	}
	else if((distance <= 3.0) && state == permanent) {
		alive = false;
		p->getWorld()->playSound(SOUND_GOT_GOODIE);
		p->getWorld()->increaseScore(10);
		p->addGold();
	}

	if(state == temporary) {
		if(ticks == 100) {
			alive = false;
		}
		for(auto n : p->getWorld()->getProtestors()) {
			double d = p->getWorld()->distanceCheck(getX(), getY(), n->getX(), n->getY());
			if(d <= 3) {
				alive = false;
				n->foundGold(p);
				break;
			}
		}
	}

}

void SonarKit::doSomething(Iceman* p) {
	if (!alive) {
		return;
	}
	ticks++;

	int lifespan = max(100, int(300 - 10 * p->getWorld()->getLevel()));
	double distance = p->getWorld()->distanceCheck(getX(), getY(), p->getX(), p->getY());

	if (distance <= 3.0) {
		alive = false;
		p->getWorld()->playSound(SOUND_GOT_GOODIE);
		p->addSonar();
		p->getWorld()->increaseScore(75);
	}

	if (ticks == lifespan) {
		alive = false;
	}
}

void WaterPool::doSomething(Iceman* p) {
	if (!alive) {
		return;
	}

	ticks++;

	int lifespan = max(100, int(300 - 10 * p->getWorld()->getLevel()));
	double distance = p->getWorld()->distanceCheck(getX(), getY(), p->getX(), p->getY());

	if (distance <= 3.0) {
		alive = false;
		p->getWorld()->playSound(SOUND_GOT_GOODIE);
		p->addSquirt();
		p->getWorld()->increaseScore(100);
	}

	if (ticks == lifespan) {
		alive = false;
	}
}

//checks if the path is clear for the protestor to move
bool Protestor::pathCheck(Iceman* p, int X, int Y) {
	vector<ActivatingObject*> c = p->getWorld()->getActivatingObjects();

	for(auto a : c){
		if (a->isBoulder()){
			double distance = p->getWorld()->distanceCheck(X, Y, a->getX(), a->getY());
			if (distance <= 3) {
				return true;
			}
		}
	}

	if(p->getWorld()->iceCheck(X, Y)){
		return true;
	}

	return false;
}

//how the protestor reacts when annoyed and if it damages the iceman
void Protestor::annoyed(Iceman* p, int damage){
	if(leaving){
		return;
	}

	health -= damage;

	if (health > 0){
		p->getWorld()->playSound(SOUND_PROTESTER_ANNOYED);
		int N = max(50, int(100 - p->getWorld()->getLevel() * 10));

		restTicks -= N;
	}
	else {
		leaving = true;
		p->getWorld()->playSound(SOUND_PROTESTER_GIVE_UP);
		restTicks = 500;
		if(damage == 100) {
			p->getWorld()->increaseScore(500);
		}
		else{
			p->getWorld()->increaseScore(100);
		}
	}
}

void Protestor::foundGold(Iceman* p) {
	p->getWorld()->playSound(SOUND_PROTESTER_FOUND_GOLD);
	p->getWorld()->increaseScore(25);
	leaving = true;
}

void Protestor::leave(Iceman* p) {
	struct Coord {
		int x, y;
		Coord() : x(0), y(0) {}
		Coord(int X, int Y) {
			x = X;
			y = Y;
		}
	};

	vector<vector<Coord>> prev(61, vector<Coord>(61));
	set<pair<int, int>> visited;
	queue<Coord> q;

	q.push(Coord(getX(), getY()));
	visited.insert({ getX(), getY() });

	int X[4] = { -1, 1, 0, 0 };
	int Y[4] = { 0, 0, 1, -1 };

	while(!q.empty()) {
		Coord current = q.front();
		q.pop();

		if (current.x == 60 && current.y == 60) {
			while(current.x != getX() || current.y != getY()) {
				path.push_back({ current.x, current.y });
				current = prev[current.x][current.y];
			}
			return;
		}

		for(int i = 0; i < 4; i++){
			int nextX = current.x + X[i];
			int nextY = current.y + Y[i];

			if(visited.count({ nextX, nextY }) != 0){
				continue;
			}

			if(pathCheck(p, nextX, nextY)){
				continue;
			}

			if(nextX > 60 || nextX < 0 || nextY > 60 || nextY < 0){
				continue;
			}

			q.push(Coord(nextX, nextY));
			prev[nextX][nextY] = current;
			visited.insert({ nextX, nextY });
		}
	}
}

void Protestor::doSomething(Iceman* p) {
	if(!alive) {
		return;
	}

	int waitTicks = max(0, int(3 - p->getWorld()->getLevel() / 4));

	if(restTicks < waitTicks){
		restTicks++;
		return;
	}

	restTicks = 0;

	int tX = p->getX();
	int tY = p->getY();

	if (leaving) {
		if (getX() == 60 && getY() == 60) {
			alive = false;
			p->getWorld()->decProtestors();
			return;
		}

		if(path.empty()) {
			leave(p);
			return;
		}

		vector<int> coords = path.back();
		path.pop_back();

		int x = coords[0];
		int y = coords[1];

		if (x > getX()) {
			setDirection(right);
		}
		else if (x < getX()) {
			setDirection(left);
		}
		else if (y > getY()) {
			setDirection(up);
		}
		else if (y < getY()) {
			setDirection(down);
		}
		moveTo(x, y);

		return;
	}

	shoutTicks++;
	perpTicks++;
	if (p->getWorld()->distanceCheck(getX(), getY(), tX, tY) <= 4 && (shoutTicks >= 15)) {
		if (getDirection() == right && ((tX - getX()) > 0)) {
			p->getWorld()->playSound(SOUND_PROTESTER_YELL);
			shoutTicks = 0;
			p->annoy(2);
			return;
		}
		else if (getDirection() == left && ((getX() - tX) > 0)) {
			p->getWorld()->playSound(SOUND_PROTESTER_YELL);
			shoutTicks = 0;
			p->annoy(2);
			return;
		}
		else if (getDirection() == up && ((tY - getY()) > 0)) {
			p->getWorld()->playSound(SOUND_PROTESTER_YELL);
			shoutTicks = 0;
			p->annoy(2);
			return;
		}
		else if (getDirection() == down && ((getY() - tY) > 0)) {
			p->getWorld()->playSound(SOUND_PROTESTER_YELL);
			shoutTicks = 0;
			p->annoy(2);
			return;
		}
	}

	if ((tX == getX()) || (tY == getY())) {
		vector<ActivatingObject*> c = p->getWorld()->getActivatingObjects();
		vector<Ice*> i = p->getWorld()->getIce();
		bool canWalk = true;
		bool isX = tX == getX();

		if (p->getWorld()->distanceCheck(getX(), getY(), tX, tY) > 4) {
			if (isX) {
				int dY = (tY > getY()) ? 1 : -1;
				for (int b = getY() + dY; b != tY + dY; b += dY) {
					if (pathCheck(p, getX(), b)) {
						canWalk = false;
					}
				}
			}
			else {
				int dX = (tX > getX()) ? 1 : -1;
				for (int b = getX() + dX; b != tX + dX; b += dX) {
					if (pathCheck(p, b, getY())) {
						canWalk = false;
					}
				}
			}

			if (canWalk) {
				if (isX && (p->getY() > getY())) {
					setDirection(up);
					moveTo(getX(), getY() + 1);
				}
				else if (isX && (p->getY() < getY())) {
					setDirection(down);
					moveTo(getX(), getY() - 1);
				}
				else if (!isX && (p->getX() > getX())) {
					setDirection(right);
					moveTo(getX() + 1, getY());
				}
				else if (!isX && (p->getX() < getX())) {
					setDirection(left);
					moveTo(getX() - 1, getY());
				}
				numSquares = 0;
				return;
			}
		}
	}
	numSquares--;
	if (numSquares <= 0) {
		int randDir = rand() % 4;

		while (true) {
			if (randDir == 0) {
				if (pathCheck(p, getX() - 1, getY())) {
					randDir = rand() % 4;
					continue;
				}
				setDirection(left);
				break;
			}
			else if (randDir == 1) {
				if (pathCheck(p, getX() + 1, getY())) {
					randDir = rand() % 4;
					continue;
				}
				setDirection(right);
				break;
			}
			else if (randDir == 2) {
				if (pathCheck(p, getX(), getY() - 1)) {
					randDir = rand() % 4;
					continue;
				}
				setDirection(down);
				break;
			}
			else if (randDir == 3) {
				if (pathCheck(p, getX(), getY() + 1)) {
					randDir = rand() % 4;
					continue;
				}
				setDirection(up);
				break;
			}
		}
		numSquares = (rand() % 53) + 8;
	}

	if (perpTicks >= 200) {
		vector<Direction> dir;

		if (getDirection() == left || getDirection() == right) {
			if (pathCheck(p, getX(), getY() + 1)) {
				dir.push_back(up);
			}
			if (pathCheck(p, getX(), getY() - 1)) {
				dir.push_back(down);
			}
		}
		else if (getDirection() == up || getDirection() == down) {
			if (pathCheck(p, getX() + 1, getY())) {
				dir.push_back(right);
			}
			if (pathCheck(p, getX() - 1, getY())) {
				dir.push_back(left);
			}
		}

		if (!dir.empty()) {
			setDirection(dir[(rand() % dir.size())]);
		}
		numSquares = (rand() % 53) + 8;
		perpTicks = 0;
	}

	if (getDirection() == left) {
		if (pathCheck(p, getX() - 1, getY()) || getX() <= 0) {
			numSquares = 0;
		}
		else {
			moveTo(getX() - 1, getY());
		}
	}
	else if (getDirection() == right) {
		if (pathCheck(p, getX() + 1, getY()) || getX() >= 60) {
			numSquares = 0;
		}
		else {
			moveTo(getX() + 1, getY());
		}

	}
	else if (getDirection() == down) {
		if (pathCheck(p, getX(), getY() - 1) || getY() <= 0) {
			numSquares = 0;
		}
		else {
			moveTo(getX(), getY() - 1);
		}
	}
	else if (getDirection() == up) {
		if (pathCheck(p, getX(), getY() + 1) || getY() >= 60) {
			numSquares = 0;
		}
		else {
			moveTo(getX(), getY() + 1);
		}
	}
}

void HardcoreProtestor::findPath(Iceman* p, bool isLeaving) {

	vector<vector<Coord>> prev(61, vector<Coord>(61));
	queue<Coord> q;

	int finalX = isLeaving ? 60 : p->getX();
	int finalY = isLeaving ? 60 : p->getY();

	int M = 16 + p->getWorld()->getLevel() * 2;

	q.push(Coord(getX(), getY()));
	visited.insert({ getX(), getY() });

	int X[4] = { -1, 1, 0, 0 };
	int Y[4] = { 0, 0, 1, -1 };


	while (!q.empty()) {
		Coord current = q.front();
		q.pop();

		if (current.x == finalX && current.y == finalY) {
			while (current.x != getX() || current.y != getY()) {
				path.push_back({ current.x, current.y });
				current = prev[current.x][current.y];
			}
			visited.clear();
			return;
		}

		for (int i = 0; i < 4; i++) {
			int nextX = current.x + X[i];
			int nextY = current.y + Y[i];

			if (visited.count({ nextX, nextY }) != 0) {
				continue;
			}

			if (pathCheck(p, nextX, nextY)) {
				continue;
			}

			if (nextX > 60 || nextX < 0 || nextY > 60 || nextY < 0) {
				continue;
			}
			if (!isLeaving && (current.distance + 1 > M)) {
				continue;
			}
			q.push(Coord(nextX, nextY, current.distance + 1));
			prev[nextX][nextY] = current;
			visited.insert({ nextX, nextY });
		}
	}
	visited.clear();
}

void HardcoreProtestor::doSomething(Iceman* p) {
	if (!alive) {
		return;
	}


	int waitTicks = max(0, int(3 - p->getWorld()->getLevel() / 4));

	if (restTicks < waitTicks) {
		restTicks++;
		return;
	}

	restTicks = 0;

	int tX = p->getX();
	int tY = p->getY();

	// 3
	if (leaving) {
		if (getX() == 60 && getY() == 60) {
			alive = false;
			p->getWorld()->decProtestors();
			return;
		}

		if (path.empty()) {
			findPath(p, true);
			return;
		}

		vector<int> coords = path.back();
		path.pop_back();

		int x = coords[0];
		int y = coords[1];

		if (x > getX()) {
			setDirection(right);
		}
		else if (x < getX()) {
			setDirection(left);
		}
		else if (y > getY()) {
			setDirection(up);
		}
		else if (y < getY()) {
			setDirection(down);
		}

		moveTo(x, y);

		return;
	}

	shoutTicks++;
	perpTicks++;
	if (p->getWorld()->distanceCheck(getX(), getY(), tX, tY) <= 4 && (shoutTicks >= 15)) {
		if (getDirection() == right && ((tX - getX()) > 0)) {
			p->getWorld()->playSound(SOUND_PROTESTER_YELL);
			shoutTicks = 0;
			p->annoy(2);
			return;
		}
		else if (getDirection() == left && ((getX() - tX) > 0)) {
			p->getWorld()->playSound(SOUND_PROTESTER_YELL);
			shoutTicks = 0;
			p->annoy(2);
			return;
		}
		else if (getDirection() == up && ((tY - getY()) > 0)) {
			p->getWorld()->playSound(SOUND_PROTESTER_YELL);
			shoutTicks = 0;
			p->annoy(2);
			return;
		}
		else if (getDirection() == down && ((getY() - tY) > 0)) {
			p->getWorld()->playSound(SOUND_PROTESTER_YELL);
			shoutTicks = 0;
			p->annoy(2);
			return;
		}
	} 

	if (p->getWorld()->distanceCheck(getX(), getY(), tX, tY) > 4) {
		int M = 16 + p->getWorld()->getLevel() * 2;

		path.clear();
		findPath(p, false);
		if (path.size() <= M && !path.empty()) {
			vector<int> coords = path.back();
			path.pop_back();

			int x = coords[0];
			int y = coords[1];

			if (x > getX()) {
				setDirection(right);
			}
			else if (x < getX()) {
				setDirection(left);
			}
			else if (y > getY()) {
				setDirection(up);
			}
			else if (y < getY()) {
				setDirection(down);
			}

			moveTo(x, y);

			path.clear();
			return;
		}
	}

	// 6
	if ((tX == getX()) || (tY == getY())) {
		vector<ActivatingObject*> c = p->getWorld()->getActivatingObjects();
		vector<Ice*> e = p->getWorld()->getIce();
		bool canWalk = true;
		bool isX = tX == getX();

		if (p->getWorld()->distanceCheck(getX(), getY(), tX, tY) > 4) {
			if (isX) {
				int dY = (tY > getY()) ? 1 : -1;
				for (int b = getY() + dY; b != tY + dY; b += dY) {
					if (pathCheck(p, getX(), b)) {
						canWalk = false;
						break;
					}
				}
			}
			else {
				int dX = (tX > getX()) ? 1 : -1;
				for (int b = getX() + dX; b != tX + dX; b += dX) {
					if (pathCheck(p, b, getY())) {
						canWalk = false;
						break;
					}
				}
			}

			if (canWalk) {
				if (isX && (p->getY() > getY())) {
					setDirection(up);
					moveTo(getX(), getY() + 1);
				}
				else if (isX && (p->getY() < getY())) {
					setDirection(down);
					moveTo(getX(), getY() - 1);
				}
				else if (!isX && (p->getX() > getX())) {
					setDirection(right);
					moveTo(getX() + 1, getY());
				}
				else if (!isX && (p->getX() < getX())) {
					setDirection(left);
					moveTo(getX() - 1, getY());
				}
				numSquares = 0;
				return;
			}
		}
	}

	numSquares--;
	if (numSquares <= 0) {
		int randDir = rand() % 4;

		while (true) {
			if (randDir == 0) {
				if (pathCheck(p, getX() - 1, getY())) {
					randDir = rand() % 4;
					continue;
				}
				setDirection(left);
				break;
			}
			else if (randDir == 1) {
				if (pathCheck(p, getX() + 1, getY())) {
					randDir = rand() % 4;
					continue;
				}
				setDirection(right);
				break;
			}
			else if (randDir == 2) {
				if (pathCheck(p, getX(), getY() - 1)) {
					randDir = rand() % 4;
					continue;
				}
				setDirection(down);
				break;
			}
			else if (randDir == 3) {
				if (pathCheck(p, getX(), getY() + 1)) {
					randDir = rand() % 4;
					continue;
				}
				setDirection(up);
				break;
			}
		}
		numSquares = (rand() % 53) + 8;
	}

	if (perpTicks >= 200) {
		vector<Direction> dir;

		if (getDirection() == left || getDirection() == right) {
			if (pathCheck(p, getX(), getY() + 1)) {
				dir.push_back(up);
			}
			if (pathCheck(p, getX(), getY() - 1)) {
				dir.push_back(down);
			}
		}
		else if (getDirection() == up || getDirection() == down) {
			if (pathCheck(p, getX() + 1, getY())) {
				dir.push_back(right);
			}
			if (pathCheck(p, getX() - 1, getY())) {
				dir.push_back(left);
			}
		}

		if (!dir.empty()) {
			setDirection(dir[(rand() % dir.size())]);
		}
		numSquares = (rand() % 53) + 8;
		perpTicks = 0;
	}

	if (getDirection() == left) {
		if (pathCheck(p, getX() - 1, getY()) || getX() <= 0) {
			numSquares = 0;
		}
		else {
			moveTo(getX() - 1, getY());
		}
	}
	else if (getDirection() == right) {
		if (pathCheck(p, getX() + 1, getY()) || getX() >= 60) {
			numSquares = 0;
		}
		else {
			moveTo(getX() + 1, getY());
		}

	}
	else if (getDirection() == down) {
		if (pathCheck(p, getX(), getY() - 1) || getY() <= 0) {
			numSquares = 0;
		}
		else {
			moveTo(getX(), getY() - 1);
		}
	}
	else if (getDirection() == up) {
		if (pathCheck(p, getX(), getY() + 1) || getY() >= 60) {
			numSquares = 0;
		}
		else {
			moveTo(getX(), getY() + 1);
		}
	}

}

void HardcoreProtestor::annoyed(Iceman* p, int damage) {
	if (leaving) {
		return;
	}

	health -= damage;

	if (health > 0) {
		p->getWorld()->playSound(SOUND_PROTESTER_ANNOYED);
		int N = max(50, int(100 - p->getWorld()->getLevel() * 10));

		restTicks -= N;
	}
	else {
		leaving = true;
		p->getWorld()->playSound(SOUND_PROTESTER_GIVE_UP);
		restTicks = 500;
		if (damage == 100) {
			p->getWorld()->increaseScore(500);
		}
		else {
			p->getWorld()->increaseScore(100);
		}
	}

}

void HardcoreProtestor::foundGold(Iceman* p) {

	int ticks_to_start = max(50, int(100 - p->getWorld()->getLevel() * 10));

	p->getWorld()->playSound(SOUND_PROTESTER_FOUND_GOLD);
	p->getWorld()->increaseScore(25);
	restTicks -= ticks_to_start;

}
