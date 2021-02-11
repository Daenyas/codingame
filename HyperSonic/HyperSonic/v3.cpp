//#include <iostream>
//#include <time.h>
//#include <string> 
//#include <chrono>
//#include <vector>
//
//#pragma GCC optimize ("O3")
//
//using namespace std;
//
//// Game Specifics
//#define BOMB_COOLDOWN 8
//#define MIN_BOX 30
//#define MAX_BOX 65
//#define HEIGHT 11
//#define WIDTH 13
//
//#define ACTION_MOVE 0
//#define ACTION_BOMB 1
//
//#define DIRECTION_TOP 0
//#define DIRECTION_RIGHT 1
//#define DIRECTION_BOTTOM 2
//#define DIRECTION_LEFT 3
//#define DIRECTION_CENTER 4
//
//// Board Specifics
//#define GRID_GROUND 46
//#define GRID_BOX 48
//#define GRID_BOX_RANGE 49
//#define GRID_BOX_BOMB 50
//#define GRID_WALL 88
//
//#define ENTITY_PLAYER 0
//#define ENTITY_BOMB 1
//#define ENTITY_OBJECT 2
//
//#define EXTRA_RANGE 1
//#define EXTRA_BOMB 2
//
//// Evaluation Specifics
//#define DEPTH 12
//#define TIME_LIMIT 95 //ms
//
//class Order {
//public:
//	char action; //2
//	char direction; //5
//
//	Order() : action(0), direction(0) {}
//	Order(char a, char d) : action(a), direction(d) {}
//	Order(const Order& order) : action(order.action), direction(order.direction) {}
//
//	Order& operator=(const Order& order)
//	{
//		this->action = order.action;
//		this->direction = order.direction;
//		return *this;
//	}
//
//	void generate()
//	{
//		this->action = rand() % 2;
//		this->direction = rand() % 5;
//	}
//
//	void toString(char x, char y)
//	{
//		string action;
//		switch (this->action)
//		{
//		case ACTION_BOMB:
//			action = "BOMB";
//			break;
//		case ACTION_MOVE:
//		default:
//			action = "MOVE";
//			break;
//		}
//
//		char newX = x;
//		char newY = y;
//		switch (this->direction)
//		{
//		case DIRECTION_BOTTOM:
//			if (newY < HEIGHT - 1) ++newY;
//			break;
//		case DIRECTION_TOP:
//			if (newY > 0) --newY;
//			break;
//		case DIRECTION_LEFT:
//			if (newX > 0) --newX;
//			break;
//		case DIRECTION_RIGHT:
//			if (newX < WIDTH - 1) ++newX;
//			break;
//		case DIRECTION_CENTER:
//		default:
//			break;
//		}
//
//		cout << action << " " << (int)newX << " " << (int)newY;
//	}
//};
//
//class Entity
//{
//public:
//	char type; // 0 : player / 1 : bomb
//	char owner; // player id
//	char x;
//	char y;
//	char param1; // player : bomb stock / bomb : turns left
//	char param2; // player : bombs range / bomb : bomb range
//	bool dead;
//	char bombsToReload;
//	char boxesScored;
//	char extraScored;
//
//	Entity(char type, char owner, char x, char y, char param1, char param2) : type(type), owner(owner), x(x), y(y), param1(param1), param2(param2), dead(false), bombsToReload(0), boxesScored(0), extraScored(0) {}
//	Entity(const Entity& entity) : type(entity.type), owner(entity.owner), x(entity.x), y(entity.y), param1(entity.param1), param2(entity.param2), dead(entity.dead), bombsToReload(entity.bombsToReload), boxesScored(entity.boxesScored), extraScored(entity.extraScored) {}
//	Entity& operator=(const Entity& entity)
//	{
//		this->type = entity.type;
//		this->owner = entity.owner;
//		this->x = entity.x;
//		this->y = entity.y;
//		this->param1 = entity.param1;
//		this->param2 = entity.param2;
//		this->dead = entity.dead;
//		this->bombsToReload = entity.bombsToReload;
//		this->boxesScored = entity.boxesScored;
//		this->extraScored = entity.extraScored;
//		return *this;
//	}
//	~Entity() {}
//};
//
//class Board {
//public:
//	char myId;
//	char grid[WIDTH*HEIGHT]; // BOX, GROUND
//	vector<Entity> entities;
//
//	Board(char myId) : myId(myId), entities(), grid() {
//		this->entities.reserve(200 * sizeof(Entity));
//	}
//
//	Board(const Board& board) : myId(board.myId), entities(board.entities), grid()
//	{
//		for (int i = 0; i < WIDTH * HEIGHT; ++i)
//		{
//			this->grid[i] = board.grid[i];
//		}
//	}
//
//	Board& operator=(const Board& board)
//	{
//		this->myId = board.myId;
//
//		for (int i = 0; i < WIDTH * HEIGHT; ++i)
//		{
//			this->grid[i] = board.grid[i];
//		}
//
//		this->entities.clear();
//		for (auto it = board.entities.begin(); it != board.entities.end(); ++it)
//		{
//			this->entities.push_back(*it);
//		}
//
//		return *this;
//	}
//
//	~Board() {}
//
//	Entity* getMyPlayer()
//	{
//		return this->getPlayerById(this->myId);
//	}
//
//	Entity* getPlayerById(char id)
//	{
//		Entity* result = nullptr;
//		for (auto it = this->entities.begin(); it != this->entities.end(); ++it)
//		{
//			if (it->type == ENTITY_PLAYER && it->owner == id)
//			{
//				result = &(*it);
//				break;
//			}
//		}
//		return result;
//	}
//
//	void runTurn(bool hitGrid[], const int turn)
//	{
//		// 1) Decrement bombs
//		for (auto it = this->entities.begin(); it != this->entities.end(); ++it)
//		{
//			if (it->type == ENTITY_BOMB)
//			{
//				--(it->param1);
//			}
//		}
//
//		// 2) Explode bombs
//		bool reset = true;
//		while (reset)
//		{
//			reset = false;
//			for (auto it = this->entities.begin(); it != this->entities.end(); ++it)
//			{
//				if (it->type == ENTITY_BOMB && it->param1 == 0 && !it->dead)
//				{
//					this->explode(*it, hitGrid, turn, reset);
//					it->dead = true;
//					// Find owner to reload
//					for (auto it2 = this->entities.begin(); it2 != this->entities.end(); ++it2)
//					{
//						if (it2->type == ENTITY_PLAYER && it2->owner == it->owner)
//						{
//							++(it2->bombsToReload);
//							break;
//						}
//					}
//				}
//			}
//		}
//	}
//
//	void clean()
//	{
//		// Remove bombs and players / objects
//		for (auto it = this->entities.begin(); it != this->entities.end();)
//		{
//			if (it->dead)
//			{
//				it = entities.erase(it);
//			}
//			else
//			{
//				++it;
//			}
//		}
//	}
//
//	void play(vector<Entity>& newBombs, const Order& order, Entity& playerEntity, const int turn)
//	{
//		// 3) Do action
//		if (order.action == ACTION_BOMB)
//		{
//			if (playerEntity.param1 > 0)
//			{
//				newBombs.push_back(Entity(ENTITY_BOMB, playerEntity.owner, playerEntity.x, playerEntity.y, BOMB_COOLDOWN, playerEntity.param2));
//				--playerEntity.param1;
//			}
//		}
//
//		char newX = playerEntity.x;
//		char newY = playerEntity.y;
//
//		switch (order.direction)
//		{
//		case DIRECTION_BOTTOM:
//			if (newY < HEIGHT - 1) ++newY;
//			break;
//		case DIRECTION_TOP:
//			if (newY > 0) --newY;
//			break;
//		case DIRECTION_LEFT:
//			if (newX > 0) --newX;
//			break;
//		case DIRECTION_RIGHT:
//			if (newX < WIDTH - 1) ++newX;
//			break;
//		case DIRECTION_CENTER:
//		default:
//			break;
//		}
//		// Always move if possible
//		if (this->grid[newX + WIDTH*newY] == GRID_GROUND)
//		{
//			bool blocked = false;
//			for (auto it = this->entities.begin(); it != this->entities.end(); ++it)
//			{
//				if (it->type == ENTITY_BOMB && it->x == newX && it->y == newY) {
//					blocked = true;
//					break;
//				}
//			}
//			if (!blocked)
//			{
//				playerEntity.x = newX;
//				playerEntity.y = newY;
//			}
//		}
//
//		// Get object if there is one
//		for (auto it = this->entities.begin(); it != this->entities.end(); ++it)
//		{
//			if (it->type == ENTITY_OBJECT && it->x == playerEntity.x && it->y == playerEntity.y)
//			{
//				switch (it->param1)
//				{
//				case EXTRA_RANGE:
//					++(playerEntity.param2);
//					break;
//				case EXTRA_BOMB:
//					++(playerEntity.param1);
//					break;
//				default:
//					break;
//				}
//				playerEntity.extraScored += (DEPTH - turn);
//				it->dead = true;
//			}
//		}
//	}
//
//	void endTurn(const vector<Entity>& newBombs, bool hitGrid[])
//	{
//		// 4) Destroy boxes
//		// 4') Object appears
//		for (int i = 0; i < WIDTH * HEIGHT; ++i)
//		{
//			if (hitGrid[i])
//			{
//				switch (this->grid[i])
//				{
//				case GRID_BOX_BOMB:
//					this->entities.push_back(Entity(ENTITY_OBJECT, 0, i % WIDTH, i / WIDTH, 2, 0));
//					break;
//				case GRID_BOX_RANGE:
//					this->entities.push_back(Entity(ENTITY_OBJECT, 0, i % WIDTH, i / WIDTH, 1, 0));
//					break;
//				case GRID_BOX:
//				default:
//					// Nothing
//					break;
//				}
//				this->grid[i] = GRID_GROUND;
//			}
//		}
//
//		// 5) Bomb appears
//		for (auto it = newBombs.begin(); it != newBombs.end(); ++it)
//		{
//			this->entities.push_back(*it);
//		}
//
//		// 6) Reload bombs that exploded
//		for (auto it = this->entities.begin(); it != this->entities.end(); ++it)
//		{
//			it->param1 += it->bombsToReload;
//			it->bombsToReload = 0;
//		}
//	}
//
//	void explode(const Entity& bomb, bool hitGrid[], const int turn, bool& reset)
//	{
//		// explode in each direction
//		char range = bomb.param2;
//		char x = bomb.x;
//		char y = bomb.y;
//
//		Entity* owner = this->getPlayerById(bomb.owner);
//
//		//center
//		burnCase(hitGrid, x, y, owner, turn, reset);
//
//		char dist = 1;
//		//top
//		while (dist < range)
//		{
//			char newY = y - dist;
//			if (newY >= 0)
//			{
//				bool stop = burnCase(hitGrid, x, newY, owner, turn, reset);
//				//stop if obstacle
//				if (stop)
//					break;
//			}
//			++dist;
//		}
//		dist = 1;
//		//bottom
//		while (dist < range)
//		{
//			char newY = y + dist;
//			if (newY < HEIGHT)
//			{
//				bool stop = burnCase(hitGrid, x, newY, owner, turn, reset);
//				//stop if obstacle
//				if (stop)
//					break;
//			}
//			++dist;
//		}
//		dist = 1;
//		//left
//		while (dist < range)
//		{
//			char newX = x - dist;
//			if (newX >= 0)
//			{
//				bool stop = burnCase(hitGrid, newX, y, owner, turn, reset);
//				//stop if obstacle
//				if (stop)
//					break;
//			}
//			++dist;
//		}
//		dist = 1;
//		//right
//		while (dist < range)
//		{
//			char newX = x + dist;
//			if (newX < WIDTH)
//			{
//				bool stop = burnCase(hitGrid, newX, y, owner, turn, reset);
//				//stop if obstacle
//				if (stop)
//					break;
//			}
//			++dist;
//		}
//	}
//
//	bool burnCase(bool hitGrid[], char x, char y, Entity* owner, const int turn, bool& reset)
//	{
//		bool stop = false;
//		if (this->grid[x + WIDTH*y] != GRID_GROUND && this->grid[x + WIDTH*y] != GRID_WALL)
//		{
//			hitGrid[x + WIDTH*y] = true;
//			stop = true;
//
//			// If owner not dead
//			if (owner != nullptr)
//				owner->boxesScored += (DEPTH - turn);
//		}
//		else if (this->grid[x + WIDTH*y] == GRID_WALL)
//		{
//			stop = true;
//		}
//		else
//		{
//			//Maybe an entity or player
//			for (auto it = this->entities.begin(); it != this->entities.end(); ++it)
//			{
//				if (it->x == x && it->y == y)
//				{
//					switch (it->type)
//					{
//					case ENTITY_BOMB:
//						// Will explode on reset ! (dead)
//						it->param1 = 0;
//						stop = true;
//						reset = true;
//						break;
//					case ENTITY_PLAYER:
//						it->dead = true;
//						break;
//					case ENTITY_OBJECT:
//					default:
//						stop = true;
//						it->dead = true;
//						break;
//					}
//				}
//			}
//		}
//		return stop;
//	}
//
//	int evaluateState()
//	{
//		int score = 0;
//
//		// Check if I am still in
//		Entity* myEntity = this->getMyPlayer();
//		if (myEntity == nullptr || myEntity->dead)
//		{
//			return -1000000;
//		}
//		// Else, check how much I scored
//		score = 10 * (int)(myEntity->boxesScored);
//		score += 5 * (int)(myEntity->extraScored);
//
//		return score;
//	}
//};
//
//
//class MonteCarlo {
//public:
//	Order bestOrders[DEPTH];
//	double bestScore;
//	int count;
//
//	Board simulationBoard;
//	Order simulationOrders[DEPTH];
//
//	bool hitGrid[WIDTH * HEIGHT];
//
//	MonteCarlo() : bestOrders(), bestScore(0), count(0), simulationBoard(0), simulationOrders(), hitGrid() {}
//
//	void resetHitGrid()
//	{
//		for (int i = 0; i < WIDTH*HEIGHT; ++i)
//		{
//			this->hitGrid[i] = false;
//		}
//	}
//
//	void run(const Board& board)
//	{
//		this->bestScore = -1;
//		this->count = 0;
//
//		// Shift last best
//		for (int i = 0; i < DEPTH - 1; ++i)
//		{
//			this->bestOrders[i] = this->bestOrders[i + 1];
//		}
//
//		auto start_time = chrono::high_resolution_clock::now();
//		auto end_time = chrono::high_resolution_clock::now();
//
//		while (chrono::duration_cast<chrono::milliseconds>(end_time - start_time).count() < TIME_LIMIT)
//		{
//			++(this->count);
//			// Get a simulation board
//			this->simulationBoard = board;
//
//			Entity* myEntity = this->simulationBoard.getMyPlayer();
//
//			// Simulate random moves
//			for (int i = 0; i < DEPTH; ++i)
//			{
//				// Generate a random move
//				this->generate(i);
//				//this->fakeGenerate();
//
//				// Simulate my moves (no opponent for now)
//				vector<Entity> newBombs;
//
//				this->resetHitGrid();
//				this->simulationBoard.runTurn(this->hitGrid, i);
//
//				// Do not continue if I die, worst case possible
//				if (myEntity->dead)
//				{
//					break;
//				}
//
//				// Clean board (bomb + players)
//				this->simulationBoard.clean();
//
//				// Play my turn (should look at opponents one as well with a dummy AI)
//				this->simulationBoard.play(newBombs, this->simulationOrders[i], *myEntity, i);
//
//				// Clean board (objects)
//				this->simulationBoard.clean();
//
//				// End the turn by destroying boxes and placing bombs behind us
//				this->simulationBoard.endTurn(newBombs, this->hitGrid);
//			}
//
//			// Estimate score
//			int score = this->simulationBoard.evaluateState();
//			if (score > this->bestScore)
//			{
//				for (int i = 0; i < DEPTH; ++i)
//				{
//					this->bestOrders[i] = this->simulationOrders[i];
//				}
//				this->bestScore = score;
//			}
//
//			// Check time spent
//			end_time = chrono::high_resolution_clock::now();
//		}
//	}
//
//	void generate(const int i)
//	{
//		if (this->count < 15 && i < DEPTH - 1)
//		{
//			this->simulationOrders[i] = this->bestOrders[i];
//		}
//		else
//		{
//			this->simulationOrders[i].generate();
//		}
//	}
//
//	void fakeGenerate()
//	{
//		this->simulationOrders[0] = Order(1, 1);
//		this->simulationOrders[1] = Order(0, 0);
//		this->simulationOrders[2] = Order(0, 2);
//		this->simulationOrders[3] = Order(1, 3);
//		this->simulationOrders[4] = Order(0, 2);
//		this->simulationOrders[5] = Order(1, 3);
//		this->simulationOrders[6] = Order(0, 3);
//		this->simulationOrders[7] = Order(1, 0);
//		this->simulationOrders[8] = Order(0, 4);
//		this->simulationOrders[9] = Order(0, 2);
//		this->simulationOrders[10] = Order(0, 2);
//		this->simulationOrders[11] = Order(1, 4);
//	}
//};
//
//inline void load(int height, int width, Board& board)
//{
//	board.entities.clear();
//
//	for (int i = 0; i < height; ++i) {
//		string row;
//		getline(cin, row);
//
//		for (int j = 0; j < width; ++j)
//		{
//			board.grid[j + WIDTH*i] = row[j];
//			cerr << board.grid[j + WIDTH*i];
//		}
//		cerr << endl;
//	}
//
//	int entities;
//	cin >> entities; cin.ignore();
//	for (int i = 0; i < entities; ++i) {
//		int entityType;
//		int owner;
//		int x;
//		int y;
//		int param1;
//		int param2;
//		cin >> entityType >> owner >> x >> y >> param1 >> param2; cin.ignore();
//
//		Entity entity(entityType, owner, x, y, param1, param2);
//		board.entities.push_back(entity);
//	}
//}
//
//inline void fakeLoad(int height, int width, Board& board)
//{
//	board.entities.clear();
//
//	string rows[HEIGHT];
//	rows[0] = "..122...221..";
//	rows[1] = ".X.X.X1X.X.X.";
//	rows[2] = ".000.....000.";
//	rows[3] = ".X2X0X.X0X2X.";
//	rows[4] = ".10112.21101.";
//	rows[5] = ".X2X.X.X.X2X.";
//	rows[6] = ".10112.21101.";
//	rows[7] = ".X2X0X.X0X2X.";
//	rows[8] = ".000.....000.";
//	rows[9] = ".X.X.X1X.X.X.";
//	rows[10] = "..122...221..";
//
//	for (int i = 0; i < height; ++i) {
//		string row = rows[i];
//
//		for (int j = 0; j < width; ++j)
//		{
//			board.grid[j + WIDTH*i] = row[j];
//			cerr << row[j] << endl;
//		}
//	}
//
//	Entity entity(0, 0, 12, 9, 0, 3);
//	board.entities.push_back(entity);
//
//	entity = Entity(0, 1, 0, 4, 0, 3);
//	board.entities.push_back(entity);
//
//	entity = Entity(1, 0, 12, 10, 2, 3);
//	board.entities.push_back(entity);
//
//	entity = Entity(1, 1, 1, 0, 3, 0);
//	board.entities.push_back(entity);
//}
//
//int main() {
//	srand(time(NULL));
//
//	MonteCarlo mc;
//
//	int width;
//	int height;
//	int myId;
//	cin >> width >> height >> myId; cin.ignore();
//
//	Board board(myId);
//	// game loop
//	while (1) {
//		//load(height, width, board);
//		fakeLoad(height, width, board);
//
//		mc.run(board);
//		Entity* myEntity = board.getMyPlayer();
//		if (myEntity == nullptr)
//		{
//			cout << "MOVE 0 0 I lost :'(" << endl;
//		}
//		else
//		{
//			mc.bestOrders[0].toString(myEntity->x, myEntity->y);
//			cout << " " << mc.bestScore << " / " << mc.count << endl;
//
//			cerr << "Choices : " << endl;
//			for (int i = 0; i < DEPTH; ++i)
//			{
//				cerr << (int)mc.bestOrders[i].action << " " << (int)mc.bestOrders[i].direction << endl;
//			}
//		}
//	}
//}