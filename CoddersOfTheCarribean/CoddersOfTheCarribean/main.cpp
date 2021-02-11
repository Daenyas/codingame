#include <iostream>
#include <sstream>
#include <string>
#include <limits>
#include <vector>
#include <algorithm>
#include <stdlib.h>
#include <time.h>
#include <typeinfo>

#pragma GCC optimize ("O3")

#define M_PI 3.14159265358979323846
#define MAP_WIDTH 23
#define MAP_HEIGHT 21
#define COOLDOWN_CANNON 2
#define COOLDOWN_MINE 5
#define INITIAL_SHIP_HEALTH 100
#define MAX_SHIP_HEALTH 100
#define MIN_SHIPS 1
#define MIN_RUM_BARRELS 10
#define MAX_RUM_BARRELS 26
#define MIN_RUM_BARREL_VALUE 10
#define MAX_RUM_BARREL_VALUE 20
#define REWARD_RUM_BARREL_VALUE 30
#define MINE_VISIBILITY_RANGE 5
#define FIRE_DISTANCE_MAX 10
#define LOW_DAMAGE 25
#define HIGH_DAMAGE 50
#define MINE_DAMAGE 25
#define NEAR_MINE_DAMAGE 10

//#define MAX_SHIP_SPEED;
//#define MAX_SHIPS;
//#define MIN_MINES;
//#define MAX_MINES;

#define MAX_SHIPS 1
#define CANNONS_ENABLED false
#define MINES_ENABLED false
#define MIN_MINES 0
#define MAX_MINES 0
#define MAX_SHIP_SPEED 1

#define TURNS 5
#define SIMULS 5000

using namespace std;

template<typename T>
inline string join(const T& value)
{
	ostringstream oss;
	oss << value;
	return oss.str();
}

template<typename Tf, typename ...Tr>
inline string join(const Tf& first, const Tr& ... remaining)
{
	return join(first) + " " + join(remaining...);
}

inline int clamp(int val, int min, int max)
{
	return val > max ? max : (val < min ? min : val);
}

template<typename T>
inline vector<T*> copy(const vector<T*>& vect)
{
	vector<T*> result;
	for (T* t : vect)
	{
		result.push_back(new T(*t));
	}
	return result;
}

//
//template<typename T>
//T max(const T& a, const T&b)
//{
//	return a > b ? a : b;
//}
//
//template<typename T>
//T min(const T& a, const T&b)
//{
//	return a < b ? a : b;
//}

template<typename T, int maxSize>
class List
{
	public:
		T array[maxSize];
		int length;

		List() : length(0)
		{
		}

		void add(const T& val)
		{
		}
};


class CubeCoordinate 
{
	public :
		static const int DIRECTIONS[6][3];
		int x;
		int y;
		int z;

		CubeCoordinate(int x, int y, int z) : x(x), y(y), z(z)
		{
		}

		CubeCoordinate neighbor(int orientation) const
		{
			int nx = this->x + DIRECTIONS[orientation][0];
			int ny = this->y + DIRECTIONS[orientation][1];
			int nz = this->z + DIRECTIONS[orientation][2];
			return CubeCoordinate(nx, ny, nz);
		}

		int distanceTo(CubeCoordinate dst) const
		{
			return (abs(x - dst.x) + abs(y - dst.y) + abs(z - dst.z)) / 2;
		}
	
		string toString() const
		{
			return join(x, y, z);
		}
};
const int CubeCoordinate::DIRECTIONS[6][3] = { { 1, -1, 0 },{ +1, 0, -1 },{ 0, +1, -1 },{ -1, +1, 0 },{ -1, 0, +1 },{ 0, -1, +1 } };

class Coord 
{
	public:
		static const int DIRECTIONS_EVEN[6][2];
		static const int DIRECTIONS_ODD[6][2];
		int x;
		int y;

		Coord() : x(-1), y(-1)
		{
		}

		Coord(int x, int y) : x(x), y(y)
		{
		}

		Coord(const Coord& other) : x(other.x), y(other.y)
		{
		}

		Coord(const CubeCoordinate& cube) : x(cube.x + (cube.z - (cube.z & 1)) / 2), y(cube.z)
		{
		}

		double angle(const Coord& targetPosition) const
		{
			double dy = (targetPosition.y - this->y) * sqrt(3) / 2;
			double dx = targetPosition.x - this->x + ((this->y - targetPosition.y) & 1) * 0.5;
			double angle = -atan2(dy, dx) * 3 / M_PI;
			if (angle < 0) 
			{
				angle += 6;
			}
			else if (angle >= 6) 
			{
				angle -= 6;
			}
			return angle;
		}

		CubeCoordinate toCubeCoordinate() const 
		{
			int xp = x - (y - (y & 1)) / 2;
			int zp = y;
			int yp = -(xp + zp);
			return CubeCoordinate(xp, yp, zp);
		}

		Coord neighbor(int orientation) const
		{
			int newY, newX;
			if (this->y % 2 == 1) 
			{
				newY = this->y + DIRECTIONS_ODD[orientation][1];
				newX = this->x + DIRECTIONS_ODD[orientation][0];
			}
			else
			{
				newY = this->y + DIRECTIONS_EVEN[orientation][1];
				newX = this->x + DIRECTIONS_EVEN[orientation][0];
			}

			return Coord(newX, newY);
		}

		bool isInsideMap() const
		{
			return this->x >= 0 && this->x < MAP_WIDTH && this->y >= 0 && this->y < MAP_HEIGHT;
		}

		int distanceTo(const Coord& dst) const
		{
			return this->toCubeCoordinate().distanceTo(dst.toCubeCoordinate());
		}

		bool equals(const Coord& other) const
		{
			return y == other.y && x == other.x;
		}

		string toString() const {
			return join(x, y);
		}

		bool isNull() const
		{
			return this->x == -1 || this->y == -1;
		}
};

const int Coord::DIRECTIONS_EVEN[6][2] = { { 1, 0 }, { 0, -1 }, { -1, -1 }, { -1, 0 }, { -1, 1 }, { 0, 1 } };
const int Coord::DIRECTIONS_ODD[6][2] = { { 1, 0 },{ 1, -1 },{ 0, -1 },{ -1, 0 },{ 0, 1 },{ 1, 1 } };

class Damage 
{
	public:
		Coord position;
		int health;
		bool hit;

		Damage(const Coord& position, int health, bool hit) : position(position), health(health), hit(hit)
		{
		}

		string toViewString() const
		{
			return join(position.y, position.x, health, (hit ? 1 : 0));
		}
};

enum Action
{
	NONE,
	FASTER,
	SLOWER,
	PORT,
	STARBOARD,
	FIRE,
	PLACEMINE
};

enum EntityType
{
	SHIP,
	BARREL,
	MINE,
	CANNONBALL
};

class Entity 
{
	public:
		int id;
		EntityType type;
		Coord position;
	
	Entity(EntityType type, int x, int y, int id) : type(type), position(x,y), id(id)
	{
	}
	
	virtual string toViewString() const
	{
		return join(id, position.x, position.y);
	}
	
	string toPlayerString(int arg1, int arg2, int arg3, int arg4) const
	{
		return join(id, typeid(this).name(), position.x, position.y, arg1, arg2, arg3, arg4);
	}
};

class Ship : public Entity
{
	public:
		int orientation;
		int speed;
		int health;
		int owner;
		string message;
		Action action;
		int mineCooldown;
		int cannonCooldown;
		Coord target;
		int newOrientation;
		Coord newPosition;
		Coord newBowCoordinate;
		Coord newSternCoordinate;

		Ship(int x, int y, int orientation, int speed, int health, int owner, int id) : Entity(EntityType::SHIP, x, y, id),
			orientation(orientation), speed(speed), health(health), owner(owner), action(Action::NONE), message(""), mineCooldown(0), cannonCooldown(0), newOrientation(0)
		{
		}

		string toViewString() const
		{
			return join(id, position.y, position.x, orientation, health, speed, (action != Action::NONE ? to_string(action) : "WAIT"), bow().y, bow().x, stern().y,
				stern().x, " ;" + message);
		}

		string toPlayerString(int playerIdx) const
		{
			return Entity::toPlayerString(orientation, speed, health, owner == playerIdx ? 1 : 0);
		}

		void setMessage(string& message)
		{
			if (message.length() > 50)
			{
				message = message.substr(0, 50) + "...";
			}
			this->message = message;
		}

		inline void moveTo(const Coord& coord)
		{
			return moveTo(coord.x, coord.y);
		}

		void moveTo(int x, int y)
		{
			Coord currentPosition = this->position;
			Coord targetPosition = Coord(x, y);

			if (currentPosition.equals(targetPosition)) 
			{
				this->action = Action::SLOWER;
				return;
			}

			double targetAngle, angleStraight, anglePort, angleStarboard, centerAngle, anglePortCenter, angleStarboardCenter;

			switch (speed)
			{
				case 2:
				{
					this->action = Action::SLOWER;
				}
				break;
				case 1:
				{
					// Suppose we've moved first
					currentPosition = currentPosition.neighbor(orientation);
					if (!currentPosition.isInsideMap()) 
					{
						this->action = Action::SLOWER;
						break;
					}

					// Target reached at next turn
					if (currentPosition.equals(targetPosition)) 
					{
						this->action = Action::NONE;
						break;
					}

					// For each neighbor cell, find the closest to target
					targetAngle = currentPosition.angle(targetPosition);
					angleStraight = min(abs(orientation - targetAngle), 6 - abs(orientation - targetAngle));
					anglePort = min(abs((orientation + 1) - targetAngle), abs((orientation - 5) - targetAngle));
					angleStarboard = min(abs((orientation + 5) - targetAngle), abs((orientation - 1) - targetAngle));

					centerAngle = currentPosition.angle(Coord(MAP_WIDTH / 2, MAP_HEIGHT / 2));
					anglePortCenter = min(abs((orientation + 1) - centerAngle), abs((orientation - 5) - centerAngle));
					angleStarboardCenter = min(abs((orientation + 5) - centerAngle), abs((orientation - 1) - centerAngle));

					// Next to target with bad angle, slow down then rotate (avoid to turn around the target!)
					if (currentPosition.distanceTo(targetPosition) == 1 && angleStraight > 1.5)
					{
						this->action = Action::SLOWER;
						break;
					}

					int distanceMin = -1;

					// Test forward
					Coord nextPosition = currentPosition.neighbor(orientation);
					if (nextPosition.isInsideMap())
					{
						distanceMin = nextPosition.distanceTo(targetPosition);
						this->action = Action::NONE;
					}

					// Test port
					nextPosition = currentPosition.neighbor((orientation + 1) % 6);
					if (nextPosition.isInsideMap()) 
					{
						int distance = nextPosition.distanceTo(targetPosition);
						if (distanceMin == -1 || distance < distanceMin || distance == distanceMin && anglePort < angleStraight - 0.5) 
						{
							distanceMin = distance;
							this->action = Action::PORT;
						}
					}

					// Test starboard
					nextPosition = currentPosition.neighbor((orientation + 5) % 6);
					if (nextPosition.isInsideMap())
					{
						int distance = nextPosition.distanceTo(targetPosition);
						if (distanceMin == -1 || distance < distanceMin
							|| (distance == distanceMin && angleStarboard < anglePort - 0.5 && this->action == Action::PORT)
							|| (distance == distanceMin && angleStarboard < angleStraight - 0.5 && this->action == Action::NONE)
							|| (distance == distanceMin && this->action == Action::PORT && angleStarboard == anglePort
								&& angleStarboardCenter < anglePortCenter)
							|| (distance == distanceMin && this->action == Action::PORT && angleStarboard == anglePort
								&& angleStarboardCenter == anglePortCenter && (orientation == 1 || orientation == 4)))
						{
							distanceMin = distance;
							this->action = Action::STARBOARD;
						}
					}
				}
				break;
				case 0:
				{
					// Rotate ship towards target
					targetAngle = currentPosition.angle(targetPosition);
					angleStraight = min(abs(orientation - targetAngle), 6 - abs(orientation - targetAngle));
					anglePort = min(abs((orientation + 1) - targetAngle), abs((orientation - 5) - targetAngle));
					angleStarboard = min(abs((orientation + 5) - targetAngle), abs((orientation - 1) - targetAngle));

					centerAngle = currentPosition.angle(Coord(MAP_WIDTH / 2, MAP_HEIGHT / 2));
					anglePortCenter = min(abs((orientation + 1) - centerAngle), abs((orientation - 5) - centerAngle));
					angleStarboardCenter = min(abs((orientation + 5) - centerAngle), abs((orientation - 1) - centerAngle));

					Coord forwardPosition = currentPosition.neighbor(orientation);

					this->action = Action::NONE;

					if (anglePort <= angleStarboard) 
					{
						this->action = Action::PORT;
					}

					if (angleStarboard < anglePort || angleStarboard == anglePort && angleStarboardCenter < anglePortCenter
						|| angleStarboard == anglePort && angleStarboardCenter == anglePortCenter && (orientation == 1 || orientation == 4)) 
					{
						this->action = Action::STARBOARD;
					}

					if (forwardPosition.isInsideMap() && angleStraight <= anglePort && angleStraight <= angleStarboard)
					{
						this->action = Action::FASTER;
					}
				}
				break;
			}
		}

		void faster()
		{
			this->action = Action::FASTER;
		}

		void slower()
		{
			this->action = Action::SLOWER;
		}

		void port()
		{
			this->action = Action::PORT;
		}

		void starboard()
		{
			this->action = Action::STARBOARD;
		}

		void placeMine()
		{
			if (MINES_ENABLED)
			{
				this->action = Action::PLACEMINE;
			}
		}

		Coord stern() const
		{
			return position.neighbor((orientation + 3) % 6);
		}

		Coord bow() const
		{
			return position.neighbor(orientation);
		}

		Coord newStern() const
		{
			return position.neighbor((newOrientation + 3) % 6);
		}

		Coord newBow() const
		{
			return position.neighbor(newOrientation);
		}

		bool at(const Coord& coord) const
		{
			Coord stern = this->stern();
			Coord bow = this->bow();
			return !stern.isNull() && stern.equals(coord) || !bow.isNull() && bow.equals(coord) || position.equals(coord);
		}

		bool newBowIntersect(const Ship& other) const
		{
			return !this->newBowCoordinate.isNull() && (this->newBowCoordinate.equals(other.newBowCoordinate) || this->newBowCoordinate.equals(other.newPosition)
				|| this->newBowCoordinate.equals(other.newSternCoordinate));
		}

		bool newBowIntersect(const vector<Ship*>& ships) const
		{
			for (Ship* other : ships)
			{
				if (this != other && newBowIntersect(*other)) 
				{
					return true;
				}
			}
			return false;
		}

		bool newPositionsIntersect(const Ship& other) const
		{
			bool sternCollision = !this->newSternCoordinate.isNull() && (this->newSternCoordinate.equals(other.newBowCoordinate)
				|| this->newSternCoordinate.equals(other.newPosition) || this->newSternCoordinate.equals(other.newSternCoordinate));
			bool centerCollision = !this->newPosition.isNull() && (this->newPosition.equals(other.newBowCoordinate) || this->newPosition.equals(other.newPosition)
				|| this->newPosition.equals(other.newSternCoordinate));
			return newBowIntersect(other) || sternCollision || centerCollision;
		}

		bool newPositionsIntersect(const vector<Ship*>& ships) const
		{
			for (Ship* other : ships)
			{
				if (this != other && newPositionsIntersect(*other))
				{
					return true;
				}
			}
			return false;
		}

		void damage(int health)
		{
			this->health -= health;
			if (this->health <= 0) 
			{
				this->health = 0;
			}
		}

		void heal(int health)
		{
			this->health += health;
			if (this->health > MAX_SHIP_HEALTH) 
			{
				this->health = MAX_SHIP_HEALTH;
			}
		}

		inline void fire(const Coord& coord)
		{
			return this->fire(coord.x, coord.y);
		}

		void fire(int x, int y)
		{
			if (CANNONS_ENABLED) 
			{
				Coord target = Coord(x, y);
				this->target = target;
				this->action = Action::FIRE;
			}
		}
};

class Mine : public Entity 
{
	public:
		Mine(int x, int y, int id) : Entity(EntityType::MINE, x, y, id)
		{
		}

		string toPlayerString(int playerIdx) const
		{
			return Entity::toPlayerString(0, 0, 0, 0);
		}

		vector<Damage*> explode(const vector<Ship*>& ships, bool force) {
			vector<Damage*> damage;
			Ship* victim = nullptr;

			for (Ship* ship : ships) {
				if (position.equals(ship->bow()) || position.equals(ship->stern()) || position.equals(ship->position))
				{
					damage.push_back(new Damage(this->position, MINE_DAMAGE, true));
					ship->damage(MINE_DAMAGE);
					victim = ship;
				}
			}

		if (force || victim != nullptr) {
			if (victim == nullptr) 
			{
				damage.push_back(new Damage(this->position, MINE_DAMAGE, true));
			}

			for (Ship* ship : ships)
			{
				if (ship != victim)
				{
					Coord impactPosition;
					if (ship->stern().distanceTo(position) <= 1)
					{
						impactPosition = ship->stern();
					}
					if (ship->bow().distanceTo(position) <= 1)
					{
						impactPosition = ship->bow();
					}
					if (ship->position.distanceTo(position) <= 1)
					{
						impactPosition = ship->position;
					}

					if (!impactPosition.isNull())
					{
						ship->damage(NEAR_MINE_DAMAGE);
						damage.push_back(new Damage(impactPosition, NEAR_MINE_DAMAGE, true));
					}
				}
			}
		}

		return damage;
	}
};

class Cannonball : public Entity 
{
	public:
		int ownerEntityId;
		int srcX;
		int srcY;
		int initialRemainingTurns;
		int remainingTurns;

		Cannonball(int row, int col, int ownerEntityId, int srcX, int srcY, int remainingTurns, int id) : Entity(EntityType::CANNONBALL, row, col, id), 
			ownerEntityId(ownerEntityId), srcX(srcX), srcY(srcY), initialRemainingTurns(initialRemainingTurns), remainingTurns(initialRemainingTurns)
		{
		}

		string toViewString() const
		{
			return join(id, position.y, position.x, srcY, srcX, initialRemainingTurns, remainingTurns, ownerEntityId);
		}

		string toPlayerString(int playerIdx) const
		{
			return Entity::toPlayerString(ownerEntityId, remainingTurns, 0, 0);
		}
};

class RumBarrel : public Entity
{
	public:
		int health;

		RumBarrel(int x, int y, int health, int id) : Entity(EntityType::BARREL, x, y, id), health(health)
		{
		}

		string toViewString() const
		{
			return join(id, position.y, position.x, health);
		}

		string toPlayerString(int playerIdx) const
		{
			return Entity::toPlayerString(health, 0, 0, 0);
		}
};

class Player
{
	public:
		int id;
		vector<Ship*> ships;
		vector<Ship*> shipsAlive;

		Player() : id(-1)
		{
		}

		Player(int id) : id(id)
		{
		}

		Player(const Player& other) : id(other.id)
		{
		}

		Player& operator=(const Player& other)
		{
			this->id = other.id;
			this->ships.clear();
			this->shipsAlive.clear();

			return *this;
		}

		void setDead()
		{
			for (Ship* ship : ships) 
			{
				ship->health = 0;
			}
		}

		int getScore() const 
		{
			int score = 0;
			for (Ship* ship : ships)
			{
				score += ship->health;
			}
			return score;
		}

		vector<string> toViewString()
		{
			vector<string> data;

			data.push_back(to_string(this->id));
			for (Ship* ship : ships) 
			{
				data.push_back(ship->toViewString());
			}

			return data;
		}
};

class Board
{
	public:
		vector<Cannonball*> cannonballs;
		vector<Mine*> mines;
		vector<RumBarrel*> barrels;
		Player players[2];
		vector<Ship*> ships;
		vector<Damage*> damage;
		vector<Ship*> shipLosts;
		vector<Coord> cannonBallExplosions;
		static int shipsPerPlayer;
		int mineCount;
		int barrelCount;

		Board()
		{
			players[0].id = 0;
			players[1].id = 1;
		}

		Board(const Board& other) : mineCount(other.mineCount), barrelCount(other.barrelCount)
		{
			this->cannonballs = copy(other.cannonballs);
			this->mines = copy(other.mines);
			this->barrels = copy(other.barrels);
			this->ships = copy(other.ships);
			this->damage = copy(other.damage);
			this->shipLosts = copy(other.shipLosts);
			this->cannonBallExplosions.insert(this->cannonBallExplosions.end(), other.cannonBallExplosions.begin(), other.cannonBallExplosions.end());
			this->players[0] = other.players[0];
			this->players[1] = other.players[1];
			this->affect();
		}

		void init(istream& in)
		{
			Board::shipsPerPlayer = 1;

			int myShipCount; // the number of remaining ships
			in >> myShipCount; in.ignore();
			int entityCount; // the number of entities (e.g. ships, mines or cannonballs)
			in >> entityCount; in.ignore();

			for (int i = 0; i < entityCount; ++i)
			{
				int entityId;
				string entityType;
				int x;
				int y;
				int arg1;
				int arg2;
				int arg3;
				int arg4;
				in >> entityId >> entityType >> x >> y >> arg1 >> arg2 >> arg3 >> arg4; in.ignore();

				if (entityType == "SHIP")
				{
					this->ships.push_back(new Ship(x, y, arg1, arg2, arg3, arg4, entityId));
				}
				else if (entityType == "BARREL")
				{
					this->barrels.push_back(new RumBarrel(x, y, arg1, entityId));
				}
				else if (entityType == "CANNONBALL")
				{
					this->cannonballs.push_back(new Cannonball(x, y, arg1, -1, -1, arg2, entityId));
				}
				else if (entityType == "MINE")
				{
					this->mines.push_back(new Mine(x, y, entityId));
				}
			}
		}

		void affect()
		{
			for (Ship* ship : this->ships)
			{
				this->players[ship->owner].ships.push_back(ship);
				if (ship->health > 0)
				{
					this->players[ship->owner].shipsAlive.push_back(ship);
				}
			}
		}

		void prepare()
		{
			for (Ship* ship : this->ships)
			{
				ship->action = Action::NONE;
				ship->message = "";
			}
			this->cannonBallExplosions.clear();
			this->damage.clear();
			this->shipLosts.clear();
		}

		void decrementRum() 
		{
			for (Ship* ship : this->ships)
			{
				ship->damage(1);
			}
		}

		void moveCannonballs()
		{
			for (auto it = this->cannonballs.begin(); it != this->cannonballs.end();)
			{
				
				if ((*it)->remainingTurns == 0)
				{
					it = cannonballs.erase(it);
					continue;
				}
				else if ((*it)->remainingTurns > 0) 
				{
					(*it)->remainingTurns--;
				}

				if ((*it)->remainingTurns == 0) 
				{
					this->cannonBallExplosions.push_back((*it)->position);
				}

				it++;
			}
		}

		void applyActions() 
		{
			for (Player player : this->players)
			{
				for (Ship* ship : player.shipsAlive)
				{
					if (ship->mineCooldown > 0) {
						ship->mineCooldown--;
					}
					if (ship->cannonCooldown > 0) {
						ship->cannonCooldown--;
					}

					ship->newOrientation = ship->orientation;

					if (ship->action != Action::NONE)
					{
						switch (ship->action)
						{
							case FASTER:
								if (ship->speed < MAX_SHIP_SPEED) 
								{
									ship->speed++;
								}
								break;
							case SLOWER:
								if (ship->speed > 0) 
								{
									ship->speed--;
								}
								break;
							case PORT:
								ship->newOrientation = (ship->orientation + 1) % 6;
								break;
							case STARBOARD:
								ship->newOrientation = (ship->orientation + 5) % 6;
								break;
							case PLACEMINE:
								if (ship->mineCooldown == 0)
								{
									Coord target = ship->stern().neighbor((ship->orientation + 3) % 6);
									if (target.isInsideMap()) {
										bool cellIsFreeOfBarrels = true;
										bool cellIsFreeOfShips = true;
										for (RumBarrel* barrel : this->barrels)
										{
											if (barrel->position.equals(target))
											{
												cellIsFreeOfBarrels = false;
												break;
											}
										}

										for (Ship* otherShip : this->ships)
										{
											if (otherShip == ship)
												continue;

											if (otherShip->at(target))
											{
												cellIsFreeOfShips = false;
												break;
											}
										}

										if (cellIsFreeOfBarrels && cellIsFreeOfShips) 
										{
											ship->mineCooldown = COOLDOWN_MINE;
											this->mines.push_back(new Mine(target.x, target.y, -1));
										}
									}

								}
								break;
							case FIRE:
								{
									int distance = ship->bow().distanceTo(ship->target);
									if (ship->target.isInsideMap() && distance <= FIRE_DISTANCE_MAX && ship->cannonCooldown == 0)
									{
										int travelTime = (int)(1 + round(ship->bow().distanceTo(ship->target) / 3.0));
										this->cannonballs.push_back(new Cannonball(ship->target.x, ship->target.y, ship->id, ship->bow().x, ship->bow().y, travelTime, -1));
										ship->cannonCooldown = COOLDOWN_CANNON;
									}
									break;
								}						
							default:
								break;
						}
					}
				}
			}
		}

		bool checkCollisions(Ship& ship)
		{
			Coord bow = ship.bow();
			Coord stern = ship.stern();
			Coord center = ship.position;

			// Collision with the barrels
			for (auto it = this->barrels.begin(); it != this->barrels.end();) 
			{
				if ((*it)->position.equals(bow) || (*it)->position.equals(stern) || (*it)->position.equals(center)) 
				{
					ship.heal((*it)->health);
					it = this->barrels.erase(it);
					continue;
				}
				
				it++;
			}

			// Collision with the mines
			for (auto it = this->mines.begin(); it != this->mines.end();)
			{
				vector<Damage *> mineDamage = (*it)->explode(this->ships, false);

				if (!mineDamage.empty())
				{
					this->damage.insert(this->damage.end(), mineDamage.begin(), mineDamage.end());
					it = this->mines.erase(it);
					continue;
				}

				it++;
			}

			return ship.health <= 0;
		}

		void moveShips() {
			// ---
			// Go forward
			// ---
			for (int i = 1; i <= MAX_SHIP_SPEED; i++) 
			{
				for (Player player : this->players)
				{
					for (Ship* ship : player.shipsAlive)
					{
						ship->newPosition = ship->position;
						ship->newBowCoordinate = ship->bow();
						ship->newSternCoordinate = ship->stern();

						if (i > ship->speed)
						{
							continue;
						}

						Coord newCoordinate = ship->position.neighbor(ship->orientation);

						if (newCoordinate.isInsideMap()) 
						{
							// Set new coordinate.
							ship->newPosition = newCoordinate;
							ship->newBowCoordinate = newCoordinate.neighbor(ship->orientation);
							ship->newSternCoordinate = newCoordinate.neighbor((ship->orientation + 3) % 6);
						}
						else
						{
							// Stop ship!
							ship->speed = 0;
						}
					}
				}

				// Check ship and obstacles collisions
				vector<Ship*> collisions;
				bool collisionDetected = true;
				while (collisionDetected) 
				{
					collisionDetected = false;

					for (Ship* ship : this->ships)
					{
						if (ship->newBowIntersect(ships)) {
							collisions.push_back(ship);
						}
					}

					for (Ship* ship : collisions) 
					{
						// Revert last move
						ship->newPosition = ship->position;
						ship->newBowCoordinate = ship->bow();
						ship->newSternCoordinate = ship->stern();

						// Stop ships
						ship->speed = 0;

						collisionDetected = true;
					}
					collisions.clear();
				}

				for (Player player : players) 
				{
					for (Ship* ship : player.shipsAlive) 
					{
						ship->position = ship->newPosition;
						if (checkCollisions(*ship)) 
						{
							this->shipLosts.push_back(ship);
						}
					}
				}
			}
		}

		void rotateShips() 
		{
			// Rotate
			for (Player player : players)
			{
				for (Ship* ship : player.shipsAlive) 
				{
					ship->newPosition = ship->position;
					ship->newBowCoordinate = ship->newBow();
					ship->newSternCoordinate = ship->newStern();
				}
			}

			// Check collisions
			bool collisionDetected = true;
			vector<Ship*> collisions;
			while (collisionDetected) {
				collisionDetected = false;

				for (Ship* ship : this->ships) 
				{
					if (ship->newPositionsIntersect(ships)) 
					{
						collisions.push_back(ship);
					}
				}

				for (Ship* ship : collisions)
				{
					ship->newOrientation = ship->orientation;
					ship->newBowCoordinate = ship->newBow();
					ship->newSternCoordinate = ship->newStern();
					ship->speed = 0;
					collisionDetected = true;
				}

				collisions.clear();
			}

			// Apply rotation
			for (Player player : players) 
			{
				for (Ship* ship : player.shipsAlive)
				{
					if (ship->health == 0)
					{
						continue;
					}

					ship->orientation = ship->newOrientation;
					if (checkCollisions(*ship))
					{
						shipLosts.push_back(ship);
					}
				}
			}
		}

		bool gameIsOver() 
		{
			for (Player player : players) 
			{
				if (player.shipsAlive.empty()) {
					return true;
				}
			}
			return barrels.size() == 0;// To remove after wood1 && LEAGUE_LEVEL == 0;
		}

		void explodeShips() 
		{
			for (auto it = this->cannonBallExplosions.begin(); it != this->cannonBallExplosions.end();) 
			{
				bool remove = false;
				for (Ship* ship : this->ships)
				{					
					if (it->equals(ship->bow()) || it->equals(ship->stern()))
					{
						this->damage.push_back(new Damage(*it, LOW_DAMAGE, true));
						ship->damage(LOW_DAMAGE);
						remove = true;
						break;
					}
					else if (it->equals(ship->position))
					{
						damage.push_back(new Damage(*it, HIGH_DAMAGE, true));
						ship->damage(HIGH_DAMAGE);
						remove = true;
						break;
					}			
				}
				
				if (remove)
				{
					it = this->cannonBallExplosions.erase(it);
					continue;
				}
				it++;
			}
		}

		void explodeMines() 
		{
			for (auto it = this->cannonBallExplosions.begin(); it != this->cannonBallExplosions.end();) 
			{
				bool remove = false;
				for (auto mit = this->mines.begin(); mit != this->mines.end();) 
				{
					if ((*mit)->position.equals(*it)) 
					{
						vector<Damage*> mines = (*mit)->explode(ships, true);
						this->damage.insert(this->damage.end(), mines.begin(), mines.end());
						mit = this->mines.erase(mit);
						remove = true;
						break;
					}
					mit++;
				}

				if (remove)
				{
					it = this->cannonBallExplosions.erase(it);
					continue;
				}
				it++;
			}
		}

		void explodeBarrels()
		{
			for (auto it = this->cannonBallExplosions.begin(); it != this->cannonBallExplosions.end();)
			{
				bool remove = false;
				for (auto rit = this->barrels.begin(); rit != this->barrels.end();)
				{
					if ((*rit)->position.equals(*it))
					{
						this->damage.push_back(new Damage(*it, 0, true));
						rit = this->barrels.erase(rit);
						remove = true;
						break;
					}
					rit++;
				}

				if (remove)
				{
					it = this->cannonBallExplosions.erase(it);
					continue;
				}
				it++;
			}
		}

		bool updateGame(int round)
		{
			this->moveCannonballs();
			this->decrementRum();

			this->applyActions();
			this->moveShips();
			this->rotateShips();

			this->explodeShips();
			this->explodeMines();
			this->explodeBarrels();

			for (Ship* ship : this->shipLosts)
			{
				this->barrels.push_back(new RumBarrel(ship->position.x, ship->position.y, REWARD_RUM_BARREL_VALUE, -1));
			}

			for (Coord position : this->cannonBallExplosions) 
			{
				this->damage.push_back(new Damage(position, 0, false));
			}

			for (auto it = this->ships.begin(); it != this->ships.end();) 
			{
				if ((*it)->health <= 0) 
				{
					for (auto it2 = this->players[(*it)->owner].shipsAlive.begin(); it != this->players[(*it)->owner].shipsAlive.end();)
					{
						if (*it2 == *it)
						{
							this->players[(*it)->owner].shipsAlive.erase(it2);
							break;
						}
						it2++;
					}
					
					it = this->ships.erase(it);
					continue;
				}
				it++;
			}

			return this->gameIsOver();
		}
};

int Board::shipsPerPlayer = 1;

class ShipStrategy
{
	public:
		string command;
		ShipStrategy() : command("")
		{
		}

		virtual void apply(Board& board, Ship& ship) = 0;
};

class ClosestBarrel : public ShipStrategy
{
	public:
		virtual void apply(Board& board, Ship& ship)
		{
			int minDistance = 5000;
			RumBarrel* closest = nullptr;
			for (RumBarrel* barrel : board.barrels)
			{
				int distance = board.players[1].ships[0]->position.distanceTo(barrel->position);
				if (distance < minDistance)
				{
					closest = barrel;
					minDistance = distance;
				}
			}

			if (closest)
			{
				ship.moveTo(closest->position);
				this->command = join("MOVE", closest->position.x, closest->position.y);
			}
			else
			{
				this->command = "WAIT";
			}
		}
};

class ClosestEnnemy : public ShipStrategy
{
	public:
		virtual void apply(Board& board, Ship& ship)
		{
			int minDistance = 5000;
			Ship* closest = nullptr;
			for (Ship* ennemy : board.players[1 - ship.owner].shipsAlive)
			{
				int distance = ennemy->position.distanceTo(ship.position);
				if (distance < minDistance)
				{
					closest = ennemy;
					minDistance = distance;
				}
			}

			if (closest)
			{
				ship.fire(closest->position);
				this->command = join("FIRE", closest->position.x, closest->position.y);
			}
			else
			{
				this->command = "WAIT";
			}
		}
};

int main(int argc, char *argv[])
{
	srand(time(NULL));

	// game loop
	while (1)
	{	
		Board board;
		board.init(cin);
		board.affect();

		// TODO : Monte Carlo, random , and just evaluate basic score
		// Then MinMax or GA ?
		int bestScore = -10000000;
		ShipStrategy* bestStrategies[3] = { nullptr, nullptr, nullptr };
		for (int s = 0; s < SIMULS; ++s)
		{
			int estimatedScore = 0;
			// Create a copy
			Board simulatedBoard(board);

			// Iterate over 5 turns
			for (int t = 0; t < TURNS; ++t)
			{
				int k = 0;
				for (Ship* ship : simulatedBoard.players[1].shipsAlive)
				{
					int stratId = rand() % 2;
					ShipStrategy* strat = nullptr;
					switch (rand())
					{
					case 0:
						strat = new ClosestBarrel();
						break;
					case 1:
						strat = new ClosestEnnemy();
						break;
					default:
					}					
					++k;
				}				
			}
			Coord coord(rand() % MAP_WIDTH, rand() % MAP_HEIGHT);
			// Get random action over 1 turn to start
			simulatedBoard.players[1].ships[0]->moveTo(coord);
			simulatedBoard.applyActions();
				
			int myScore = simulatedBoard.players[1].getScore();
			int opponentScore = simulatedBoard.players[0].getScore();
			int diff = myScore - opponentScore;
				
			estimatedScore += diff * 100;
			//cerr << "Scores : " << myScore << " / " << opponentScore << endl;

			// TODO : can template this
			// Find closest barrel
			int minDistance = 5000;
			RumBarrel* closest = nullptr;
			for (RumBarrel* barrel : simulatedBoard.barrels)
			{
				int distance = simulatedBoard.players[1].ships[0]->position.distanceTo(barrel->position);
				if (distance < minDistance)
				{
					closest = barrel;
					minDistance = distance;
				}
			}
				
			if (closest)
			{
				estimatedScore -= minDistance;
					
			}
			//cerr << "closest : " << minDistance << endl;

			if (estimatedScore > bestScore)
			{
				choice = coord;
				bestScore = estimatedScore;
			}
		}	





		//// V1
		//int minDistance = 5000;
		//RumBarrel* closest = nullptr;
		//Board simulatedBoard(board);
		//for (RumBarrel* barrel : simulatedBoard.barrels)
		//{
		//	int distance = simulatedBoard.players[1].ships[0]->position.distanceTo(barrel->position);
		//	if (distance < minDistance)
		//	{
		//		closest = barrel;
		//		minDistance = distance;
		//	}
		//}
		//	
		//if (closest)
		//{
		//	cout << "MOVE " << closest->position.x << " " << closest->position.y << endl;
		//}
		//else
		//{
		//	cout << "WAIT" << endl;
		//}		

		// Write an action using cout. DON'T FORGET THE "<< endl"
		// To debug: cerr << "Debug messages..." << endl;
		// cout << "MOVE 11 10" << endl; 
		// Any valid action, such as "WAIT" or "MOVE x y"		
	}
}
