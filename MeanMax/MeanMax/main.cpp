#include <iostream>
#include <string>
#include <vector>
#include <math.h>

using namespace std;

/**
* League : Wood 3
* Rank : 1
**/

/* Constants */
// Math
#define EPSILON 0.00001

// Board
#define PLAYERS_COUNT 3
#define MAP_RADIUS 6000

// Units
#define UNITTYPE_WRECK 4
#define LOOTER_RADIUS 400.0

#define UNITTYPE_REAPER 0
#define MASS_REAPER 0.5
#define FRICTION_REAPER 0.20

/* Board classes */
class Point 
{
public:
	double x;
	double y;

public:
	Point(double x, double y) : x(x), y(y)
	{
	}

	Point(const Point& other) : x(other.x), y(other.y)
	{
	}

	Point& operator=(const Point& other)
	{
		this->x = other.x;
		this->y = other.y;
	}

	double distance(const Point& other) const
	{
		return sqrt((this->x - other.x) * (this->x - other.x) + (this->y - other.y) * (this->y - other.y));
	}

	// Squared distance is faster to compute when we want to find a closer point
	double distance2(const Point& other) const
	{
		return (this->x - other.x) * (this->x - other.x) + (this->y - other.y) * (this->y - other.y);
	}

	// Move the point to x and y
	void move(double x, double y)
	{
		this->x = x;
		this->y = y;
	}

	// Move the point to an other point for a given distance
	void moveTo(const Point& direction, double distance)
	{
		double d = this->distance(direction);

		if (d < EPSILON) 
		{
			return;
		}

		double dx = direction.x - x;
		double dy = direction.y - y;
		double coef = distance / d;

		this->x += dx * coef;
		this->y += dy * coef;
	}

	bool isInRange(const Point& center, double range) const
	{
		return center != *this && this->distance(center) <= range;
	}	

	// Operators overloading
	bool operator==(const Point& other) const
	{
		return fabs(this->x - other.x) < EPSILON && fabs(this->y - other.y) < EPSILON;
	}

	bool operator!=(const Point& other) const
	{
		return !(*this == other);
	}
};

class Wreck : public Point
{
public:
	int id;
	int water;
	double radius;	

	Wreck(int id, double x, double y, int water, double radius) : Point(x, y), id(id), water(water), radius(radius)
	{
	}

	Wreck(const Wreck& other) : Point(other), id(other.id), water(other.water), radius(other.radius)
	{
	}

	Wreck& operator=(const Wreck& other)
	{
		Point::operator=(other);

		this->id = other.id;
		this->water = other.water;
		this->radius = other.radius;
	}
};

class Unit : public Point
{
public:
	int type;
	double radius;
	double mass;
	double friction;

	int id;
	double vx;
	double vy;	

// Should not try to construct a Unit, abstract concept
protected:
	Unit(int type, double radius, double mass, double friction, int id, double x, double y, double vx, double vy) : 
		Point(x, y), type(type), radius(radius), mass(mass), friction(friction), id(id), vx(vx), vy(vy)
	{		
	}

	Unit(const Unit& other) : Point(other), type(other.type), radius(other.radius), mass(other.mass), friction(other.friction), id(other.id), vx(other.vx), vy(other.vy)
	{
	}
	
	Unit& operator=(const Unit& other)
	{
		Point::operator=(other);

		this->type = other.type;
		this->radius = other.radius;
		this->mass = other.mass;
		this->friction = other.friction;
		
		this->id = other.id;
		this->vx = other.vx;
		this->vy = other.vy;
	}

public:
	void move(double thrust) 
	{
		this->x += this->vx * thrust;
		this->y += this->vy * thrust;
	}

	double speed() const
	{
		return sqrt(vx * vx + vy * vy);
	}

	// Apply thrust on speed
	void thrust(const Point& direction, int power)
	{
		double distance = this->distance(direction);

		// Avoid a division by zero
		if (abs(distance) <= EPSILON) 
		{
			return;
		}

		double coef = (((double)power) / this->mass) / distance;
		this->vx += (direction.x - this->x) * coef;
		this->vy += (direction.y - this->y) * coef;
	}
};

class Reaper : public Unit
{
public:
	Reaper(int id, double x, double y, double vx, double vy) : Unit(UNITTYPE_REAPER, LOOTER_RADIUS, MASS_REAPER, FRICTION_REAPER, id, x, y, vx, vy)
	{
	}	

	Reaper(const Reaper& other) : Unit(other)
	{
	}

	Reaper& operator=(const Reaper& other)
	{
		Unit::operator=(other);
		return *this;
	}
};


class Player
{
public:
	int index;
	int score;
	int rage;
	Reaper* reaper;

	Player(int index, int score, int rage) : index(index), score(score), rage(rage)
	{
	}

	Player(const Player& other) : index(other.index), score(other.score), rage(other.rage)
	{
		if (other.reaper)
		{
			this->reaper = new Reaper(*other.reaper);
		}
	}

	Player& operator=(const Player& other)
	{
		this->index = other.index;
		this->score = other.score;
		this->rage = other.rage;
		if (other.reaper)
		{
			this->reaper = new Reaper(*other.reaper);
		}
	}

	~Player()
	{
		delete this->reaper;
	}
};

class Board
{
public:
	Player* players[PLAYERS_COUNT];
	vector<Wreck> wrecks;
	
	Board()
	{
	}

	Board(const Board& other) : wrecks(other.wrecks)
	{
		for (int i = 0; i < PLAYERS_COUNT; ++i)
		{
			if (other.players[i])
			{
				this->players[i] = new Player(*(other.players[i]));
			}				
		}
	}

	Board& operator=(const Board& other)
	{
		for (int i = 0; i < PLAYERS_COUNT; ++i)
		{
			delete this->players[i];
			if (other.players[i])
			{
				this->players[i] = new Player(*(other.players[i]));
			}			
		}

		this->wrecks = other.wrecks;
	}

	~Board()
	{
		for (int i = 0; i < PLAYERS_COUNT; ++i)
		{
			delete this->players[i];
		}
	}

	void resetUnits()
	{
		for (int i = 0; i < PLAYERS_COUNT; ++i)
		{
			delete this->players[i]->reaper;
		}

		this->wrecks.clear();
	}
};

/* Game functions */
void readInputs(Board& board, istream& stream)
{
	int score;
	for (int i = 0; i < PLAYERS_COUNT; ++i)
	{
		stream >> score; stream.ignore();
		board.players[i]->score = score;
	}

	int rage;
	for (int i = 0; i < PLAYERS_COUNT; ++i)
	{
		stream >> rage; stream.ignore();
		board.players[i]->rage = rage;
	}
	
	board.resetUnits();
	int unitCount;
	stream >> unitCount; stream.ignore();
	for (int i = 0; i < unitCount; i++)
	{
		int unitId;
		int unitType;
		int player;
		float mass;
		int radius;
		int x;
		int y;
		int vx;
		int vy;
		int extra;
		int extra2;
		stream >> unitId >> unitType >> player >> mass >> radius >> x >> y >> vx >> vy >> extra >> extra2; stream.ignore();

		switch (unitType)
		{
			case UNITTYPE_REAPER:
				board.players[player]->reaper = new Reaper(unitId, x, y, vx, vy);
				break;
			case UNITTYPE_WRECK:
				board.wrecks.push_back(Wreck(unitId, x, y, extra, radius));
				break;
			default:
				throw string("Unit Type not handled");
		}
	}
}

int main()
{
	// Init board
	Board board;
	for (int i = 0; i < PLAYERS_COUNT; ++i)
	{
		board.players[i] = new Player(i, 0, 0);
	}

	// Game loop
	while (1) {
		// Read inputs
		readInputs(board, cin);		

		// Decide action

		// Simple v1 just to test model : go to closest wreck
		auto myReaper = board.players[0]->reaper;
		Wreck* winner = nullptr;
		double minDistance2 = (MAP_RADIUS * 2) * (MAP_RADIUS * 2);
		for (auto it = board.wrecks.begin(); it != board.wrecks.end(); ++it)
		{
			double distance2 = myReaper->distance2(*it);
			if (distance2 < minDistance2)
			{
				winner = &(*it);
				minDistance2 = distance2;
			}
		}

		// Output
		if (myReaper->isInRange(*winner, winner->radius))
		{
			cout << "WAIT" << endl;
		}
		else
		{
			int acc = 300;
			cout << winner->x << " " << winner->y << " " << acc << endl;
		}
		
		// Not used yet
		cout << "WAIT" << endl;
		cout << "WAIT" << endl;
	}
}