#include <iostream>
#include <time.h>
#include <chrono>
#include <math.h>
#include <algorithm>

#pragma GCC optimize ("O3,inline,omit-frame-pointer")

using namespace std;

// Dimensions
#define WIDTH 16000;
#define HEIGHT 9000;

// Game Interactions
#define WOLFF_MAX_MOVE 1000
#define ENNEMY_MAX_MOVE 500
#define ENNEMY_DEATH_RANGE 2000

#define MOVE_COMMAND 0
#define SHOOT_COMMAND 1

// MC
#define DEPTH 10
#define TIME_LIMIT 0.095f
#define SHIFT_BUCKET_SIZE 20
#define BUCKET_SIZE 100

// Scoring
#define DEATH_SCORE -1000000
#define DATA_SCORE 100
#define KILL_SCORE 10

template<typename T, unsigned char S>
class Vector
{
	public:
		T items[S];
		unsigned char count;
		
		Vector() : count(0), items() {}
		Vector(const Vector& vector) : count(vector.count)
		{
			//TODO : maybe use memcpy as we know size of T????
			for (int i = 0; i < vector.count; ++i)
			{
				this->items[i] = vector.items[i];
			}
		}
		Vector& operator=(const Vector& vector)
		{
			this->count = vector.count;
			//TODO : maybe use memcpy as we know size of T????
			for (int i = 0; i < vector.count; ++i)
			{
				this->items[i] = vector.items[i];
			}
			return *this;
		}

		inline void add(const T& item)
		{
			this->items[this->count] = item;
			++(this->count);
		}
};

class Point
{
	public:
		short int x;
		short int y;
		
		Point() : x(-1), y(-1) {}
		Point(int x, int y) : x(x), y(y) {}
		Point(const Point& point) : x(point.x), y(point.y) {}
		Point& operator=(const Point& point)
		{
			this->x = point.x;
			this->y = point.y;
			return *this;
		}

		inline static double distance(const Point& point1, const Point& point2)
		{
			return sqrt((point1.x - point2.x)*(point1.x - point2.x) + (point1.y - point2.y)*(point1.y - point2.y));
		}
};

class Data
{
	public:
		Point position;
		char id;
		
		Data() : position(), id(-1) {}
		Data(const Point& position, char id) : position(position), id(id) {}
		Data(const Data& data) : position(data.position), id(data.id) {}
		Data& operator=(const Data& data)
		{
			this->position = data.position;
			this->id = data.id;
			return *this;
		}
};

class Ennemy
{
	public:
		Point position;
		char id;
		char target;	
		unsigned char life;

		Ennemy() : position(), id(-1), target(-1), life(0) {}
		Ennemy(const Point& position, char id, unsigned char life) : position(position), id(id), target(-1), life(life) {}
		Ennemy(const Ennemy& ennemy) : position(ennemy.position), id(ennemy.id), target(ennemy.target), life(ennemy.life) {}
		Ennemy& operator=(const Ennemy& ennemy)
		{
			this->position = ennemy.position;
			this->id = ennemy.id;
			this->target = ennemy.target;
			this->life = ennemy.life;
			return *this;
		}
};

class Player
{
	public:
		Point position;

		Player() : position() {}
		Player(const Player& player) : position(player.position) {}
		Player& operator=(const Player& player)
		{
			this->position = player.position;
			return *this;
		}
};

class Command
{
	public:
		char order; // MOVE or SHOOT
		short int x; // x or id
		short int y; // y or -1

		Command() : order(0), x(0), y(0) {}

		void print(const Ennemy ennemies[])
		{
			if (order == MOVE_COMMAND)
			{
				cout << "MOVE " << this->x << " " << this->y;
			}
			else
			{
				cout << "SHOOT " << (int)(ennemies[this->x].id);
			}			
		}

		void generate(unsigned char ennemiesCount)
		{
			this->order = rand() % 2;
			if (this->order == MOVE_COMMAND)
			{
				this->x = rand() % WIDTH;
				this->y = rand() % HEIGHT;
			}
			else
			{
				this->x = ennemiesCount > 0 ? rand() % ennemiesCount : -1;
				this->y = -1;
			}
		}
};

class Board
{
	public:
		Player player;
		Vector<Data, 10> dataPoints; 
		Vector<Ennemy, 20> ennemies;
		int startingLife;
		int shotCount;

		Board() : player(), dataPoints(), ennemies(), startingLife(-1), shotCount(-1) {}
		Board(const Board& board) : player(board.player), dataPoints(board.dataPoints), ennemies(board.ennemies), startingLife(board.startingLife), shotCount(board.shotCount) {}
		Board& operator=(const Board& board)
		{
			this->player = board.player;
			this->dataPoints = board.dataPoints;
			this->ennemies = board.ennemies;
			this->startingLife = board.startingLife;
			this->shotCount = board.shotCount;
			return *this;
		}

		int run(const Command& command)
		{
			if (command.order == MOVE_COMMAND && command.x == -1)
			{
				// Invalid command
				return 1;
			}

			// if game over, stop simulation : return 2;

			// 1 : Enemies move towards their targets.
			for (int i = 0; i < this->ennemies.count; ++i)
			{
				this->findTargetAndMove(this->ennemies.items[i]);
			}
			// 2 : If a MOVE command was given, Wolff moves towards his target.
			// 3 : Game over if an enemy is close enough to Wolff.
			// 4 : If a SHOOT command was given, Wolff shoots an enemy.
			// 5 : Enemies with zero life points are removed from play.
			// 6 : Enemies collect data points they share coordinates with.



			return 0;
		}

		void findTargetAndMove(Ennemy& ennemy)
		{
			// Find target
			double minDistance = 30000;
			double distance = 0;
			for (int i = 0; i < this->dataPoints.count; ++i)
			{
				distance = Point::distance(ennemy.position, this->dataPoints[i].position);
				if (distance < minDistance)
				{
					minDistance = distance;
					ennemy.target = i;
				}
			}
			azezae
			// Move to target
			//this->dataPoints.items[ennemy.target];
			//ennemy.position = **;

		}

		int evaluate(const Board& referenceBoard, const Command commandsList[])
		{
			int score = 0;

			int kills = (referenceBoard.ennemies.count - this->ennemies.count);

			int aliveDataPoints = this->dataPoints.count;			
			
			int shotCount = 0;
			for (int i = 0; i < DEPTH; ++i)
			{
				if (commandsList[i].order == SHOOT_COMMAND)
				{
					++shotCount;
				}
			}			

			int formula = aliveDataPoints * max(0, (this->startingLife - 3 * (shotCount + this->shotCount))) * 3;

			score = (kills * KILL_SCORE) + (aliveDataPoints * DATA_SCORE) + formula;

			return score;
		}
};

class MonteCarlo
{
	public:
		Command bestCommands[DEPTH];
		int bestScore;
		int count;

		Board simulationBoard;
		Command simulationCommands[DEPTH];

		MonteCarlo() {}
		void run(const Board& board)
		{
			this->bestScore = DEATH_SCORE;
			this->count = 0;

			// Shift last best
			for (int i = 0; i < DEPTH - 1; ++i)
			{
				this->bestCommands[i] = this->bestCommands[i + 1];
			}

			chrono::duration<float> duration;
			auto start_time = chrono::system_clock::now();
			duration = chrono::system_clock::now() - start_time;

			while (duration.count() < TIME_LIMIT)
			{
				// Split in buckets
				for (int buckets = 0; buckets < BUCKET_SIZE; ++buckets)
				{
					++(this->count);

					// Get a simulation board
					this->simulationBoard = board;

					int commandResult = 0;

					// Simulate random moves
					for (int i = 0; i < DEPTH; ++i)
					{
						// Generate a random move
						this->generate(i);

						// Run turn
						commandResult = this->simulationBoard.run(this->simulationCommands[i]);
						// 1 : invalid command / 2 : game is over
						if (commandResult > 0)
						{
							break;
						}
					}

					// 1 means invalid command, so don't keep this run
					if (commandResult == 1)
					{
						continue;
					}

					// Evaluate score
					int score = this->simulationBoard.evaluate(board, this->simulationCommands);

					if (score > this->bestScore)
					{
						for (int i = 0; i < DEPTH; ++i)
						{
							this->bestCommands[i] = this->simulationCommands[i];
						}
						this->bestScore = score;
					}
				}

				// Check time spent
				duration = chrono::system_clock::now() - start_time;
			}
		
		}

		inline void generate(const int i)
		{
			if (this->count < SHIFT_BUCKET_SIZE && i < DEPTH - 1)
			{
				this->simulationCommands[i] = this->bestCommands[i];
			}
			else
			{
				this->simulationCommands[i].generate(this->simulationBoard.ennemies.count);
			}
		}
};

inline void load(Board& board, istream& input)
{
	int x;
	int y;
	input >> x >> y; input.ignore();
	cerr << x << " " << y << endl;
	board.player.position = Point(x, y);

	int dataCount;
	input >> dataCount; input.ignore();
	cerr << dataCount << endl;
	for (int i = 0; i < dataCount; ++i) {
		int dataId;
		int dataX;
		int dataY;
		input >> dataId >> dataX >> dataY; input.ignore();
		cerr << dataId << " " << dataX << " " << dataY << endl;
		board.dataPoints.add(Data(Point(dataX, dataY), dataId));
	}
	int enemyCount;
	input >> enemyCount; input.ignore();
	cerr << enemyCount << endl;
	for (int i = 0; i < enemyCount; i++) {
		int enemyId;
		int enemyX;
		int enemyY;
		int enemyLife;
		input >> enemyId >> enemyX >> enemyY >> enemyLife; input.ignore();
		cerr << enemyId << " " << enemyX << " " << enemyY << " " << enemyLife << endl;
		board.ennemies.add(Ennemy(Point(enemyX, enemyY), enemyId, enemyLife));
	}
}

int main()
{
	srand(time(NULL));

	Board board = Board();
	MonteCarlo evaluator = MonteCarlo();

	bool first = true;
	// game loop
	while (1) {
		load(board, cin);
		if (first)
		{
			first = false;
			board.shotCount = 0;
			int lifeTotal = 0;
			for (int i = 0; i < board.ennemies.count; ++i)
			{
				lifeTotal += board.ennemies.items[i].life;
			}
			board.startingLife = lifeTotal;
		}

		evaluator.run(board);


		evaluator.bestCommands[0].print(board.ennemies.items);
		cout << evaluator.bestScore << " / " << evaluator.count << endl;

		if (evaluator.bestCommands[0].order == SHOOT_COMMAND)
		{
			++(board.shotCount);
		}
	}
}