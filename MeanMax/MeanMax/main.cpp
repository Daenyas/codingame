#include <iostream>
#include <string>
#include <vector>
#include <math.h>

using namespace std;

/**
* League : Wood 1
* Rank : Top 1
**/

/* Constants */
// Math
#define EPSILON 0.00001

// Board
#define PLAYERS_COUNT 3
#define MAP_RADIUS 6000

// Units
#define LOOTER_RADIUS 400.0

#define REAPER_UNITID 0
#define REAPER_MASS 0.5
#define REAPER_FRICTION 0.20
#define REAPER_SKILL_DURATION 3
#define REAPER_SKILL_COST 30
#define REAPER_SKILL_ORDER 0
#define REAPER_SKILL_RANGE 2000.0
#define REAPER_SKILL_RADIUS 1000.0
#define REAPER_SKILL_MASS_BONUS 10.0

#define DESTROYER_UNITID 1
#define DESTROYER_MASS 1.5
#define DESTROYER_FRICTION 0.30
#define DESTROYER_SKILL_DURATION 1
#define DESTROYER_SKILL_COST 60
#define DESTROYER_SKILL_ORDER 2
#define DESTROYER_SKILL_RANGE 2000.0
#define DESTROYER_SKILL_RADIUS 1000.0
#define DESTROYER_NITRO_GRENADE_POWER 1000

#define DOOF_UNITID 2
#define DOOF_MASS 1.0
#define DOOF_FRICTION 0.25
#define DOOF_RAGE_COEF 0.01
#define DOOF_SKILL_DURATION 3
#define DOOF_SKILL_COST 30
#define DOOF_SKILL_ORDER 1
#define DOOF_SKILL_RANGE 2000.0
#define DOOF_SKILL_RADIUS 1000.0

#define TANKER_UNITID 3
#define TANKER_EMPTY_MASS 2.5
#define TANKER_MASS_BY_WATER 0.5
#define TANKER_FRICTION 0.40
#define TANKER_RADIUS_BASE 400.0
#define TANKER_RADIUS_BY_SIZE 50.0
#define TANKER_MIN_SIZE 4

#define WRECK_UNITID 4

#define REAPER_SKILL_EFFECT 5
#define DOOF_SKILL_EFFECT 6
#define DESTROYER_SKILL_EFFECT 7

#define MAX_THRUST 300
#define MAX_RAGE 300
#define WIN_SCORE 50

/* Board classes */
class Board;
class Unit;

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

class SkillEffect : public Point
{
public:
	int id;
	int type;
	double radius;
	int duration;
	int order;

protected:
	SkillEffect(int id, double x, double y, int type, double radius, int duration, int order) : Point(x, y), id(id), type(type), radius(radius), duration(duration), order(order)
	{
	}

	SkillEffect(const SkillEffect& other) : Point(other), id(other.id), type(other.type), radius(other.radius), duration(other.duration), order(other.order)
	{
	}

	SkillEffect& operator=(const SkillEffect& other)
	{
		Point::operator=(other);

		this->id = other.id;
		this->type = other.type;
		this->radius = other.radius;
		this->duration = other.duration;
		this->order = other.order;
	}

public:
	void apply(Board& board);

protected:
	virtual void apply(Unit* unit) = 0;
};

class ReaperSkillEffect : public SkillEffect
{
public:
	ReaperSkillEffect(int id, double x, double y) : SkillEffect(id, x, y, REAPER_SKILL_EFFECT, REAPER_SKILL_RADIUS, REAPER_SKILL_DURATION, REAPER_SKILL_ORDER)
	{
	}

	ReaperSkillEffect(const ReaperSkillEffect& other) : SkillEffect(other.id, other.x, other.y, REAPER_SKILL_EFFECT, REAPER_SKILL_RADIUS, REAPER_SKILL_DURATION, REAPER_SKILL_ORDER)
	{
	}

	ReaperSkillEffect& operator=(const ReaperSkillEffect& other)
	{
		SkillEffect::operator=(other);
	}

protected:
	void apply(Unit* unit);
};

class DoofSkillEffect : public SkillEffect
{
public:
	DoofSkillEffect(int id, double x, double y) : SkillEffect(id, x, y, DOOF_SKILL_EFFECT, DOOF_SKILL_RADIUS, DOOF_SKILL_DURATION, DOOF_SKILL_ORDER)
	{
	}

	DoofSkillEffect(const DoofSkillEffect& other) : SkillEffect(other.id, other.x, other.y, DOOF_SKILL_EFFECT, DOOF_SKILL_RADIUS, DOOF_SKILL_DURATION, DOOF_SKILL_ORDER)
	{
	}

	DoofSkillEffect& operator=(const DoofSkillEffect& other)
	{
		SkillEffect::operator=(other);
	}

protected:
	void apply(Unit* unit)
	{
		// Nothing to do now
	}
};

class DestroyerSkillEffect : public SkillEffect
{
public:
	DestroyerSkillEffect(int id, double x, double y) : SkillEffect(id, x, y, DESTROYER_SKILL_EFFECT, DESTROYER_SKILL_RADIUS, DESTROYER_SKILL_DURATION, DESTROYER_SKILL_ORDER)
	{
	}

	DestroyerSkillEffect(const DestroyerSkillEffect& other) : SkillEffect(other.id, other.x, other.y, DESTROYER_SKILL_EFFECT, DESTROYER_SKILL_RADIUS, DESTROYER_SKILL_DURATION, DESTROYER_SKILL_ORDER)
	{
	}

	DestroyerSkillEffect& operator=(const DestroyerSkillEffect& other)
	{
		SkillEffect::operator=(other);
	}

protected:
	void apply(Unit* unit);
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

class Tanker : public Unit
{
public:
	int water;
	int size;

	Tanker(int id, double x, double y, double vx, double vy, int water, int size) :
		Unit(TANKER_UNITID, TANKER_RADIUS_BASE + TANKER_RADIUS_BY_SIZE * size, TANKER_EMPTY_MASS + TANKER_MASS_BY_WATER * water, TANKER_FRICTION, id, x, y, vx, vy), water(water), size(size)
	{
	}

	Tanker(const Tanker& other) : Unit(other), water(other.water), size(other.size)
	{
	}

	Tanker& operator=(const Tanker& other)
	{
		Unit::operator=(other);
		this->water = water;
		this->size = size;
		return *this;
	}
};

class Looter : public Unit
{
public:
	int skillCost;
	double skillRange;

protected:
	Looter(int type, double mass, double friction, int skillCost, double skillRange, int id, double x, double y, double vx, double vy) :
		Unit(type, LOOTER_RADIUS, mass, friction, id, x, y, vx, vy), skillCost(skillCost), skillRange(skillRange)
	{
	}

	Looter(const Looter& other) : Unit(other), skillCost(other.skillCost), skillRange(other.skillRange)
	{
	}

	Looter& operator=(const Looter& other)
	{
		Unit::operator=(other);

		this->skillCost = other.skillCost;
		this->skillRange = other.skillRange;
		return *this;
	}

public:
	virtual SkillEffect* skill(const Point& point) = 0;
};

class Reaper : public Looter
{
public:
	Reaper(int id, double x, double y, double vx, double vy) : Looter(REAPER_UNITID, REAPER_MASS, REAPER_FRICTION, REAPER_SKILL_COST, REAPER_SKILL_RANGE, id, x, y, vx, vy)
	{
	}

	Reaper(const Reaper& other) : Looter(other)
	{
	}

	Reaper& operator=(const Reaper& other)
	{
		Looter::operator=(other);

		return *this;
	}

	virtual SkillEffect* skill(const Point& point)
	{
		return new ReaperSkillEffect(0, point.x, point.y);
	}
};

class Destroyer : public Looter
{
public:
	Destroyer(int id, double x, double y, double vx, double vy) : Looter(DESTROYER_UNITID, DESTROYER_MASS, DESTROYER_FRICTION, DESTROYER_SKILL_COST, DESTROYER_SKILL_RANGE, id, x, y, vx, vy)
	{
	}

	Destroyer(const Destroyer& other) : Looter(other)
	{
	}

	Destroyer& operator=(const Destroyer& other)
	{
		Looter::operator=(other);

		return *this;
	}

	virtual SkillEffect* skill(const Point& point)
	{
		return new DestroyerSkillEffect(0, point.x, point.y);
	}
};

class Doof : public Looter
{
public:
	Doof(int id, double x, double y, double vx, double vy) : Looter(DOOF_UNITID, DOOF_MASS, DOOF_FRICTION, DOOF_SKILL_COST, DOOF_SKILL_RANGE, id, x, y, vx, vy)
	{
	}

	Doof(const Doof& other) : Looter(other)
	{
	}

	Doof& operator=(const Doof& other)
	{
		Looter::operator=(other);

		return *this;
	}

	virtual SkillEffect* skill(const Point& point)
	{
		return new DoofSkillEffect(0, point.x, point.y);
	}
};

class Player
{
public:
	int index;
	int score;
	int rage;

	Reaper* reaper;
	Destroyer* destroyer;
	Doof* doof;


	Player(int index, int score, int rage) : index(index), score(score), rage(rage)
	{
	}

	Player(const Player& other) : index(other.index), score(other.score), rage(other.rage)
	{
		if (other.reaper)
		{
			this->reaper = new Reaper(*other.reaper);
		}
		if (other.destroyer)
		{
			this->destroyer = new Destroyer(*other.destroyer);
		}
		if (other.doof)
		{
			this->doof = new Doof(*other.doof);
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
		if (other.destroyer)
		{
			this->destroyer = new Destroyer(*other.destroyer);
		}
		if (other.doof)
		{
			this->doof = new Doof(*other.doof);
		}
	}

	~Player()
	{
		delete this->reaper;
		delete this->destroyer;
		delete this->doof;
	}
};

class Board
{
public:
	Player* players[PLAYERS_COUNT];
	vector<Tanker> tankers;
	vector<Wreck> wrecks;
	vector<Unit*> units;

	Board()
	{
	}

	Board(const Board& other) : tankers(other.tankers), wrecks(other.wrecks)
	{
		for (int i = 0; i < PLAYERS_COUNT; ++i)
		{
			if (other.players[i])
			{
				this->players[i] = new Player(*(other.players[i]));
			}
		}

		this->linkUnits();
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

		this->tankers = other.tankers;
		this->wrecks = other.wrecks;

		this->linkUnits();
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
			delete this->players[i]->destroyer;
			delete this->players[i]->doof;
		}

		this->tankers.clear();
		this->wrecks.clear();
	}

	void linkUnits()
	{
		this->units.clear();

		for (int i = 0; i < PLAYERS_COUNT; ++i)
		{
			this->units.push_back(this->players[i]->reaper);
			this->units.push_back(this->players[i]->destroyer);
			this->units.push_back(this->players[i]->doof);
		}

		for (auto it = this->tankers.begin(); it != this->tankers.end(); ++it)
		{
			this->units.push_back(&(*it));
		}
	}
};

void SkillEffect::apply(Board& board)
{
	this->duration -= 1;
	for (auto it = board.units.begin(); it != board.units.end(); ++it)
	{
		if (this->isInRange(**it, this->radius + (*it)->radius))
		{
			apply(*it);
		}
	}
}

void ReaperSkillEffect::apply(Unit* unit)
{
	// Increase mass
	unit->mass += REAPER_SKILL_MASS_BONUS;
}

void DestroyerSkillEffect::apply(Unit* unit)
{
	// Push units back
	unit->thrust(*this, -DESTROYER_NITRO_GRENADE_POWER);
}

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
		case REAPER_UNITID:
			board.players[player]->reaper = new Reaper(unitId, x, y, vx, vy);
			break;
		case DESTROYER_UNITID:
			board.players[player]->destroyer = new Destroyer(unitId, x, y, vx, vy);
			break;
		case DOOF_UNITID:
			board.players[player]->doof = new Doof(unitId, x, y, vx, vy);
			break;
		case TANKER_UNITID:
			board.tankers.push_back(Tanker(unitId, x, y, vx, vy, extra, extra2));
			break;
		case WRECK_UNITID:
			board.wrecks.push_back(Wreck(unitId, x, y, extra, radius));
			break;
		default:
			throw string("Unit Type not handled");
		}
	}

	board.linkUnits();
}

void strategy1(const Board& board)
{
	// My units
	auto myReaper = board.players[0]->reaper;
	auto myDestroyer = board.players[0]->destroyer;
	auto myDoof = board.players[0]->doof;

	// Go to closest Wreck
	const Wreck* reaperTarget = nullptr;
	double minDistance2Reaper = (MAP_RADIUS * 2) * (MAP_RADIUS * 2);
	for (auto it = board.wrecks.begin(); it != board.wrecks.end(); ++it)
	{
		double distance2 = myReaper->distance2(*it);
		if (distance2 < minDistance2Reaper)
		{
			reaperTarget = &(*it);
			minDistance2Reaper = distance2;
		}
	}

	// Go to closest Tanker
	const Tanker* destroyerTarget = nullptr;
	double minDistance2Destroyer = (MAP_RADIUS * 2) * (MAP_RADIUS * 2);
	for (auto it = board.tankers.begin(); it != board.tankers.end(); ++it)
	{
		double distance2 = myDestroyer->distance2(*it);
		if (distance2 < minDistance2Destroyer)
		{
			destroyerTarget = &(*it);
			minDistance2Destroyer = distance2;
		}
	}

	// Go to strongest ennemy reaper
	Player* bestEnnemy = board.players[1]->score > board.players[2]->score ? board.players[1] : board.players[2];	

	// Output Reaper
	if (!reaperTarget) // Anticipate and follow destroyer
	{
		cout << destroyerTarget->x << " " << destroyerTarget->y << " " << MAX_THRUST << endl;
	}
	else if (myReaper->isInRange(*reaperTarget, reaperTarget->radius)) // Stay in
	{
		cout << "WAIT" << endl;
	}
	else // GO GO GO
	{
		cout << reaperTarget->x << " " << reaperTarget->y << " " << MAX_THRUST << endl;
	}

	// Output Destroyer
	if (destroyerTarget) // GO GO GO
	{
		cout << destroyerTarget->x << " " << destroyerTarget->y << " " << MAX_THRUST << endl;
	}
	else // Should not happen
	{
		cout << "WAIT" << endl;
	}

	// Output Doof		
	cout << bestEnnemy->reaper->x << " " << bestEnnemy->reaper->y << " " << MAX_THRUST << endl;
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
		strategy1(board);
	}
}