//#include <iostream>
//#include <time.h>
//#include <string> 
//#include <chrono>
//
//#pragma GCC optimize ("O3")
//
//using namespace std;
//
//// Genetic Specifics
//#define GENOME_SIZE 10
//#define POPULATION_SIZE 80
//#define SIMULATION_NUMBER 400
//
//// Game Specifics
//#define MAX_ENTITIES 200
//#define BOMB_COOLDOWN 8
//#define MIN_BOX 30
//#define MAX_BOX 65
//#define HEIGHT 11
//#define WIDTH 13
//
//#define MOVE_ACTION 0
//#define BOMB_ACTION 1
//
//#define TOP 0
//#define RIGHT 1
//#define BOTTOM 2
//#define LEFT 3
//
//// Board Specific
//#define GROUND 46
//#define BOX 48
//
//#define PLAYER 0
//#define BOMB 1
//
//#define BOX_SCORE 10
//#define BOMB_RESET 20
//#define KILL_SCORE 200
//#define PICK_SCORE 5
//
///*
//Genetic Algorithm
//Generic Template
//*/
//
//// B = Board
//// G = Genome
//template<typename B, typename G, size_t S>
//class GeneticAlgorithm
//{
//private:
//	//Parameters
//	const size_t populationSize;
//	const size_t generationNumbers;
//
//	//Intern attributes
//	G population[S];
//
//	//Intern functions
//	inline void generate()
//	{
//		for (int i = 0; i < S; ++i)
//		{
//			this->population[i].generate();
//		}
//	}
//
//	inline void evaluate(const B& board)
//	{
//		for (int i = 0; i < S / 2; ++i)
//		{
//			population[i].evaluate(board);
//		}
//	}
//
//	inline void select()
//	{
//		this->mergeSort(0, S - 1);
//	}
//	void mergeSort(size_t from, size_t to)
//	{
//		if (to != from)
//		{
//			int middle = (from + to) / 2;
//			mergeSort(from, middle);
//			mergeSort(middle + 1, to);
//			merge(from, middle, to);
//		}
//	}
//	void merge(size_t from1, size_t to1, size_t to2)
//	{
//		G* tab = new G[(to1 - from1 + 1)]; //copy of the first array
//		int from2 = to1 + 1;
//		int count1 = from1;
//		int count2 = from2;
//		int i;
//
//		// copy elements from beginning of this->population
//		for (i = from1; i <= to1; ++i) {
//			tab[i - from1] = this->population[i];
//		}
//
//		// merge both parts
//		for (i = from1; i <= to2; ++i)
//		{
//			if (count1 == from2) // first part elements all used
//				break; // all sorted
//			else if (count2 == (to2 + 1)) // second part elements all used
//			{ // copy elements from first part to the end of this->population
//				this->population[i] = tab[count1 - from1];
//				++count1;
//			}
//			else if (tab[count1 - from1] < this->population[count2]) // implies to define operator< on G
//			{ // add element from first part
//				this->population[i] = tab[count1 - from1];
//				++count1; // increment in first part
//			}
//			else
//			{ // add element from second part
//				this->population[i] = this->population[count2];
//				++count2; // increment in second part
//			}
//		}
//		delete[] tab;
//	}
//
//	inline void cross(size_t turn)
//	{
//		G crossed[S / 2];
//		int mutationChance = 75 - ((turn / (this->generationNumbers - 1.)) * 60); // Goes from 75 to 15
//		int i;
//
//		for (i = 0; i < S / 2; ++i) {
//			crossed[i] = G::cross(this->population[S / 2 + (rand() % S / 2)], this->population[S / 2 + (rand() % S / 2)]); //do the new
//			if (rand() % 100 <= mutationChance)crossed[i].mutate();
//		}
//		for (i = 0; i < S / 2; ++i) {
//			this->population[i] = crossed[i];
//		}
//	}
//
//public:
//	GeneticAlgorithm(size_t generationNumbers) : populationSize(S), generationNumbers(generationNumbers) {}
//	inline G run(const B& board)
//	{
//		this->generate();
//		//this->shift(); // to keep previous strategies ?
//		this->evaluate(board);
//		this->select();
//		for (int i = 0; i < this->generationNumbers; ++i)
//		{
//			this->cross(i);
//			this->evaluate(board);
//			this->select();
//		}
//		// Return the best
//		return this->population[S - 1];
//	}
//};
//
//
//class Gene
//{
//private:
//	size_t order; //only 2 orders for now : 0 is MOVE / 1 is BOMB
//	size_t direction; //only 4 directions
//public:
//	Gene() : order(0), direction(0) {}
//	inline size_t getOrder() const
//	{
//		return this->order;
//	}
//	inline void setOrder(size_t c)
//	{
//		this->order = c;
//	}
//	inline size_t getDirection() const
//	{
//		return this->direction;
//	}
//	inline void setDirection(size_t d)
//	{
//		this->direction = d;
//	}
//	inline void generate()
//	{
//		this->order = rand() % 2;
//		this->direction = rand() % 4;
//	}
//	inline string print(size_t myX, size_t myY)
//	{
//		string order;
//		switch (this->order)
//		{
//		case MOVE_ACTION:
//			order = "MOVE";
//			break;
//		case BOMB_ACTION:
//		default:
//			order = "BOMB";
//			break;
//		}
//
//		size_t x = myX;
//		size_t y = myY;
//		switch (this->direction)
//		{
//		case TOP:
//			if (y > 0) --y;
//			break;
//		case RIGHT:
//			if (x < WIDTH) ++x;
//			break;
//		case BOTTOM:
//			if (y < HEIGHT) ++y;
//			break;
//		case LEFT:
//		default:
//			if (x > 0) --x;
//			break;
//		}
//
//		return order + " " + to_string(x) + " " + to_string(y);
//	}
//};
//
//class Entity
//{
//private:
//	size_t type; // 0 : player / 1 : bomb
//	size_t owner; // player id
//	size_t position;
//	size_t param1; // player : bomb stock / bomb : turns left
//	size_t param2; // player : bombs range / bomb : bomb range		
//	bool isRemoved;
//	size_t reloadBombs; // for simulation
//public:
//	Entity() : type(0), owner(0), position(0), param1(0), param2(0), isRemoved(false), reloadBombs(0) {}
//	Entity(size_t type, size_t owner, size_t position, size_t param1, size_t param2) : type(type), owner(owner), position(position), param1(param1), param2(param2), isRemoved(false), reloadBombs(0) {}
//	inline void setType(size_t type) { this->type = type; }
//	inline void setOwner(size_t owner) { this->owner = owner; }
//	inline void setPosition(int x, int y) { this->position = x + WIDTH * y; }
//	inline void setParam1(size_t param1) { this->param1 = param1; }
//	inline void setParam2(size_t param2) { this->param2 = param2; }
//	inline void setIsRemoved(bool isRemoved) { this->isRemoved = isRemoved; }
//	inline void setReloadBombs(size_t reloadBombs) { this->reloadBombs = reloadBombs; }
//	inline size_t getType() const { return this->type; }
//	inline size_t getOwner() const { return this->owner; }
//	inline size_t getPosition() const { return this->position; }
//	inline size_t getParam1() const { return this->param1; }
//	inline size_t getParam2() const { return this->param2; }
//	inline bool getIsRemoved() const { return this->isRemoved; }
//	inline size_t getReloadBombs() const { return this->reloadBombs; }
//	inline void reloadBombsCount()
//	{
//		this->param1 += this->reloadBombs;
//		this->reloadBombs = 0;
//	}
//};
//
//class Board {
//private:
//	size_t grid[WIDTH*HEIGHT]; // BOX, GROUND
//	Entity entities[MAX_ENTITIES];
//	size_t entitiesNumber;
//	size_t myId;
//	size_t myEntityId;
//public:
//	Board(size_t myId) : entitiesNumber(0), myId(myId), myEntityId(MAX_ENTITIES) {}
//	inline void setCase(size_t key, size_t value)
//	{
//		this->grid[key] = value;
//	}
//	inline size_t getCase(int x, int y)
//	{
//		return this->grid[x + WIDTH*y];
//	}
//	inline void addEntity(const Entity& entity)
//	{
//		this->entities[this->entitiesNumber] = entity;
//		++this->entitiesNumber;
//	}
//	inline void removeEntity(size_t key)
//	{
//		this->entities[key].setIsRemoved(true);
//	}
//	inline Entity& getEntity(size_t key)
//	{
//		return this->entities[key];
//	}
//
//	size_t findEntityId(size_t type, size_t id)
//	{
//		size_t result = MAX_ENTITIES;
//		for (int i = 0; i < this->entitiesNumber; ++i)
//		{
//			if (this->entities[i].getType() == type && this->entities[i].getOwner() == id)
//			{
//				result = i;
//			}
//		}
//		return result;
//	}
//
//	Entity& getMyEntity()
//	{
//		if (this->myEntityId == MAX_ENTITIES)
//			this->myEntityId = findEntityId(PLAYER, this->myId);
//		return this->entities[this->myEntityId];
//	}
//
//	int evaluate(const Gene& gene)
//	{
//		//Run gene on board
//		int score = 0;
//
//		bool hitGrid[WIDTH*HEIGHT] = { false };
//
//		//Start turn : decrement bombs and reload players
//		for (int i = 0; i < this->entitiesNumber; ++i)
//		{
//			if (this->entities[i].getType() == BOMB)
//			{
//				size_t timer = this->entities[i].getParam1();
//				this->entities[i].setParam1(--timer);
//			}
//			else // player
//			{
//				this->entities[i].reloadBombsCount();
//			}
//		}
//
//		//Explode bombs
//		bool reset = true;
//		while (reset)
//		{
//			reset = false;
//			for (int i = 0; i < this->entitiesNumber; ++i)
//			{
//				if (this->entities[i].getType() == BOMB)
//				{
//					size_t timer = this->entities[i].getParam1();
//					if (timer == 0)
//					{
//						//Deflegrate
//						score += this->deflegrate(hitGrid, i, reset);
//					}
//				}
//			}
//		}
//
//		// Remove boxes
//		for (size_t i = 0; i < WIDTH*HEIGHT; ++i)
//		{
//			if (hitGrid[i])
//				this->grid[i] = GROUND;
//		}
//
//		// Do Action
//		Entity& myEntity = getMyEntity();
//		switch (gene.getOrder())
//		{
//		case MOVE_ACTION:
//			//just move
//			this->moveTo(myEntity, gene.getDirection());
//			break;
//		case BOMB_ACTION:
//		default:
//			//put bomb then move
//			this->putBomb(myEntity);
//			this->moveTo(myEntity, gene.getDirection());
//			break;
//		}
//
//		// Find out if new position is better :
//		// Have more box in range : N * BOXES_SCORE
//		// If closest is far, privilegiate reducing distance with it
//		// Later : picking a packet ?
//		// Later: check if someone die
//
//		return score;
//	}
//
//	int deflegrate(bool hitGrid[], size_t i, bool& reset)
//	{
//		Entity& bomb = this->entities[i];
//		int score = 0;
//		if (!bomb.getIsRemoved())
//		{
//			// remove bomb as it explodes
//			bomb.setIsRemoved(true);
//
//			// explode in each direction
//			size_t range = bomb.getParam2();
//			size_t dist = 0;
//			size_t x = bomb.getPosition() % WIDTH;
//			size_t y = bomb.getPosition() / WIDTH;
//
//			//top
//			while (dist < range)
//			{
//				size_t newY = y - dist;
//				if (newY >= 0)
//				{
//					bool stop = burnCase(hitGrid, x, newY, score, reset);
//					//stop if obstacle
//					if (stop)
//						break;
//				}
//				++dist;
//			}
//			dist = 0;
//			//bottom
//			while (dist < range)
//			{
//				size_t newY = y + dist;
//				if (newY < HEIGHT)
//				{
//					bool stop = burnCase(hitGrid, x, newY, score, reset);
//					//stop if obstacle
//					if (stop)
//						break;
//				}
//				++dist;
//			}
//			dist = 0;
//			//left
//			while (dist < range)
//			{
//				size_t newX = x - dist;
//				if (newX >= 0)
//				{
//					bool stop = burnCase(hitGrid, newX, y, score, reset);
//					//stop if obstacle
//					if (stop)
//						break;
//				}
//				++dist;
//			}
//			dist = 0;
//			//right
//			while (dist < range)
//			{
//				size_t newX = x + dist;
//				if (newX < WIDTH)
//				{
//					bool stop = burnCase(hitGrid, newX, y, score, reset);
//					//stop if obstacle
//					if (stop)
//						break;
//				}
//				++dist;
//			}
//		}
//
//		//reverse score if not my bomb
//		if (bomb.getOwner() != this->myId)
//			score *= -1;
//		return score;
//	}
//
//	bool burnCase(bool hitGrid[], size_t x, size_t y, int& score, bool& reset)
//	{
//		bool stop = false;
//		if (this->grid[x + WIDTH*y] == BOX)
//		{
//			hitGrid[x + WIDTH*y] = true;
//			stop = true;
//			score += BOX_SCORE;
//		}
//		else
//		{
//			//maybe an entity or player
//			for (int i = 0; i < this->entitiesNumber; ++i)
//			{
//				Entity& entity = this->entities[i];
//				if (!entity.getIsRemoved()) //maybe check this only for players, not for bomb, but add check of  timer for bomb to know if reset combo or simultaneous
//				{
//					if (entity.getType() == BOMB)
//					{
//						//Maybe problems for simultaneous explosions x..x will stop both x on this direction, see later
//						entity.setParam1(0);
//						stop = true;
//						reset = true;
//						//if bomb is mine, i got reset, it's cool
//						if (entity.getOwner() == this->myId)
//							score += BOMB_RESET;
//						size_t ownerEntId = this->findEntityId(PLAYER, entity.getOwner());
//						Entity& owner = this->getEntity(ownerEntId);
//						owner.setReloadBombs(owner.getReloadBombs() + 1);
//					}
//					else //if (entity.getType() == PLAYER)
//					{
//						entity.setIsRemoved(true); //kill player
//					}
//				}
//			}
//		}
//		return stop;
//	}
//
//	void moveTo(Entity& entity, size_t direction)
//	{
//		size_t x = entity.getPosition() % WIDTH;
//		size_t y = entity.getPosition() / WIDTH;
//
//		switch (direction)
//		{
//		case TOP:
//			if (y > 0) --y;
//			break;
//		case RIGHT:
//			if (x < WIDTH) ++x;
//			break;
//		case BOTTOM:
//			if (y < HEIGHT) ++y;
//			break;
//		case LEFT:
//		default:
//			if (x > 0) --x;
//			break;
//		}
//
//		if (this->getCase(x, y) == GROUND)
//		{
//			bool empty = true;
//			for (int i = 0; i < this->entitiesNumber; ++i)
//			{
//				const Entity& ent = this->entities[i];
//				if (ent.getPosition() == (x + WIDTH * y) && ent.getType() == BOMB && ent.getParam1() < BOMB_COOLDOWN)
//				{
//					empty = false;
//				}
//			}
//
//			// if empty, move to new case
//			if (empty)
//			{
//				entity.setPosition(x, y);
//			}
//		}
//	}
//
//	void putBomb(Entity& entity)
//	{
//		if (entity.getParam1() > 0) //if we have some bombs available
//		{
//			Entity bomb(BOMB, entity.getOwner(), entity.getPosition(), BOMB_COOLDOWN, entity.getParam2());
//			this->addEntity(bomb);
//			entity.setParam1(entity.getParam1() - 1);
//		}
//	}
//};
//
//class Genome {
//private:
//	Gene genes[GENOME_SIZE];
//	int score;
//public:
//	Genome() : score(0) {}
//	int getScore() const
//	{
//		return this->score;
//	}
//	void setScore(int score)
//	{
//		this->score = score;
//	}
//
//	// Required by GeneticAlgorithm template
//	void generate()
//	{
//		for (int i = 0; i < GENOME_SIZE; ++i)
//		{
//			this->genes[i].generate();
//		}
//		this->score = 0;
//	}
//	static Genome cross(const Genome& g1, const Genome& g2)
//	{
//		Genome tmp;
//		// Random crossing
//		for (int i = 0; i < GENOME_SIZE; ++i) {
//			tmp.genes[i] = rand() % 2 ? g1.genes[i] : g2.genes[i];
//		}
//		return tmp;
//	}
//	void mutate()
//	{
//		// 1 random Gene is regenerated
//		this->genes[rand() % GENOME_SIZE].generate();
//	}
//	bool operator<(const Genome& g)
//	{
//		return this->score < g.score;
//	}
//	void evaluate(const Board& b)
//	{
//		int score = 0;
//		Board evaluatingBoard = b;
//		// Iterate on each turn
//		for (int i = 0; i < GENOME_SIZE; ++i)
//		{
//			score += evaluatingBoard.evaluate(this->genes[i]);
//		}
//		this->score = score;
//	}
//
//	// To print
//	inline string printFirstGene(size_t myX, size_t myY)
//	{
//		return this->genes[0].print(myX, myY);
//	}
//};
//
//inline void load(int height, int width, Board& board)
//{
//	for (int i = 0; i < height; ++i) {
//		string row;
//		getline(cin, row);
//
//		for (int j = 0; j < width; ++j)
//		{
//			board.setCase(j + WIDTH*i, row[j]);
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
//		Entity entity(entityType, owner, x + WIDTH*y, param1, param2);
//		board.addEntity(entity);
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
//			board.setCase(j + WIDTH*i, row[j]);
//		}
//	}
//
//	Entity entity(0, 0, 2 + WIDTH * 9, 1, 3);
//	board.addEntity(entity);
//
//	entity = Entity(0, 1, 10 + WIDTH * 9, 0, 3);
//	board.addEntity(entity);
//
//	/*entity = Entity(1, 1, 10 + WIDTH * 9, 8, 3);
//	board.addEntity(entity);
//
//	entity = Entity(1, 0, 5 + WIDTH * 8, 5, 3);
//	board.addEntity(entity);
//	*/
//}
//
//int main() {
//	srand(time(NULL));
//	GeneticAlgorithm<Board, Genome, POPULATION_SIZE> ga(SIMULATION_NUMBER);
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
//		auto start_time = chrono::high_resolution_clock::now();
//		Genome winner = ga.run(board);
//		auto end_time = chrono::high_resolution_clock::now();
//		cerr << "Execution time : " << chrono::duration_cast<chrono::milliseconds>(end_time - start_time).count() << " ms" << endl;
//
//		const Entity& myEntity = board.getMyEntity();
//		cout << winner.printFirstGene(myEntity.getPosition() % WIDTH, myEntity.getPosition() / WIDTH) << endl;
//	}
//}