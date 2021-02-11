#include <iostream>
#include <time.h>
#include <string> 
#include <chrono>
#include <math.h>

#pragma GCC optimize ("O3,inline,omit-frame-pointer")

using namespace std;

// Game Specifics
#define BOMB_COOLDOWN 8
#define MIN_BOX 30
#define MAX_BOX 65
#define HEIGHT 11
#define WIDTH 13

#define ACTION_MOVE 0
#define ACTION_BOMB 1

#define DIRECTION_TOP 0
#define DIRECTION_RIGHT 1
#define DIRECTION_BOTTOM 2
#define DIRECTION_LEFT 3
#define DIRECTION_CENTER 4

// Board Specifics
#define GRID_GROUND 46
#define GRID_BOX 48
#define GRID_BOX_RANGE 49
#define GRID_BOX_BOMB 50
#define GRID_WALL 88

#define ENTITY_PLAYER 0
#define ENTITY_BOMB 1
#define ENTITY_OBJECT 2

#define EXTRA_RANGE 1
#define EXTRA_BOMB 2

// Evaluation Specifics
#define DEPTH 12
#define TIME_LIMIT 0.095f //95 ms

#define MAX_ENTITIES 200
#define CHUNK_SIZE 500

class Order {
public:
	char action; //2
	char direction; //5

	Order() : action(0), direction(0) {}
	Order(char a, char d) : action(a), direction(d) {}
	Order(const Order& order) : action(order.action), direction(order.direction) {}
	
	Order& operator=(const Order& order)
	{
		this->action = order.action;
		this->direction = order.direction;
		return *this;
	}

	void generate()
	{
		this->action = rand() % 2;
		this->direction = rand() % 5;
	}

	void toString(char x, char y)
	{
		string action;
		switch (this->action)
		{
			case ACTION_BOMB:
				action = "BOMB";
				break;
			case ACTION_MOVE:
			default:
				action = "MOVE";
				break;
		}

		char newX = x;
		char newY = y;
		switch (this->direction)
		{
			case DIRECTION_BOTTOM:
				if (newY < HEIGHT - 1) ++newY;
				break;
			case DIRECTION_TOP:
				if (newY > 0) --newY;
				break;
			case DIRECTION_LEFT:
				if (newX > 0) --newX;
				break;
			case DIRECTION_RIGHT:
				if (newX < WIDTH - 1) ++newX;
				break;
			case DIRECTION_CENTER:
			default:
				break;
		}

		cout << action << " " << (int)newX << " " << (int)newY;
	}
};

class Entity
{
public:
	char type; // 0 : player / 1 : bomb
	char owner; // player id
	char x;
	char y;
	char param1; // player : bomb stock / bomb : turns left
	char param2; // player : bombs range / bomb : bomb range
	bool dead;
	char bombsToReload;
	char boxesScored;
	char extraScored;

	Entity() : type(-1), owner(-1), x(-1), y(-1), param1(-1), param2(-1), dead(false), bombsToReload(-1), boxesScored(-1), extraScored(-1) {}
	Entity(char type, char owner, char x, char y, char param1, char param2) : type(type), owner(owner), x(x), y(y), param1(param1), param2(param2), dead(false), bombsToReload(0), boxesScored(0), extraScored(0) {}
	Entity(const Entity& entity) : type(entity.type), owner(entity.owner), x(entity.x), y(entity.y), param1(entity.param1), param2(entity.param2), dead(entity.dead), bombsToReload(entity.bombsToReload), boxesScored(entity.boxesScored), extraScored(entity.extraScored) {}
	Entity& operator=(const Entity& entity)
	{
		this->type = entity.type;
		this->owner = entity.owner;
		this->x = entity.x;
		this->y = entity.y;
		this->param1 = entity.param1;
		this->param2 = entity.param2;
		this->dead = entity.dead;
		this->bombsToReload = entity.bombsToReload;
		this->boxesScored = entity.boxesScored; 
		this->extraScored = entity.extraScored;
		return *this;
	}
	~Entity() {}
};

class Board {
public:
	char myId;
	char grid[WIDTH*HEIGHT]; // BOX, GROUND
	Entity entities[MAX_ENTITIES];
	char entitiesNumber;
	char zonesCount[4];

	Board(char myId) : myId(myId), entities(), grid(), entitiesNumber(0) {
	}

	Board(const Board& board) : myId(board.myId), entities(), grid(), entitiesNumber(board.entitiesNumber)
	{
		for (int i = 0; i < WIDTH * HEIGHT; ++i)
		{
			this->grid[i] = board.grid[i];
		}
		for (int i = 0; i < board.entitiesNumber; ++i)
		{
			this->entities[i] = board.entities[i];
		}
	}

	Board& operator=(const Board& board)
	{
		this->myId = board.myId;

		for (int i = 0; i < WIDTH * HEIGHT; ++i)
		{
			this->grid[i] = board.grid[i];
		}

		for (int i = 0; i < board.entitiesNumber; ++i)
		{
			this->entities[i] = board.entities[i];
		}
		this->entitiesNumber = board.entitiesNumber;

		return *this;
	}

	~Board() {}

	inline void addEntity(const Entity& entity)
	{
		this->entities[this->entitiesNumber] = entity;
		(++this->entitiesNumber);
	}

	Entity* getMyPlayer()
	{
		return this->getPlayerById(this->myId);
	}

	Entity* getPlayerById(char id)
	{
		Entity* result = nullptr;
		for (int i = 0; i < this->entitiesNumber; ++i)
		{
			if (this->entities[i].type == ENTITY_PLAYER && this->entities[i].owner == id)
			{
				result = &(this->entities[i]);
				break;
			}
		}		
		return result;
	}

	void runTurn(bool hitGrid[], const int turn)
	{
		// 1) Decrement bombs
		for (int i = 0; i < this->entitiesNumber; ++i)
		{
			if (this->entities[i].type == ENTITY_BOMB)
			{
				--(this->entities[i].param1);
			}
		}

		// 2) Explode bombs
		bool reset = true;
		while (reset)
		{
			reset = false;
			for (int i = 0; i < this->entitiesNumber; ++i)
			{
				if (this->entities[i].type == ENTITY_BOMB && this->entities[i].param1 == 0 && !this->entities[i].dead)
				{
					this->explode(this->entities[i], hitGrid, turn, reset);
					this->entities[i].dead = true;
					// Find owner to reload
					for (int j = 0; j < this->entitiesNumber; ++j)
					{
						if (this->entities[j].type == ENTITY_PLAYER && this->entities[j].owner == this->entities[i].owner)
						{
							++(this->entities[j].bombsToReload);
							break;
						}							
					}
				}
			}
		}
	}

	void clean()
	{
		// Remove bombs and players / objects
		for (int i = 0; i < this->entitiesNumber; ++i)
		{
			if (this->entities[i].dead)
			{
				for (int j = i; j < this->entitiesNumber - 1; ++j)
				{
					this->entities[j] = this->entities[j + 1];
				}
				--(this->entitiesNumber);
			}
		}	
	}

	void play(Entity newBombs[], char& newBombsNumber, const Order& order, Entity& playerEntity, const int turn)
	{
		// 3) Do action
		if (order.action == ACTION_BOMB)
		{
			if (playerEntity.param1 > 0)
			{
				newBombs[newBombsNumber] = Entity(ENTITY_BOMB, playerEntity.owner, playerEntity.x, playerEntity.y, BOMB_COOLDOWN, playerEntity.param2);
				++newBombsNumber;
				--playerEntity.param1;
			}
		}

		char newX = playerEntity.x;
		char newY = playerEntity.y;

		switch (order.direction)
		{
			case DIRECTION_BOTTOM:
				if (newY < HEIGHT - 1) ++newY;
				break;
			case DIRECTION_TOP:
				if (newY > 0) --newY;
				break;
			case DIRECTION_LEFT:
				if (newX > 0) --newX;
				break;
			case DIRECTION_RIGHT:
				if (newX < WIDTH - 1) ++newX;
				break;
			case DIRECTION_CENTER:
			default:
				break;
		}
		// Always move if possible
		if (this->grid[newX + WIDTH*newY] == GRID_GROUND)
		{
			bool blocked = false;
			for (int i = 0; i < this->entitiesNumber; ++i)
			{
				if (this->entities[i].type == ENTITY_BOMB && this->entities[i].x == newX && this->entities[i].y == newY) {
					blocked = true;
					break;
				}
			}
			if (!blocked)
			{
				playerEntity.x = newX;
				playerEntity.y = newY;
			}
		}

		// Get object if there is one
		for (int i = 0; i < this->entitiesNumber; ++i)
		{
			if (this->entities[i].type == ENTITY_OBJECT && this->entities[i].x == playerEntity.x && this->entities[i].y == playerEntity.y)
			{
				switch (this->entities[i].param1)
				{
					case EXTRA_RANGE:
						++(playerEntity.param2);
						break;
					case EXTRA_BOMB:
						++(playerEntity.param1);
						break;
					default:
						break;				
				}
				playerEntity.extraScored += (DEPTH - turn);
				this->entities[i].dead = true;
			}
		}
	}

	void endTurn(const Entity newBombs[], const char newBombsNumber, bool hitGrid[])
	{
		// 4) Destroy boxes
		// 4') Object appears
		for (int i = 0; i < WIDTH * HEIGHT; ++i)
		{
			if(hitGrid[i])
			{
				switch (this->grid[i])
				{
					case GRID_BOX_BOMB:
						this->addEntity(Entity(ENTITY_OBJECT, 0, i % WIDTH, i / WIDTH, 2, 0));
						break;
					case GRID_BOX_RANGE:
						this->addEntity(Entity(ENTITY_OBJECT, 0, i % WIDTH, i / WIDTH, 1, 0));
						break;
					case GRID_BOX:
					default:
						// Nothing
						break;
				}
				this->grid[i] = GRID_GROUND;
			}
		}

		// 5) Bomb appears
		for (int i = 0; i < newBombsNumber; ++i)
		{
			this->addEntity(newBombs[i]);
		}

		// 6) Reload bombs that exploded
		for (int i = 0; i < this->entitiesNumber; ++i)
		{
			this->entities[i].param1 += this->entities[i].bombsToReload;
			this->entities[i].bombsToReload = 0;
		}
	}

	void explode(const Entity& bomb, bool hitGrid[], const int turn, bool& reset)
	{
		// explode in each direction
		char range = bomb.param2;
		char x = bomb.x;
		char y = bomb.y;

		Entity* owner = this->getPlayerById(bomb.owner);
		
		//center
		burnCase(hitGrid, x, y, owner, turn, reset);

		char dist = 1;
		//top
		while (dist < range)
		{
			char newY = y - dist;
			if (newY >= 0)
			{
				bool stop = burnCase(hitGrid, x, newY, owner, turn, reset);
				//stop if obstacle
				if (stop)
					break;
			}
			++dist;
		}
		dist = 1;
		//bottom
		while (dist < range)
		{
			char newY = y + dist;
			if (newY < HEIGHT)
			{
				bool stop = burnCase(hitGrid, x, newY, owner, turn, reset);
				//stop if obstacle
				if (stop)
					break;
			}
			++dist;
		}
		dist = 1;
		//left
		while (dist < range)
		{
			char newX = x - dist;
			if (newX >= 0)
			{
				bool stop = burnCase(hitGrid, newX, y, owner, turn, reset);
				//stop if obstacle
				if (stop)
					break;
			}
			++dist;
		}
		dist = 1;
		//right
		while (dist < range)
		{
			char newX = x + dist;
			if (newX < WIDTH)
			{
				bool stop = burnCase(hitGrid, newX, y, owner, turn, reset);
				//stop if obstacle
				if (stop)
					break;
			}
			++dist;
		}
	}

	bool burnCase(bool hitGrid[], char x, char y, Entity* owner, const int turn, bool& reset)
	{
		bool stop = false;
		if (this->grid[x + WIDTH*y] != GRID_GROUND && this->grid[x + WIDTH*y] != GRID_WALL)
		{
			hitGrid[x + WIDTH*y] = true;
			stop = true;

			// If owner not dead
			if (owner != nullptr)
				owner->boxesScored += (DEPTH - turn);
		}
		else if (this->grid[x + WIDTH*y] == GRID_WALL)
		{
			stop = true;
		}
		else
		{
			//Maybe an entity or player
			for (int i = 0; i < this->entitiesNumber; ++i)
			{				
				if (this->entities[i].x == x && this->entities[i].y == y)
				{
					switch (this->entities[i].type)
					{
						case ENTITY_BOMB:
							// Will explode on reset ! (dead)
							this->entities[i].param1 = 0;
							stop = true;
							reset = true;
							break;
						case ENTITY_PLAYER:
							this->entities[i].dead = true;
							break;
						case ENTITY_OBJECT:
						default:
							stop = true;
							this->entities[i].dead = true;
							break;
					}
				}				
			}
		}
		return stop;
	}

	int evaluateState()
	{
		int score = 0;
		
		// Check if I am still in
		Entity* myEntity = this->getMyPlayer();
		if (myEntity == nullptr || myEntity->dead)
		{
			return -1000000;
		}
		// Else, check how much I scored
		score = 15 * (int) (myEntity->boxesScored);
		score += 5 * (int) (myEntity->extraScored);

		// Check if I kill someone ? (extend it for 3 or 4 players)
		/*Entity* ennemyEntity = this->getPlayerById(1 - this->myId);
		if (ennemyEntity == nullptr || ennemyEntity->dead)
		{
			score += 5000;
		}*/

		// Get the one with the most boxes
		char boxNumber = -1;
		char maxCount = 0;
		for (int i = 0; i < 4; ++i)
		{
			if (this->zonesCount[i] > maxCount)
			{
				boxNumber = i;
				maxCount = this->zonesCount[i];
			}
		}

		// If there is still some boxes
		// Better if we are near the central points of the region
		char dist = 0;
		switch (boxNumber)
		{
			case 0:
				dist = this->getDistanceTo(*myEntity, 3, 3);
				break;
			case 1:
				dist = this->getDistanceTo(*myEntity, 3, 8);
				break;
			case 2:
				dist = this->getDistanceTo(*myEntity, 10, 3);
				break;
			case 3:
				dist = this->getDistanceTo(*myEntity, 10, 8);
				break;
			case -1:
			default:
				break;
				//do nothing			
		}
		score -= 10 * dist;
		

		return score;
	}	

	void countBoxes()
	{
		this->zonesCount[0] = 0;
		this->zonesCount[1] = 0;
		this->zonesCount[2] = 0;
		this->zonesCount[3] = 0;

		char i = 0;
		char j;
		while (i <= WIDTH/2)
		{
			j = 0;
			while (j <= HEIGHT / 2)
			{
				++(this->zonesCount[0]);
				++j;
			}
			while (j < HEIGHT)
			{
				++(this->zonesCount[1]);
				++j;
			}
			++i;
		}
		while (i < WIDTH)
		{
			j = 0;
			while (j <= HEIGHT / 2)
			{
				++(this->zonesCount[2]);
				++j;
			}
			while (j < HEIGHT)
			{
				++(this->zonesCount[3]);
				++j;
			}
			++i;
		}
	}

	inline char getDistanceTo(const Entity& entity, char x, char y)
	{
		return abs(entity.x - x) + abs(entity.y - y);
	}
};


class MonteCarlo {
public:
	Order bestOrders[DEPTH];
	double bestScore;
	int count;

	Board simulationBoard;
	Order simulationOrders[DEPTH];

	bool hitGrid[WIDTH * HEIGHT];

	MonteCarlo() : bestOrders(), bestScore(0), count(0), simulationBoard(0), simulationOrders(), hitGrid() {}

	void resetHitGrid()
	{
		for (int i = 0; i < WIDTH*HEIGHT; ++i)
		{
			this->hitGrid[i] = false;
		}
	}

	void run(const Board& board)
	{
		this->bestScore = -100000;
		this->count = 0;

		// Shift last best
		for (int i = 0; i < DEPTH - 1; ++i)
		{
			this->bestOrders[i] = this->bestOrders[i + 1];
		}

		chrono::duration<float> duration;
		auto start_time = chrono::system_clock::now();
		duration = chrono::system_clock::now() - start_time;

		Entity newBombs[4];
		char newBombsNumber = 0;

		while (duration.count() < TIME_LIMIT)
		{
			for (int chunk = 0; chunk < CHUNK_SIZE; ++chunk)
			{
				++(this->count);
				// Get a simulation board
				this->simulationBoard = board;

				// Count boxes in 4 regions
				this->simulationBoard.countBoxes();

				Entity* myEntity = this->simulationBoard.getMyPlayer();

				// Simulate random moves
				for (int i = 0; i < DEPTH; ++i)
				{
					// Generate a random move
					this->generate(i);
					//this->fakeGenerate();

					// Simulate my moves (no opponent for now)
					// reset newBombs holder
					newBombsNumber = 0;

					this->resetHitGrid();
					this->simulationBoard.runTurn(this->hitGrid, i);

					// Do not continue if I die, worst case possible
					if (myEntity->dead)
					{
						break;
					}

					// Clean board (bomb + players)
					this->simulationBoard.clean();

					// Play my turn (should look at opponents one as well with a dummy AI)
					this->simulationBoard.play(newBombs, newBombsNumber, this->simulationOrders[i], *myEntity, i);

					// Clean board (objects)
					this->simulationBoard.clean();

					// End the turn by destroying boxes and placing bombs behind us
					this->simulationBoard.endTurn(newBombs, newBombsNumber, this->hitGrid);
				}

				// Estimate score
				int score = this->simulationBoard.evaluateState();
				if (score > this->bestScore)
				{
					for (int i = 0; i < DEPTH; ++i)
					{
						this->bestOrders[i] = this->simulationOrders[i];
					}
					this->bestScore = score;
				}

			}
			// Check time spent
			duration = chrono::system_clock::now() - start_time;
		}
	}

	void generate(const int i)
	{
		if (this->count < 15 && i < DEPTH - 1)
		{
			this->simulationOrders[i] = this->bestOrders[i];
		}
		else
		{
			this->simulationOrders[i].generate();
		}
	}

	void fakeGenerate()
	{		
		this->simulationOrders[0] =		Order(1, 1);
		this->simulationOrders[1] =		Order(0, 0);
		this->simulationOrders[2] =		Order(0, 2);
		this->simulationOrders[3] =		Order(1, 3);
		this->simulationOrders[4] =		Order(0, 2);
		this->simulationOrders[5] =		Order(1, 3);
		this->simulationOrders[6] =		Order(0, 3);
		this->simulationOrders[7] =		Order(1, 0);
		this->simulationOrders[8] =		Order(0, 4);
		this->simulationOrders[9] =		Order(0, 2);
		this->simulationOrders[10] =	Order(0, 2);
		this->simulationOrders[11] =	Order(1, 4);				
	}
};

inline void load(int height, int width, Board& board)
{
	board.entitiesNumber = 0;

	for (int i = 0; i < height; ++i) {
		string row;
		getline(cin, row);
		
		for (int j = 0; j < width; ++j)
		{
			board.grid[j + WIDTH*i] = row[j];
			cerr << board.grid[j + WIDTH*i];
		}
		cerr << endl;
	}

	int entities;
	cin >> entities; cin.ignore();
	for (int i = 0; i < entities; ++i) {
		int entityType;
		int owner;
		int x;
		int y;
		int param1;
		int param2;
		cin >> entityType >> owner >> x >> y >> param1 >> param2; cin.ignore();

		Entity entity(entityType, owner, x, y, param1, param2);
		board.addEntity(entity);
	}
}

inline void fakeLoad(int height, int width, Board& board)
{
	board.entitiesNumber = 0;

	string rows[HEIGHT];
	rows[0] = ".....2.2..1..";
	rows[1] = ".X.X1X0X1X0X.";
	rows[2] = ".....1.11..2.";
	rows[3] = ".X.X0X0X0X.X0";
	rows[4] = "....2.0.20.1.";
	rows[5] = ".X.X2X.X2X.X.";
	rows[6] = "21.02.0......";
	rows[7] = "0X.X0X0X0X.X.";
	rows[8] = ".2..11.1.....";
	rows[9] = ".X0X1X0X1X.X.";
	rows[10] = "..1..2.......";
	
	for (int i = 0; i < height; ++i) {
		string row = rows[i];

		for (int j = 0; j < width; ++j)
		{
			board.grid[j + WIDTH*i] = row[j];
			cerr << row[j] << endl;
		}
	}

	//players
	Entity entity(0, 0, 2, 2, 2, 3);
	board.addEntity(entity);

	entity = Entity(0, 1, 9, 6, 3, 3);
	board.addEntity(entity);

	//bombs
	entity = Entity(1, 0, 4, 2, 2, 3);
	board.addEntity(entity);

	entity = Entity(1, 1, 10, 5, 7, 3);
	board.addEntity(entity);

	//objs
	entity = Entity(2, 0, 10, 4, 2, 0);
	board.addEntity(entity);

	entity = Entity(2, 0, 8, 6, 2, 0);
	board.addEntity(entity);

	entity = Entity(2, 0, 8, 12, 4, 0);
	board.addEntity(entity);

	entity = Entity(2, 0, 8, 8, 1, 0);
	board.addEntity(entity);
}

int main() {
	srand(time(NULL));

	MonteCarlo mc;

	int width;
	int height;
	int myId;
	cin >> width >> height >> myId; cin.ignore();
	
	Board board(myId);
	// game loop
	while (1) {
		//load(height, width, board);
		fakeLoad(height, width, board);
		
		mc.run(board);
		Entity* myEntity = board.getMyPlayer();
		if (myEntity == nullptr)
		{
			cout << "MOVE 0 0 I lost :'(" << endl;
		}
		else
		{
			mc.bestOrders[0].toString(myEntity->x, myEntity->y);
			cout << " " << mc.bestScore << " / " << mc.count << endl;

			cerr << "Choices : " << endl;
			for (int i = 0; i < DEPTH; ++i)
			{				
				cerr << (int)mc.bestOrders[i].action << " " << (int)mc.bestOrders[i].direction << endl;
			}
		}		
	}
}