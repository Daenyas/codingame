#include "stdafx.h"

#include <iostream>
#include <string>

#include <chrono>

#define WIDTH 6
#define HEIGHT 12
#define MAX_BLOCK_POS 20
#define MAX_GROUP_NB 200

#define POP_SIZE 20 //Multiple de 4
#define GEN_SIZE 8
#define MAX_SIMUL 500

#pragma GCC push_options
#pragma GCC optimize ("Ofast")

struct Choice {
	int col;
	int rot;
};

using namespace std;
//4 --> 40 points

class Block {
private:
	int count;
	char color;
	int id;
	int positions[MAX_BLOCK_POS][2]; // Remove that and prevent too much calls on feeding TODO : refaire en liste chainee as pos does not matter
public:
	Block(int id) : id(id), count(0){}
	Block(const Block& block) : count(block.count), color(block.color), id(block.id) {
		for (int i = 0; i < MAX_BLOCK_POS; ++i) {
			this->positions[i][0] = block.positions[i][0];
			this->positions[i][1] = block.positions[i][1];
		}
	}
	
	Block& operator= (const Block& block) {
		this->count = block.count;
		this->color = block.color;
		this->id = block.id;
		for (int i = 0; i < MAX_BLOCK_POS; ++i) {
			this->positions[i][0] = block.positions[i][0];
			this->positions[i][1] = block.positions[i][1];
		}
		return *this;
	}

	void fill(const char& color, int i, int j, int id) {
		this->count = 0;
		this->color = color;
		this->id = id;
		addPos(i, j);
	}

	void reset() {
		this->count = 0;
		this->id = -1;
	}
	int getCount() { return this->count; }
	char getColor() { return this->color; }
	void addPos(int i, int j) {
		positions[count][0] = i;
		positions[count][1] = j;
		++this->count;		
	}
	void removePos(int i, int j) {
		int k = 0;
		for (; k < this->count; ++k) {
			if (this->positions[k][0] == i && this->positions[k][1] == j){
				--this->count;
				break;
			}
		}
		for (; k < this->count; ++k) {
			this->positions[k][0] = this->positions[k + 1][0];
			this->positions[k][1] = this->positions[k + 1][1];
		}
	}
	int get(int count, int i) { return this->positions[count][i]; }
	int getId() { return this->id; }
	bool pop(int& x, int& y) {
		if (this->count > 0) {
			--this->count;
			x = this->positions[this->count][0];
			y = this->positions[this->count][1];
			return true;
		}
		return false;
	}
};

//Class for a player Grid
class Grid {
private:
	char blocs[WIDTH][HEIGHT];
	bool visited[WIDTH][HEIGHT];
	Block* groups[MAX_GROUP_NB];
	int groupIdMax;
	int groupsId[WIDTH][HEIGHT];

public:
	Grid() : groupIdMax(0) {
		for (int x = 0; x<WIDTH; ++x) {
			for (int y = 0; y<HEIGHT; ++y) {
				this->blocs[x][y] = '.';
				this->visited[x][y] = false;
				this->groupsId[x][y] = -1;
			}
		}
		for (int i = 0; i < MAX_GROUP_NB; ++i) {
			this->groups[i] = new Block(i);
		}
	}
	Grid(const Grid& grid) {
		for (int x = 0; x<WIDTH; ++x) {
			for (int y = 0; y<HEIGHT; ++y) {
				this->blocs[x][y] = grid.blocs[x][y];
				this->visited[x][y] = grid.visited[x][y];
				this->groupsId[x][y] = grid.groupsId[x][y];
			}
		}
		this->groupIdMax = grid.groupIdMax;
		for (int i = 0; i < MAX_GROUP_NB; ++i) {
			this->groups[i] = new Block(*grid.groups[i]);
		}
	}
	Grid& operator= (const Grid& grid) {
		for (int x = 0; x<WIDTH; ++x) {
			for (int y = 0; y<HEIGHT; ++y) {
				this->blocs[x][y] = grid.blocs[x][y];
				this->visited[x][y] = grid.visited[x][y];
				this->groupsId[x][y] = grid.groupsId[x][y];
			}
		}
		this->groupIdMax = grid.groupIdMax;
		for (int i = 0; i < MAX_GROUP_NB; ++i) {
			*this->groups[i] = *grid.groups[i];
		}
		return *this;
	}
	~Grid() {
		for (int i = 0; i < MAX_GROUP_NB; ++i) {
			delete this->groups[i];
		}
	}
	char get(int x, int y) const {
		return this->blocs[x][y];
	}
	void set(int x, int y, char val) {
		this->blocs[x][y] = val;
		//valeur modifiée : groupe cassé a priori
		this->clearGroup(x, y);		
	}
	void clearGroup(int x, int y) {
		int id = this->groupsId[x][y];		
		if (id != -1) { //we had a group :
			Block * b = this->groups[id];
			int c = 0, d = 0;
			while (b->pop(c, d)) {
				this->groupsId[c][d] = -1;
				this->visited[c][d] = false;
			}
			//delete will be done in Grid destruction
		}
		else {
			this->visited[x][y] = false;
		}
	}
	int update();
	int findExplosion(); //rec
	int findExplosionSub(int& CP); //rec
	void down();
	void resetFirst() {
		for (int x = 0; x<WIDTH; ++x) {
			for (int y = 0; y<HEIGHT; ++y) {
				this->visited[x][y] = false;
				this->groupsId[x][y] = -1;
			}
		}
		for (int i = 0; i < MAX_GROUP_NB; ++i) {
			this->groups[i]->reset();
		}
		this->groupIdMax = 0;
	}

	void destroySkulls(int i, int j);

	void printGrid() {
		cerr << "Grid : " << endl;
		for (int j = HEIGHT-1; j >=0 ; --j) {
			for (int i = 0; i < WIDTH; ++i) {
				cerr << this->blocs[i][j];
			}
			cerr << endl;
		}
	}
	void fillBlocks();
	void checkNeighbors(Block* b, int i, int j);
	void updateNeighbor(Block* b, int i, int j);
	void fusion(int block1, int block2);
};

//Remplissage des blocks de la grille
void Grid::fillBlocks() {
	//this->printGrid();

	for (int i = 0; i < WIDTH; ++i) {
		for (int j = 0; j < HEIGHT; ++j) {
			if (!this->visited[i][j] && get(i, j) != '.' && get(i, j) != '0') {
				//Process point if not yet done
				Block* b = this->groups[this->groupIdMax];
				b->fill(blocs[i][j], i, j, this->groupIdMax);
				//Add to groups list
				this->groups[groupIdMax] = b;
				this->groupsId[i][j] = groupIdMax;
				++this->groupIdMax;
				this->visited[i][j] = true;
				this->checkNeighbors(b, i, j);				
			}
		}
	}	
}

//Remplissage des voisins
void Grid::checkNeighbors(Block* b, int i, int j) {
	char color = b->getColor();
	//middle position i, j
	if (i < WIDTH-1 && this->get(i + 1, j) == color) {
		updateNeighbor(b, i + 1, j);
	}
	if (i > 0 && this->get(i - 1, j) == color) {
		updateNeighbor(b, i - 1, j);
	}
	if (j < HEIGHT-1 && this->get(i, j + 1) == color) {
		updateNeighbor(b, i, j + 1);
	}
	if (j > 0 && this->get(i, j - 1) == color) {
		updateNeighbor(b, i, j - 1);
	}
}

void Grid::updateNeighbor(Block* b, int i, int j) {
	//same color in here !
	int groupId = b->getId();
	if (!this->visited[i][j]) {
		b->addPos(i, j);
		this->visited[i][j] = true;
		this->groupsId[i][j] = groupId;
		checkNeighbors(b, i, j);
	}
	else{
		//fusion des groups si different
		int groupId2 = this->groupsId[i][j];
		if(groupId != groupId2)
			this->fusion(groupId, groupId2);
	}
}

void Grid::fusion(int block1, int block2) {
	Block* b1 = this->groups[block1];
	Block* b2 = this->groups[block2];
	int x = 0, y = 0;
	while (b2->pop(x, y)) {
		b1->addPos(x, y);
		this->groupsId[x][y] = block1;
	}
	//delete will be done in destruction
}

void Grid::destroySkulls(int i, int j) {
	//middle position i, j
	if (i < WIDTH-1 && this->get(i + 1, j) == '0') {
		this->set(i + 1, j, '.');
	}
	if (i > 0 && this->get(i - 1, j) == '0') {
		this->set(i - 1, j, '.');
	}
	if (j < HEIGHT-1 && this->get(i, j + 1) == '0') {
		this->set(i, j + 1, '.');
	}
	if (j > 0 && this->get(i, j - 1) == '0') {
		this->set(i, j - 1, '.');
	}
}

int Grid::findExplosion() {
	//this->printGrid();

	int score = 0;
	int CB = 0;//color Bonus = nb couleurs distinctes
	int B = 0; // Nombre de boules détruites dans cette étape
	int CP = 0; // puissance de chaine
	int GB = 0; // group Bonus

	for (int i = 0; i < this->groupIdMax; ++i) {
		Block * b = this->groups[i];
		int count = b->getCount();
		if (count >= 4) {
			for (int c = 0; c < b->getCount(); ++c) {
				this->blocs[b->get(c, 0)][b->get(c, 1)] = '.';
				this->destroySkulls(b->get(c, 0), b->get(c, 1));
			}
			this->clearGroup(b->get(0, 0), b->get(0, 1));
			CB = B == 0 ? 0 : (CB == 0 ? 2 : CB * 2); //marche pas si 2 groupes de même couleur simultanément, see that LATER
			B += count;
			GB += (count - 4); //pas quand count > 11, see that LATER
		}
	}
	int factor = (CP + CB + GB);
	factor = factor>1 ? (factor < 999 ? factor : 999) : 1;
	score = (10 * B) * factor;
	
	if (score > 0) {
		this->down();
		CP = 8;
		score += this->findExplosionSub(CP);
	}
	return score;
}


int Grid::findExplosionSub(int& CP) {
	//this->printGrid();

	int score = 0;
	int CB = 0;//color Bonus = nb couleurs distinctes
	int B = 0; // Nombre de boules détruites dans cette étape
	int GB = 0; // group Bonus

	for (int i = 0; i < this->groupIdMax; ++i) {
		Block * b = this->groups[i];
		int count = b->getCount();
		if (count >= 4) {
			for (int c = 0; c < b->getCount(); ++c) {
				this->blocs[b->get(c, 0)][b->get(c, 1)] = '.';
				this->destroySkulls(b->get(c, 0), b->get(c, 1));
			}
			this->clearGroup(b->get(0, 0), b->get(0, 1));
			CB = B == 0 ? 0 : (CB == 0 ? 2 : CB * 2); //marche pas si 2 groupes de même couleur simultanément, see that LATER
			B += count;
			GB += (count - 4); //pas quand count > 11, see that LATER
		}
	}
	int factor = (CP + CB + GB);
	factor = factor>1 ? (factor < 999 ? factor : 999) : 1;
	score = (10 * B) * factor;
	
	if (score > 0) {
		this->down();
		CP *= 2;
		score += this->findExplosionSub(CP);
	}
	return score;
}

void Grid::down() {
	for (int i = 0; i < WIDTH; ++i) {
		int firstDot = -1;
		for (int j = 0; j < HEIGHT; ++j) {
			if (get(i, j) == '.') {
				//Trouve premier point en partant du bas
				if (firstDot == -1)firstDot = j;			
			}
			else if (firstDot != -1) {
				// Si on avait un point trouvé, on décale tout le monde de la hauteur au premier point
				for (int k = firstDot; k < HEIGHT - (j-firstDot); ++k) {
					set(i, k, get(i, k + (j - firstDot)));
				}
				for (int k = HEIGHT - (j - firstDot); k < HEIGHT; ++k) {
					set(i, k, '.');
				}
				//On décale de j-firstDot et reset
				j += (j - firstDot) - 1;
				firstDot = -1;
			}
		}
	}
	this->fillBlocks();
}

int Grid::update() {
	//Find Explosion
	int score = this->findExplosion();
	return score;
}

//State of the whole board
class State {
private:
	int scores[2];
	char skulls[2];
	Grid* grids[2];
public:
	State() {
		scores[0] = 0;
		scores[1] = 0;
		skulls[0] = 0;
		skulls[1] = 0;
		grids[0] = new Grid();
		grids[1] = new Grid();
	}
	State(const State& state) {
		this->scores[0] = state.getMyScore();
		this->scores[1] = state.getOpponentScore();
		this->skulls[0] = state.getMySkulls();
		this->skulls[1] = state.getOpponentSkulls();
		this->grids[0] = new Grid(*state.getMyGrid());
		this->grids[1] = new Grid(*state.getOpponentGrid());
	}
	~State() {
		delete this->grids[0];
		delete this->grids[1];
	}
	State& operator= (const State& state) {
		this->scores[0] = state.getMyScore();
		this->scores[1] = state.getOpponentScore();
		this->skulls[0] = state.getMySkulls();
		this->skulls[1] = state.getOpponentSkulls();
		*this->grids[0] = *state.getMyGrid();
		*this->grids[1] = *state.getOpponentGrid();
		return *this;
	}
	Grid* getMyGrid() const { return this->grids[0]; }
	Grid* getOpponentGrid() const { return this->grids[1]; }
	int getMyScore() const { return this->scores[0]; }
	int getOpponentScore() const { return this->scores[1]; }
	char getMySkulls() const { return this->skulls[0]; }
	char getOpponentSkulls() const { return this->skulls[1]; }
	void setMyScore(int val) { this->scores[0] = val; }
	void setOpponentScore(int val) { this->scores[1] = val; }
	void setMySkulls(char val) { this->skulls[0] = val; }
	void setOpponentSkulls(char val) { this->skulls[1] = val; }

	bool send(int playerId, const Choice& choice, char colorA, char colorB);
	int update();
};

bool State::send(int playerId, const Choice& choice, char colorA, char colorB) {
	Grid* grid = playerId == 0 ? getMyGrid() : getOpponentGrid();
	//find last empty position in col choice
	if (choice.rot % 2) {
		// --> impair ==> droit
		int last = 0;
		for (int i = 0; i < HEIGHT; ++i) {
			if (grid->get(choice.col, i) == '.')break;
			++last;
		}
		if (last >= HEIGHT - 1) return false;

		grid->set(choice.col, last, choice.rot == 1 ? colorA : colorB);
		grid->set(choice.col, last + 1, choice.rot == 3 ? colorA : colorB);
	}
	else {
		//par terre
		int colOther = choice.rot == 0 ? choice.col + 1 : choice.col - 1;
		if (colOther < 0 || colOther >= WIDTH)return false;

		int lastCol = 0, lastOther = 0;
		for (int i = 0; i < HEIGHT; ++i) {
			if (grid->get(choice.col, i) == '.')break;
			++lastCol;
		}
		if (lastCol >= HEIGHT) return false;

		for (int i = 0; i < HEIGHT; ++i) {
			if (grid->get(colOther, i) == '.')break;
			++lastOther;
		}
		if (lastOther >= HEIGHT) return false;

		grid->set(choice.col, lastCol, colorA);
		grid->set(colOther, lastOther, colorB);
	}
	grid->fillBlocks();
	return true;
}

int State::update() {
	//Update grids
	int score = this->getMyGrid()->update(); //Not Opponent for now	
	this->setMyScore(getMyScore() + score);
	//Update skulls
	//later
	return score;
}

//Game control
class Game {
private:
	char turn;
	State* previousState;
	State* currentState;
	char nextColors[8][2];
public:
	Game() :turn(0) {
		currentState = new State();
		previousState = new State();
	}
	~Game() {
		delete currentState;
		delete previousState;
	}
	char getTurn() { return this->turn; }
	State* getPreviousState() { return this->previousState; }
	State* getCurrentState() const { return this->currentState; }
	char getNextColor(int turn, int pos) const { return this->nextColors[turn][pos]; }
	void setNextColor(int turn, int pos, int val) { this->nextColors[turn][pos] = (char)(val + 48); }
	void initGame();
	void updateTurn();
	void fakeUpdateTurn();
	void printOutput(const Choice& choice);
	void saveStates();
	void updateMyScore(const Choice& choice);
	void preRun() const{
		this->currentState->getMyGrid()->resetFirst(); //in case
		this->currentState->getMyGrid()->fillBlocks();
	}
};

void Game::initGame() {
	//No init this time !
}

void Game::updateMyScore(const Choice& choice) {
	State* simulated = new State(*this->getCurrentState());
	bool possible = simulated->send(0, choice, this->getNextColor(0, 0), this->getNextColor(0, 1));
	if (possible) {
		simulated->update();
		this->getCurrentState()->setMyScore(simulated->getMyScore());
	}
	else {
		this->getCurrentState()->setMyScore(0);
	}
	delete simulated;
	cerr << "I will get " << this->getCurrentState()->getMyScore() << " with this move" << endl;
}

void Game::fakeUpdateTurn() {
	//Get Inputs
	this->setNextColor(0, 0, 1);
	this->setNextColor(0, 1, 2);

	this->setNextColor(1, 0, 3);
	this->setNextColor(1, 1, 2);

	this->setNextColor(2, 0, 4);
	this->setNextColor(2, 1, 2);

	this->setNextColor(3, 0, 1);
	this->setNextColor(3, 1, 1);

	this->setNextColor(4, 0, 3);
	this->setNextColor(4, 1, 3);

	this->setNextColor(5, 0, 4);
	this->setNextColor(5, 1, 4);

	this->setNextColor(6, 0, 1);
	this->setNextColor(6, 1, 5);

	this->setNextColor(7, 0, 4);
	this->setNextColor(7, 1, 1);
	// One line of the map ('.' = empty, '0' = skull block, '1' to '5' = colored block)
	for (int i = 0; i < HEIGHT; ++i) {
		string row = "......";
		for (int j = 0; j < WIDTH; ++j) {
			this->currentState->getMyGrid()->set(j, HEIGHT - i - 1, row[j]);
		}
	}
	string rowA = "......";
	for (int j = 0; j < WIDTH; ++j) {
		this->currentState->getMyGrid()->set(j, HEIGHT - 6 - 1, rowA[j]);
	}
	rowA = "......";
	for (int j = 0; j < WIDTH; ++j) {
		this->currentState->getMyGrid()->set(j, HEIGHT - 7 - 1, rowA[j]);
	}
	rowA = "...4..";
	for (int j = 0; j < WIDTH; ++j) {
		this->currentState->getMyGrid()->set(j, HEIGHT - 8 - 1, rowA[j]);
	}
	rowA = "451230";
	for (int j = 0; j < WIDTH; ++j) {
		this->currentState->getMyGrid()->set(j, HEIGHT - 9 - 1, rowA[j]);
	}
	rowA = "001130";
	for (int j = 0; j < WIDTH; ++j) {
		this->currentState->getMyGrid()->set(j, HEIGHT - 10 - 1, rowA[j]);
	}
	rowA = "042201";
	for (int j = 0; j < WIDTH; ++j) {
		this->currentState->getMyGrid()->set(j, HEIGHT - 11 - 1, rowA[j]);
	}
	
	for (int i = 0; i < HEIGHT; ++i) {
		string row = "......";
		for (int j = 0; j < WIDTH; ++j) {
			this->currentState->getOpponentGrid()->set(j, HEIGHT - i - 1, row[j]);
		}
	}
}

void Game::updateTurn() {
	//Get Inputs
	for (int i = 0; i < 8; ++i) {
		int colorA; // color of the first block
		int colorB; // color of the attached block
		cin >> colorA >> colorB; cin.ignore();
		this->setNextColor(i, 0, colorA);
		this->setNextColor(i, 1, colorB);
	}
	// One line of the map ('.' = empty, '0' = skull block, '1' to '5' = colored block)
	for (int i = 0; i < HEIGHT; ++i) {
		string row;
		cin >> row; cin.ignore();
		for (int j = 0; j < WIDTH; ++j) {
			this->currentState->getMyGrid()->set(j, HEIGHT - i - 1, row[j]);
		}
	}
	for (int i = 0; i < HEIGHT; ++i) {
		string row;
		cin >> row; cin.ignore();
		for (int j = 0; j < WIDTH; ++j) {
			this->currentState->getOpponentGrid()->set(j, HEIGHT - i - 1, row[j]);
		}
	}
	//Find Score
	//this->calculateOpponentScore();
	//
}

void Game::printOutput(const Choice& choice) {
	cout << choice.col << " " << choice.rot << endl;
}

void Game::saveStates() {
	*this->previousState = *this->currentState;
}

// Here represented as
class Genome {
private:
	Choice genes[GEN_SIZE];
	int score;
	bool evaluated;
public:
	Genome() : score(0), evaluated(false), genes() {}
	Genome(const Genome& gen) {
		this->score = gen.score;
		this->evaluated = gen.evaluated;
		for (int i = 0; i < GEN_SIZE; ++i) {
			this->genes[i] = gen.genes[i];
		}
	}
	Genome& operator=(const Genome& gen) {
		score = gen.score;
		evaluated = gen.evaluated;
		for (int i = 0; i < GEN_SIZE; ++i) {
			this->genes[i] = gen.genes[i];
		}
		return *this;
	}
	Choice getGene(int i) const { return this->genes[i]; }
	void setGene(int i, Choice val) { this->genes[i] = val; }
	int getScore() const { return this->score; }
	void setScore(int score) { this->score = score; }
	void eval(const Game& game);
	bool operator<(const Genome& gen) {
		return this->score < gen.getScore();
	}
	static Genome* cross(const Genome& gen1, const Genome& gen2) {
		Genome* tmp = new Genome();
		// On prend le debut du gene 1 et la fin du gene 2
		for (int i = 0; i < GEN_SIZE; ++i) {
			tmp->setGene(i, rand() % 2 ? gen1.getGene(i) : gen2.getGene(i));
		}
		return tmp;
	}
	void shift() {
		for (int i = 0; i < GEN_SIZE - 1; ++i) {
			this->setGene(i, this->genes[i + 1]);
		}
		this->setGene(GEN_SIZE - 1, { rand() % 6 , rand() % 4 });
		this->evaluated = false;
	}
	void setEvaluated(bool b) { this->evaluated = b; }
	void printGenome() {
		cerr << "Gene : ";
		for (int i = 0; i < GEN_SIZE; ++i) {
			cerr << this->genes[i].col << " , " << this->genes[i].rot << " " << endl;
		}
		cerr << "Score : " << this->score << endl;
	}
};

void Genome::eval(const Game& game) {
	int score = 0;
	//simulate state in GEN_SIZE turns
	if (!this->evaluated) {
		State* simulatedState = new State(*game.getCurrentState());
		for (int i = 0; i < GEN_SIZE; ++i) {
			bool possible = simulatedState->send(0, this->getGene(i), game.getNextColor(i, 0), game.getNextColor(i, 1));
			if (possible) {
				int scored = simulatedState->update();
				int factor = (i < 4 && scored>500) ? 20 * (4 - i) : ((i<7 && scored > 1000) ? 5 * (7 - i) : 1); // on privilegie les rapides > 700
				//int factor = 1;
				//if (scored > 2000)factor *= 10 * (GEN_SIZE - i);
				score += factor*scored;
			}
			else {
				simulatedState->setMyScore(0);
				score = 0;
				break;
			}
		}		
		this->score = score; 
		this->evaluated = true;
		delete simulatedState;
	}
}

// Abstract class for Genetic Algorithm
class GeneticBot {
protected:
	Genome* population[POP_SIZE];
public:
	GeneticBot() {
		for (int i = 0; i < POP_SIZE; ++i) {
			this->population[i] = new Genome();
		}
	};
	~GeneticBot() {
		for (int i = 0; i < POP_SIZE; ++i) {
			delete this->population[i];
		}
	}
	Genome* getGenome(int i) { return this->population[i]; }
	void setGenome(int i, const Genome& val) { *this->population[i] = val; }
	void generate();
	void eval(const Game& game);
	void select();
	void cross();
	void mutate();
	Genome* run(const Game& game);
	void shiftGenomes();
};

void GeneticBot::generate() {
	for (int i = 0; i < POP_SIZE; ++i) {
		for (int j = 0; j < GEN_SIZE; ++j) {
			this->getGenome(i)->setGene(j, { rand() % 6, rand() % 4 });
		}
	}
}

void GeneticBot::eval(const Game& game) {
	for (int i = 0; i < POP_SIZE; ++i) {
		this->getGenome(i)->eval(game);
	}
}

void fusion(Genome* tableau[], const int debut1, const int fin1, const int fin2)
{
	Genome* tableau2 = new Genome[(fin1 - debut1 + 1)];
	int debut2 = fin1 + 1;
	int compteur1 = debut1;
	int compteur2 = debut2;
	int i;

	// copie des éléments du début de tableau
	for (i = debut1; i <= fin1; i++) {
		tableau2[i - debut1] = *tableau[i];
	}

	// fusion des deux tableaux
	for (i = debut1; i <= fin2; i++)
	{
		if (compteur1 == debut2) // éléments du 1er tableau tous utilisés
			break; // éléments tous classés
		else if (compteur2 == (fin2 + 1)) // éléments du 2nd tableau tous utilisés
		{ // copie en fin de tableau des éléments du 1er sous tableau
			*tableau[i] = tableau2[compteur1 - debut1];
			compteur1++;
		}
		else if (tableau2[compteur1 - debut1]<*tableau[compteur2])
		{ // ajout d'1 élément du 1er sous tableau
			*tableau[i] = tableau2[compteur1 - debut1];
			compteur1++; // on avance ds le 1er sous tableau
		}
		else
		{ // copie de l'élément à la suite du tableau
			*tableau[i] = *tableau[compteur2];
			compteur2++; // on avance ds le 2nd sous tableau
		}
	}
	delete[] tableau2;
}

void triFusionAux(Genome* tableau[], const int debut, const int fin)
{
	if (debut != fin) // condition d'arrêt
	{
		int milieu = (debut + fin) / 2;
		triFusionAux(tableau, debut, milieu); // trie partie1
		triFusionAux(tableau, milieu + 1, fin); // trie partie2
		fusion(tableau, debut, milieu, fin); // fusion des deux parties
	}
}

void triFusion(Genome* tableau[], const int longueur)
{
	if (longueur>0)
		triFusionAux(tableau, 0, longueur - 1);
}

//simple tri et on garde les N/2 premiers implicitement on mettra les cross dans les N/2 plus mauvais (tri croissant)
void GeneticBot::select() {
	//Tri
	triFusion(this->population, POP_SIZE);
}

// Dans les N/2 premiers
void GeneticBot::cross() {
	//Nouvelle population
	Genome* croisements[POP_SIZE / 2];
	for (int i = 0; i < POP_SIZE / 2; ++i) {
		Genome* gen1 = this->getGenome(rand() % POP_SIZE);
		Genome* gen2 = this->getGenome(rand() % POP_SIZE);
		croisements[i] = Genome::cross(*gen1, *gen2); //do the new
	}
	for (int i = 0; i < POP_SIZE / 2; ++i) {
		*this->population[i] = *croisements[i];
		delete croisements[i];
	}
}

void GeneticBot::mutate() {
	//Not for now
	for (int i = 0; i < POP_SIZE-1; ++i) { //Don't modify the last one to keep strategy going
		if (rand() % 100 == 0)
		{
			this->getGenome(i)->setGene(rand() % GEN_SIZE, { rand() % 6, rand() % 4 });
			this->getGenome(i)->setEvaluated(false);
		}
	}
}
void GeneticBot::shiftGenomes() {
	for (int i = 0; i < POP_SIZE; ++i) {
		this->getGenome(i)->shift();
	}
}

Genome* GeneticBot::run(const Game& game) {
	game.preRun();
	// To keep previous values in the game
	this->shiftGenomes();
	// Simulate MAX_SIMUL times;
	for (int i = 0; i < MAX_SIMUL; ++i) {
		this->eval(game);
		this->select();
		this->cross();
		this->mutate();
		//only if too much skulls : reset all
		if (i > 5 && this->population[POP_SIZE - 1]->getScore() == 0) {
			this->generate();
		}
	}
	// Final eval & sort
	this->eval(game);
	this->select();
	cerr << " Winner is : " << endl;
	this->population[POP_SIZE - 1]->printGenome();
	return this->population[POP_SIZE - 1];
}

Choice getAction(const Game& game, GeneticBot& bot) {
	auto start_time = chrono::high_resolution_clock::now();
	Genome* winner = bot.run(game);
	auto end_time = chrono::high_resolution_clock::now();
	cerr << "Time spent in run : " << chrono::duration_cast<chrono::milliseconds>(end_time - start_time).count() << " ms ! " << endl;
	return winner->getGene(0);
}

/**
* Auto-generated code below aims at helping you parse
* the standard input according to the problem statement.
**/
int main()
{
	srand(time(NULL));
	Game game;
	GeneticBot bot;
	bot.generate();
	// game loop
	//while (1) {
		//game.updateTurn();
		game.fakeUpdateTurn();
		Choice choice = getAction(game, bot);
		game.printOutput(choice);
		game.updateMyScore(choice);
		//Save Previous State
		game.saveStates();
	//}
}