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
//
//#define ENTITY_PLAYER 0
//#define ENTITY_BOMB 1
//
//// Evaluation Specifics
//#define DEPTH 12
//#define TIME_LIMIT 95 //ms
//
//class Order {
//public:
//	char action; //2
//	char direction; //5
//	Order() : action(0), direction(0) {}
//	void generate()
//	{
//		this->action = rand() % 2;
//		this->direction = rand() % 5;
//	}
//
//	string toString(char x, char y)
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
//			if (newY < HEIGHT) ++newY;
//			break;
//		case DIRECTION_TOP:
//			if (newY > 0) --newY;
//			break;
//		case DIRECTION_LEFT:
//			if (newX > 0) --newX;
//			break;
//		case DIRECTION_RIGHT:
//			if (newX < WIDTH) ++newX;
//			break;
//		case DIRECTION_CENTER:
//		default:
//			break;
//		}
//
//		return action + " " + to_string((int)newX) + " " + to_string((int)newY);
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
//
//	Entity(char type, char owner, char x, char y, char param1, char param2) : type(type), owner(owner), x(x), y(y), param1(param1), param2(param2), dead(false), bombsToReload(0), boxesScored(0) {}
//	Entity(const Entity& entity) : type(entity.type), owner(entity.owner), x(entity.x), y(entity.y), param1(entity.param1), param2(entity.param2), dead(entity.dead), bombsToReload(entity.bombsToReload), boxesScored(entity.boxesScored) {}
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
//	Board(char myId) : myId(myId), entities() {}
//
//	Board(const Board& board) : myId(board.myId), entities(board.entities)
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
//		for (int i = 0; i < WIDTH * HEIGHT; ++i)
//		{
//			this->grid[i] = board.grid[i];
//		}
//		this->entities.clear();
//		for (auto it = board.entities.begin(); it != board.entities.end(); ++it)
//		{
//			this->entities.push_back(*it);
//		}
//	}
//
//	~Board() {}
//
//	Entity* getMyEntity()
//	{
//		return this->getEntityById(this->myId);
//	}
//
//	Entity* getEntityById(char id)
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
//	void runTurn(bool hitGrid[])
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
//					this->explode(*it, hitGrid, reset);
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
//		// Remove bombs and players
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
//	void play(vector<Entity>& newBombs, const Order& order, Entity& playerEntity)
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
//			if (newY < HEIGHT) ++newY;
//			break;
//		case DIRECTION_TOP:
//			if (newY > 0) --newY;
//			break;
//		case DIRECTION_LEFT:
//			if (newX > 0) --newX;
//			break;
//		case DIRECTION_RIGHT:
//			if (newX < WIDTH) ++newX;
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
//	}
//
//	void endTurn(const vector<Entity>& newBombs, bool hitGrid[])
//	{
//		// 4) Destroy boxes
//		for (int i = 0; i < WIDTH * HEIGHT; ++i)
//		{
//			if (hitGrid[i])
//			{
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
//	void explode(const Entity& bomb, bool hitGrid[], bool& reset)
//	{
//		// explode in each direction
//		char range = bomb.param2;
//		char dist = 0;
//		char x = bomb.x;
//		char y = bomb.y;
//
//		Entity* owner = this->getEntityById(bomb.owner);
//
//		//top
//		while (dist < range)
//		{
//			char newY = y - dist;
//			if (newY >= 0)
//			{
//				bool stop = burnCase(hitGrid, x, newY, owner, reset);
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
//				bool stop = burnCase(hitGrid, x, newY, owner, reset);
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
//				bool stop = burnCase(hitGrid, newX, y, owner, reset);
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
//				bool stop = burnCase(hitGrid, newX, y, owner, reset);
//				//stop if obstacle
//				if (stop)
//					break;
//			}
//			++dist;
//		}
//	}
//
//	bool burnCase(bool hitGrid[], char x, char y, Entity* owner, bool& reset)
//	{
//		bool stop = false;
//		if (this->grid[x + WIDTH*y] == GRID_BOX)
//		{
//			hitGrid[x + WIDTH*y] = true;
//			++(owner->boxesScored);
//			stop = true;
//		}
//		else
//		{
//			//Maybe an entity or player
//			for (auto it = this->entities.begin(); it != this->entities.end(); ++it)
//			{
//				if (it->x == x && it->y == y)
//				{
//					if (it->type == ENTITY_BOMB)
//					{
//						// Will explode on reset !
//						it->param1 = 0;
//						stop = true;
//						reset = true;
//					}
//					else //if (entity.getType() == PLAYER)
//					{
//						it->dead = true;
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
//		Entity* myEntity = this->getMyEntity();
//		if (myEntity == nullptr)
//		{
//			return -1000000;
//		}
//		// Else, check how much I scored
//		score = 10 * myEntity->boxesScored;
//
//		return score;
//	}
//};
//
//
//class MonteCarlo {
//public:
//	Order bestOrder;
//	double bestScore;
//	MonteCarlo() : bestScore(0) {}
//
//	void run(const Board& board)
//	{
//		this->bestScore = 0;
//
//		auto start_time = chrono::high_resolution_clock::now();
//		auto end_time = chrono::high_resolution_clock::now();
//
//		while (chrono::duration_cast<chrono::milliseconds>(end_time - start_time).count() < TIME_LIMIT)
//		{
//			// Get a simulation board
//			Board simulationBoard(board);
//
//			Order simulationOrder;
//			Order firstSimulationOrder;
//			Entity* myEntity = simulationBoard.getMyEntity();
//
//			// Simulate random moves
//			for (int i = 0; i < DEPTH; ++i)
//			{
//				// Generate a random move
//				simulationOrder.generate();
//
//				// Save first one
//				if (i == 0)
//				{
//					firstSimulationOrder = simulationOrder;
//				}
//
//				// Simulate my moves (no opponent for now)
//				vector<Entity> newBombs;
//				bool hitGrid[WIDTH * HEIGHT] = { false };
//				simulationBoard.runTurn(hitGrid);
//
//				// Do not continue if I die, worst case possible
//				if (myEntity->dead)
//				{
//					break;
//				}
//
//				// Clean board (bomb + players)
//				simulationBoard.clean();
//
//				// Play my turn (should look at opponents one as well with a dummy AI)
//				simulationBoard.play(newBombs, simulationOrder, *myEntity);
//
//				// End the turn by destroying boxes and placing bombs behind us
//				simulationBoard.endTurn(newBombs, hitGrid);
//			}
//
//			// Estimate score
//			int score = simulationBoard.evaluateState();
//			if (score > this->bestScore)
//			{
//				this->bestOrder = firstSimulationOrder;
//				this->bestScore = score;
//			}
//
//			// Check time spent
//			end_time = chrono::high_resolution_clock::now();
//		}
//	}
//};
//
//
//inline void load(int height, int width, Board& board)
//{
//	for (int i = 0; i < height; ++i) {
//		string row;
//		getline(cin, row);
//
//		for (int j = 0; j < width; ++j)
//		{
//			board.grid[j + WIDTH*i] = row[j];
//		}
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
//	string rows[HEIGHT];
//	rows[0] = "...0.....0...";
//	rows[1] = ".0...0.....0.";
//	rows[2] = ".............";
//	rows[3] = "..0.......0.0";
//	rows[4] = "....0.0.0....";
//	rows[5] = "0.0.......0.0";
//	rows[6] = "....0.0.0....";
//	rows[7] = "..........0.0";
//	rows[8] = ".............";
//	rows[9] = ".....0.0...0.";
//	rows[10] = ".............";
//
//	for (int i = 0; i < height; ++i) {
//		string row = rows[i];
//
//		for (int j = 0; j < width; ++j)
//		{
//			board.grid[j + WIDTH*i] = row[j];
//		}
//	}
//
//	Entity entity(0, 0, 2, 9, 1, 3);
//	board.entities.push_back(entity);
//
//	entity = Entity(0, 1, 10, 9, 0, 3);
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
//	// game loop
//	while (1) {
//		Board board(myId);
//
//		//load(height, width, board);
//		fakeLoad(height, width, board);
//
//		mc.run(board);
//		Entity* myEntity = board.getMyEntity();
//		if (myEntity == nullptr)
//		{
//			cout << "MOVE 0 0 I lost :'(" << endl;
//		}
//		else
//		{
//			cout << mc.bestOrder.toString(myEntity->x, myEntity->y) << " " << mc.bestScore << endl;
//		}
//	}
//}