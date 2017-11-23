#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <math.h>
#include <time.h>
#include <chrono>

// Compiler optimizations on CG
//#pragma GCC optimize "Ofast,omit-frame-pointer,inline,unroll-loops"

using namespace std;

/**
* League : Bronze
* Rank : 271
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
#define TANKER_THRUST 500
#define TANKER_SPAWN_RADIUS 8000.0
#define WATERTOWN_RADIUS 3000.0
#define WATERTOWN Point(0,0)

#define WRECK_UNITID 4

#define REAPER_SKILL_EFFECT 5
#define DOOF_SKILL_EFFECT 6
#define DESTROYER_SKILL_EFFECT 7

#define NULL_COLLISION Collision(1.0 + EPSILON)
#define IMPULSE_COEFF 0.5
#define MIN_IMPULSE 30.0

#define MAX_THRUST 300
#define MAX_RAGE 300
#define WIN_SCORE 50

// Actions
#define WAIT_ACTION 0
#define MOVE_ACTION 1
#define SKILL_ACTION 2

// Simulations
#define SIMULATIONS_NUMBER 200


/* Action class */
class Action
{
public:
	int type;
	int x;
	int y;
	int throttel;

	void print()
	{
		switch (this->type)
		{
			case WAIT_ACTION:
				cout << "WAIT" << endl;
				break;
			case MOVE_ACTION:
				cout << this->x << " " << this->y << " " << this->throttel << endl;
				break;
			case SKILL_ACTION:
				cout << "SKILL " << this->x << " " << this->y << endl;
				break;
			default:
				cout << "WAIT" << endl;
				cerr << "Unknown Action Type " << this->type << endl;
				break;
		}
	}
};

/* Board classes */
class Board;
class Unit;
class Player;
class Collision;

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

		return *this;
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

		return *this;
	}

public:
	void apply(Board& board);

protected:
	virtual void apply(Unit* unit) = 0;
};

class ReaperSkillEffect : public SkillEffect
{
public:	
	ReaperSkillEffect(int id, double x, double y, int duration) : SkillEffect(id, x, y, REAPER_SKILL_EFFECT, REAPER_SKILL_RADIUS, duration, REAPER_SKILL_ORDER)
	{
	}

	ReaperSkillEffect(const ReaperSkillEffect& other) : SkillEffect(other.id, other.x, other.y, other.id, other.radius, other.duration, other.order)
	{
	}

	ReaperSkillEffect& operator=(const ReaperSkillEffect& other)
	{
		SkillEffect::operator=(other);

		return *this;
	}

protected:
	void apply(Unit* unit);
};

class DoofSkillEffect : public SkillEffect
{
public:
	DoofSkillEffect(int id, double x, double y, int duration) : SkillEffect(id, x, y, DOOF_SKILL_EFFECT, DOOF_SKILL_RADIUS, duration, DOOF_SKILL_ORDER)
	{
	}

	DoofSkillEffect(const DoofSkillEffect& other) : SkillEffect(other.id, other.x, other.y, other.id, other.radius, other.duration, other.order)
	{
	}

	DoofSkillEffect& operator=(const DoofSkillEffect& other)
	{
		SkillEffect::operator=(other);

		return *this;
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
	DestroyerSkillEffect(int id, double x, double y, int duration) : SkillEffect(id, x, y, DESTROYER_SKILL_EFFECT, DESTROYER_SKILL_RADIUS, duration, DESTROYER_SKILL_ORDER)
	{
	}

	DestroyerSkillEffect(const DestroyerSkillEffect& other) : SkillEffect(other.id, other.x, other.y, other.id, other.radius, other.duration, other.order)
	{
	}

	DestroyerSkillEffect& operator=(const DestroyerSkillEffect& other)
	{
		SkillEffect::operator=(other);

		return *this;
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

		return *this;
	}

	// Reaper harvesting
	bool harvest(Player* players[PLAYERS_COUNT], const vector<SkillEffect* >& skillEffects);
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

		return *this;
	}

public:
	void move(double time)
	{
		this->x += this->vx * time;
		this->y += this->vy * time;
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
		if (fabs(distance) <= EPSILON)
		{
			return;
		}

		double coef = (((double)power) / this->mass) / distance;
		this->vx += (direction.x - this->x) * coef;
		this->vy += (direction.y - this->y) * coef;
	}

	bool isInDoofSkill(const vector<SkillEffect *>& skillEffects) const
	{
		for (auto it = skillEffects.begin(); it != skillEffects.end(); ++it)
		{
			if ((*it)->type == DOOF_SKILL_EFFECT && this->isInRange(**it, (*it)->radius + this->radius))
			{
				return true;
			}
		}
		
		return false;
	}

	void adjust(const vector<SkillEffect *>& skillEffects) 
	{
		this->x = round(this->x);
		this->y = round(this->y);
		
		if (this->isInDoofSkill(skillEffects)) 
		{
			// No friction if we are in a doof skill effect
			this->vx = round(this->vx);
			this->vy = round(this->vy);
		}
		else
		{
			this->vx = round(this->vx * (1.0 - this->friction));
			this->vy = round(this->vy * (1.0 - this->friction));
		}
	}

	// Search the next collision with the map border
	virtual Collision getCollision();

	// Search the next collision with an other unit
	Collision getCollision(Unit* u);
	
	// Bounce with the map border
	void bounce()
	{
		double mcoeff = 1.0 / this->mass;
		double nxnysquare = this->x * this->x + this->y * this->y;
		double product = (this->x * this->vx + this->y * this->vy) / (nxnysquare * mcoeff);
		double fx = this->x * product;
		double fy = this->y * product;

		this->vx -= fx * mcoeff;
		this->vy -= fy * mcoeff;

		fx = fx * IMPULSE_COEFF;
		fy = fy * IMPULSE_COEFF;

		// Normalize vector at min or max impulse
		double impulse = sqrt(fx * fx + fy * fy);
		double coeff = 1.0;
		if (impulse > EPSILON && impulse < MIN_IMPULSE)
		{
			coeff = MIN_IMPULSE / impulse;
		}

		fx = fx * coeff;
		fy = fy * coeff;
		this->vx -= fx * mcoeff;
		this->vy -= fy * mcoeff;

		double diff = distance(WATERTOWN) + this->radius - MAP_RADIUS;
		if (diff >= 0.0) 
		{
			// Unit still outside of the map, reposition it
			this->moveTo(WATERTOWN, diff + EPSILON);
		}
	}

	// Bounce between 2 units
	void bounce(Unit* u) 
	{
		double mcoeff = (this->mass + u->mass) / (this->mass * u->mass);
		double nx = this->x - u->x;
		double ny = this->y - u->y;
		double nxnysquare = nx * nx + ny * ny;
		double dvx = this->vx - u->vx;
		double dvy = this->vy - u->vy;
		double product = (nx * dvx + ny * dvy) / (nxnysquare * mcoeff);
		double fx = nx * product;
		double fy = ny * product;
		double m1c = 1.0 / this->mass;
		double m2c = 1.0 / u->mass;

		this->vx -= fx * m1c;
		this->vy -= fy * m1c;
		u->vx += fx * m2c;
		u->vy += fy * m2c;

		fx = fx * IMPULSE_COEFF;
		fy = fy * IMPULSE_COEFF;

		// Normalize vector at min or max impulse
		double impulse = sqrt(fx * fx + fy * fy);
		double coeff = 1.0;
		if (impulse > EPSILON && impulse < MIN_IMPULSE) 
		{
			coeff = MIN_IMPULSE / impulse;
		}

		fx = fx * coeff;
		fy = fy * coeff;

		this->vx -= fx * m1c;
		this->vy -= fy * m1c;
		u->vx += fx * m2c;
		u->vy += fy * m2c;

		double diff = (this->distance(*u) - this->radius - u->radius) / 2.0;
		if (diff <= 0.0)
		{
			// Unit overlapping. Fix positions.
			this->moveTo(*u, diff - EPSILON);
			u->moveTo(*this, diff - EPSILON);
		}
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

	Wreck* die()
	{
		// Don't spawn a wreck if our center is outside of the map
		if (this->distance(WATERTOWN) >= MAP_RADIUS)
		{
			return nullptr;
		}

		// TODO : should handle new id with max of ids or something like this
		return new Wreck(0, round(x), round(y), this->water, this->radius);
	}

	bool isFull() {
		return this->water >= this->size;
	}

	void play()
	{
		if (isFull()) 
		{
			// Try to leave the map
			this->thrust(WATERTOWN, -TANKER_THRUST);
		}
		else if (this->distance(WATERTOWN) > WATERTOWN_RADIUS)
		{
			// Try to reach watertown
			this->thrust(WATERTOWN, TANKER_THRUST);
		}
	}

	Collision getCollision();	
};

class Looter : public Unit
{
public:
	int skillCost;
	double skillRange;
	const Action* action; // Reference already created action

protected:
	Looter(int type, double mass, double friction, int skillCost, double skillRange, int id, double x, double y, double vx, double vy) :
		Unit(type, LOOTER_RADIUS, mass, friction, id, x, y, vx, vy), skillCost(skillCost), skillRange(skillRange)
	{
	}

	Looter(const Looter& other) : 
		Unit(other), skillCost(other.skillCost), skillRange(other.skillRange), action(other.action)
	{
	}

	Looter& operator=(const Looter& other)
	{
		Unit::operator=(other);

		this->skillCost = other.skillCost;
		this->skillRange = other.skillRange;
		this->action = other.action;

		return *this;
	}

public:	
	virtual SkillEffect* skill(const Point& point) = 0;

	void moveAction()
	{
		if (this->action->type == MOVE_ACTION)
		{
			this->thrust(Point(this->action->x, this->action->y), this->action->throttel);
		}
	}

	void skillAction(vector<SkillEffect *>& effects)
	{
		if (this->action->type == SKILL_ACTION)
		{
			effects.push_back(this->skill(Point(this->action->x, this->action->y)));
		}
	}
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
		return new ReaperSkillEffect(0, point.x, point.y, REAPER_SKILL_DURATION);
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
		return new DestroyerSkillEffect(0, point.x, point.y, DESTROYER_SKILL_DURATION);
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
		return new DoofSkillEffect(0, point.x, point.y, DOOF_SKILL_DURATION);
	}

	int sing() 
	{
		return (int)floor(this->speed() * DOOF_RAGE_COEF);
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

		this->reaper = nullptr;
		this->destroyer = nullptr;
		this->doof = nullptr;
	}
};

class Collision 
{
public:
	double t;
	Unit* a;
	Unit* b;

	Collision(double t) : t(t)
	{
	}

	Collision(double t, Unit* a) : t(t), a(a)
	{
	}

	Collision(double t, Unit* a, Unit* b) : t(t), a(a), b(b)
	{
	}

	Tanker* dead() const
	{
		if (a->type == DESTROYER_UNITID && b->type == TANKER_UNITID && b->mass < REAPER_SKILL_MASS_BONUS)
		{
			return (Tanker*)b;
		}

		if (b->type == DESTROYER_UNITID && a->type == TANKER_UNITID && a->mass < REAPER_SKILL_MASS_BONUS)
		{
			return (Tanker*)a;
		}

		return nullptr;
	}
};

class Board
{
public:
	Player* players[PLAYERS_COUNT];
	vector<Tanker> tankers;
	vector<Wreck> wrecks;
	vector<Unit*> units; // Reference already created units
	vector<SkillEffect*> skillEffects; // Need pointers because SkillEffect is abstract

	Board()
	{
		for (int i = 0; i < PLAYERS_COUNT; ++i)
		{
			this->players[i] = nullptr;
		}
	}

	Board(const Board& other) : tankers(other.tankers), wrecks(other.wrecks)
	{
		for (int i = 0; i < PLAYERS_COUNT; ++i)
		{
			if (other.players[i])
			{
				this->players[i] = new Player(*(other.players[i]));
			}
			else
			{
				this->players[i] = nullptr;
			}
		}

		this->linkUnits();

		// TODO : copy skill effects
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
			else
			{
				this->players[i] = nullptr;
			}
		}

		this->tankers = other.tankers;
		this->wrecks = other.wrecks;

		this->linkUnits();

		for (auto it = this->skillEffects.begin(); it != this->skillEffects.end(); ++it)
		{
			delete *it;
		}
		this->skillEffects.clear();
		// TODO : copy skill effects

		return *this;
	}

	~Board()
	{
		for (int i = 0; i < PLAYERS_COUNT; ++i)
		{
			delete this->players[i];
			this->players[i] = nullptr;
		}

		for (auto it = this->skillEffects.begin(); it != this->skillEffects.end(); ++it)
		{
			delete *it;			
		}
	}

	void resetUnits()
	{
		for (int i = 0; i < PLAYERS_COUNT; ++i)
		{
			delete this->players[i]->reaper;
			delete this->players[i]->destroyer;
			delete this->players[i]->doof;

			this->players[i]->reaper = nullptr;
			this->players[i]->destroyer = nullptr;
			this->players[i]->doof = nullptr;
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

	void setPlayerActions(int playerId, const Action actions[3])
	{
		this->players[playerId]->reaper->action = &actions[0];
		this->players[playerId]->destroyer->action = &actions[1];
		this->players[playerId]->doof->action = &actions[2];
	}

	// Get the next collision for the current round
	// All units are tested
	Collision getNextCollision() 
	{
		Collision result = NULL_COLLISION;

		for (auto it = this->units.begin(); it != this->units.end(); ++it)
		{
			// Test collision with map border first
			Collision collision = (*it)->getCollision();

			if (collision.t < result.t) {
				result = collision;
			}
			
			for (auto it2 = it; it2 != this->units.end(); ++it2)
			{
				collision = (*it)->getCollision(*it2);

				if (collision.t < result.t) {
					result = collision;
				}
			}
		}

		return result;
	}


	void updateGame()
	{
		// TODO : Create skill effects
		for (int i = 0; i < PLAYERS_COUNT; ++i)
		{
			this->players[i]->reaper->skillAction(this->skillEffects);
			this->players[i]->destroyer->skillAction(this->skillEffects);
			this->players[i]->doof->skillAction(this->skillEffects);
		}

		// Apply skill effects
		for (auto it = this->skillEffects.begin(); it != this->skillEffects.end(); ++it)
		{
			(*it)->apply(*this);
		}
		
		// Apply thrust for tankers
		for (auto it = this->tankers.begin(); it != this->tankers.end(); ++it)
		{
			it->play();
		}

		// Apply wanted thrust for looters
		for (int i = 0; i < PLAYERS_COUNT; ++i)
		{
			this->players[i]->reaper->moveAction();
			this->players[i]->destroyer->moveAction();
			this->players[i]->doof->moveAction();
		}		

		double t = 0.0;

		// Play the round. Stop at each collisions and play it. Reapeat until t > 1.0

		Collision collision = getNextCollision();

		while (collision.t + t <= 1.0)
		{
			double delta = collision.t;

			// move of delta time
			for (auto it = this->units.begin(); it != this->units.end(); ++it)
			{
				(*it)->move(delta);
			}
			t += collision.t;

			this->playCollision(collision);

			collision = getNextCollision();
		}

		// No more collision. Move units until the end of the round
		double delta = 1.0 - t;
		for (auto it = this->units.begin(); it != this->units.end(); ++it)
		{
			(*it)->move(delta);
		}

		for (auto it = this->tankers.begin(); it != this->tankers.end();)
		{
			double distance = it->distance(WATERTOWN);
			bool full = it->isFull();

			if (distance <= WATERTOWN_RADIUS && !full) 
			{
				// A non full tanker in watertown collect some water
				it->water += 1;
				it->mass += TANKER_MASS_BY_WATER;
			}
			else if (distance >= TANKER_SPAWN_RADIUS + it->radius && full)
			{
				// Remove too far away and not full tankers from the game

				// Remove from units
				vector<Unit *>::iterator it2;
				for (it2 = this->units.begin(); it2 != this->units.end(); ++it2)
				{
					if ((*it2)->id == it->id)
					{
						this->units.erase(it2);
						break;
					}
				}

				// Remove from tankers
				it = this->tankers.erase(it);
				continue;
			}

			++it;
		}

		// TODO : Spawn new tankers for each dead tanker during the round
		// Water collection for reapers
		for (auto it = this->wrecks.begin(); it != this->wrecks.end();)
		{
			bool alive = it->harvest(this->players, this->skillEffects);
			
			if (!alive)
			{
				// TODO : Spawn new tanker

				// Remove from wrecks
				it = this->wrecks.erase(it);				
				continue;
			}

			++it;
		}		

		// Round values and apply friction
		this->adjust();

		// Generate rage
		for (int i = 0; i < PLAYERS_COUNT; ++i)
		{
			this->players[i]->rage = min(MAX_RAGE, this->players[i]->rage + this->players[i]->doof->sing());
		}

		// Restore masses
		for (auto it = this->units.begin(); it != this->units.end(); ++it)
		{
			while ((*it)->mass >= REAPER_SKILL_MASS_BONUS) 
			{
				(*it)->mass -= REAPER_SKILL_MASS_BONUS;
			}
		}

		// Remove dead skill effects
		for (auto it = this->skillEffects.begin(); it != this->skillEffects.end();)
		{
			if ((*it)->duration <= 0)
			{
				// Remove from skillEffects
				it = this->skillEffects.erase(it);
				continue;
			}

			++it;
		}
	}
	
	void adjust()
	{
		for (auto it = this->units.begin(); it != this->units.end(); ++it)
		{
			(*it)->adjust(this->skillEffects);
		}
	}

	// Play a collision
	void playCollision(const Collision& collision) 
	{
		if (!collision.b)
		{
			// Bounce with border
			collision.a->bounce();
		}
		else
		{
			Tanker* dead = collision.dead();

			if (dead) 
			{
				// A destroyer kill a tanker				
				Wreck* wreck = dead->die();

				// If a tanker is too far away, there's no wreck
				if (wreck) 
				{
					this->wrecks.push_back(*wreck);
					delete wreck;
				}

				// Remove from units
				vector<Unit *>::iterator it;
				for (it = this->units.begin(); it != this->units.end(); ++it)
				{
					if ((*it)->id == dead->id)
					{
						this->units.erase(it);
						break;
					}
				}

				// Remove from tankers
				vector<Tanker>::iterator it2;
				for (it2 = this->tankers.begin(); it2 != this->tankers.end(); ++it2)
				{
					if (it2->id == dead->id)
					{
						this->tankers.erase(it2);
						break;
					}
				}
			}
			else 
			{
				// Bounce between two units
				collision.a->bounce(collision.b);
			}
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

bool Wreck::harvest(Player* players[PLAYERS_COUNT], const vector<SkillEffect *>& skillEffects)
{
	for (int i = 0; i < PLAYERS_COUNT; ++i)
	{
		auto reaper = players[i]->reaper;
		if (this->isInRange(*reaper, radius) && !reaper->isInDoofSkill(skillEffects))
		{
			players[i]->score += 1;
			water -= 1;
		}
	}

	return this->water > 0;
}

Collision Unit::getCollision()
{
	// Check instant collision
	if (this->distance(WATERTOWN) + this->radius >= MAP_RADIUS)
	{
		return Collision(0.0, this);
	}

	// We are not moving, we can't reach the map border
	if (fabs(this->vx) < EPSILON && fabs(this->vy) < EPSILON)
	{
		return NULL_COLLISION;
	}

	// Search collision with map border
	// Resolving: sqrt((x + t*vx)^2 + (y + t*vy)^2) = MAP_RADIUS - radius <=> t^2*(vx^2 + vy^2) + t*2*(x*vx + y*vy) + x^2 + y^2 - (MAP_RADIUS - radius)^2 = 0
	// at^2 + bt + c = 0;
	// a = vx^2 + vy^2
	// b = 2*(x*vx + y*vy)
	// c = x^2 + y^2 - (MAP_RADIUS - radius)^2

	double a = this->vx * this->vx + this->vy * this->vy;

	if (a <= 0.0)
	{
		return NULL_COLLISION;
	}

	double b = 2.0 * (this->x * this->vx + this->y * this->vy);
	double c = this->x * this->x + this->y * this->y - (MAP_RADIUS - this->radius) * (MAP_RADIUS - this->radius);
	double delta = b * b - 4.0 * a * c;

	if (delta <= 0.0)
	{
		return NULL_COLLISION;
	}

	double t = (-b + sqrt(delta)) / (2.0 * a);

	if (t <= 0.0)
	{
		return NULL_COLLISION;
	}

	return Collision(t, this);
}

Collision Unit::getCollision(Unit* u)
{
	// Check instant collision
	if (this->distance(*u) <= this->radius + u->radius) {
		return Collision(0.0, this, u);
	}

	// Both units are motionless
	if (fabs(this->vx) < EPSILON && fabs(this->vy) < EPSILON && fabs(u->vx) < EPSILON && fabs(u->vy) < EPSILON)
	{
		return NULL_COLLISION;
	}

	// Change referencial
	// Unit u is not at point (0, 0) with a speed vector of (0, 0)
	double x2 = this->x - u->x;
	double y2 = this->y - u->y;
	double r2 = this->radius + u->radius;
	double vx2 = this->vx - u->vx;
	double vy2 = this->vy - u->vy;

	// Resolving: sqrt((x + t*vx)^2 + (y + t*vy)^2) = radius <=> t^2*(vx^2 + vy^2) + t*2*(x*vx + y*vy) + x^2 + y^2 - radius^2 = 0
	// at^2 + bt + c = 0;
	// a = vx^2 + vy^2
	// b = 2*(x*vx + y*vy)
	// c = x^2 + y^2 - radius^2 

	double a = vx2 * vx2 + vy2 * vy2;

	if (a <= 0.0)
	{
		return NULL_COLLISION;
	}

	double b = 2.0 * (x2 * vx2 + y2 * vy2);
	double c = x2 * x2 + y2 * y2 - r2 * r2;
	double delta = b * b - 4.0 * a * c;

	if (delta < 0.0)
	{
		return NULL_COLLISION;
	}

	double t = (-b - sqrt(delta)) / (2.0 * a);

	if (t <= 0.0)
	{
		return NULL_COLLISION;
	}

	return Collision(t, this, u);
}

Collision Tanker::getCollision()
{
	// Tankers can go outside of the map
	return NULL_COLLISION;
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
		case REAPER_SKILL_EFFECT:
			board.skillEffects.push_back(new ReaperSkillEffect(unitId, x, y, extra));
			break;
		case DOOF_SKILL_EFFECT:
			board.skillEffects.push_back(new DoofSkillEffect(unitId, x, y, extra));
			break;
		case DESTROYER_SKILL_EFFECT:
			board.skillEffects.push_back(new DestroyerSkillEffect(unitId, x, y, extra));
			break;
		default:
			cerr << "Unknown Unit Type " << unitType << endl;
			break;			
		}
	}

	board.linkUnits();

	for (auto it = board.skillEffects.begin(); it != board.skillEffects.end(); ++it)
	{
		delete *it;
	}
	board.skillEffects.clear();
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

int computeScore(const Board& board)
{
	return 0;
}

Action generateRandomAction()
{
	Action action;
	action.type = rand() % 3;
	if (action.type > WAIT_ACTION)
	{
		action.x = (rand() % (2 * MAP_RADIUS) + 1) - MAP_RADIUS;
		action.y = (rand() % (2 * MAP_RADIUS) + 1) - MAP_RADIUS;
	}

	if (action.type == MOVE_ACTION)
	{
		action.throttel = rand() % (MAX_THRUST + 1);
	}

	return action;
}

void strategy2(const Board& board)
{
	// Monte Carlo Depth 1
	Board simulationBoard;
	int bestScore = -1000;
	Action bestActions[3];

	for (int i = 0; i < SIMULATIONS_NUMBER; ++i)
	{
		// Copy board to simulate
		simulationBoard = board;

		// Generate actions
		Action actions[3];
		for (int j = 0; j < 3; ++j)
		{
			actions[j] = generateRandomAction();
		}

		// Play them
		simulationBoard.setPlayerActions(0, actions);
		simulationBoard.setPlayerActions(1, actions);
		simulationBoard.setPlayerActions(2, actions);
		simulationBoard.updateGame();

		// Evaluate score
		int score = computeScore(simulationBoard);
		
		if (score > bestScore)
		{
			// New is better
			bestScore = score;
			for (int j = 0; j < 3; ++j)
			{
				bestActions[j] = actions[j];
			}			
		}
	}

	// Print best
	for (int i = 0; i < 3; ++i)
	{
		bestActions[i].print();
	}
}

int main()
{
	// Rand init
	srand(time(NULL));

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
		auto start = std::chrono::high_resolution_clock::now();
		strategy2(board);
		auto end = std::chrono::high_resolution_clock::now();
		auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
		cerr << "Strategy tooked " << elapsed.count() << " milliseconds." << endl;
	}
}