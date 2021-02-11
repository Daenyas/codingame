// SmashTheCodeV2.cpp : définit le point d'entrée pour l'application console.
//
#include "stdafx.h"

#include <string>
#include <iostream>
#include <chrono>

#define WIDTH 6
#define HEIGHT 12

#define POPULATION_SIZE 20 //Multiple de 4
#define GENOME_SIZE 8
#define GENERATION_NUMBER 900
#define OPPONENT_GENERATION_NUMBER 100
#define GROUP_MAX_SIZE 20

#pragma GCC optimize ("O3")

using namespace std;

struct Gene {
	int column;
	int rotation;
};

class Group {
private:
	int count;
	char color;
	int positions[GROUP_MAX_SIZE][2]; // Remove that and prevent too much calls on feeding TODO : refaire en liste chainee as pos does not matter
public:
	Group(int x, int y, char color) : color(color), count(0) { this->addPos(x, y); }

	int getCount() { return this->count; }
	char getColor() { return this->color; }

	void addPos(int i, int j) {
		positions[count][0] = i;
		positions[count][1] = j;
		++this->count;
	}

	bool pop(int& x, int& y) {
		if (this->count > 0) {
			--this->count;
			x = this->positions[this->count][0];
			y = this->positions[this->count][1];
			return true;
		}
		return false;
	}

	int getPos(int count, int i) { return this->positions[count][i]; }
};


class Grid {
private:
	char color[WIDTH][HEIGHT];
	bool changed[WIDTH][HEIGHT];
	bool grouped[WIDTH][HEIGHT];
public:
	Grid() {
		for (int x = 0; x < WIDTH; ++x) {
			for (int y = 0; y < HEIGHT; ++y) {
				changed[x][y] = false;
				grouped[x][y] = false;
			}
		}
	}
	char getColor(int x, int y) const { return this->color[x][y]; }
	void setColor(int x, int y, char val) { this->color[x][y] = val; }
	void changeColor(int x, int y, char val) {
		this->color[x][y] = val;
		this->changed[x][y] = (val != '.' && val != '0');
	}

	void printGrid() {
		cerr << "Grid : " << endl;
		for (int j = HEIGHT - 1; j >= 0; --j) {
			for (int i = 0; i < WIDTH; ++i) {
				cerr << this->color[i][j];
			}
			cerr << endl;
		}
	}

	int update() { return this->findExplosion(); }
	int findExplosion();
	int findExplosionSub(int& CP);
	void checkNeighbor(Group& group, int i, int j);
	void checkNeighbors(Group& group, int i, int j);
	void destroySkulls(int i, int j);
	void down();
};

int Grid::findExplosion() {
	int score = 0;
	int CP = 0; // puissance de chaine
	int CB = 0; // color Bonus = nb couleurs distinctes
	int B = 0; // nombre de boules détruites dans cette étape
	int GB = 0; // group Bonus

	for (int x = 0; x < WIDTH; ++x) {
		for (int y = 0; y < HEIGHT; ++y) {
			if (this->changed[x][y] && this->color[x][y] != '.' && this->color[x][y] != '0') { //If changed : flood fill !
				Group group(x, y, color[x][y]);
				this->changed[x][y] = false;
				this->grouped[x][y] = true;
				this->checkNeighbors(group, x, y);
				if (group.getCount() >= 4) {
					for (int c = 0; c < group.getCount(); ++c) {
						this->color[group.getPos(c, 0)][group.getPos(c, 1)] = '.';
						this->destroySkulls(group.getPos(c, 0), group.getPos(c, 1));
					}
					CB = B == 0 ? 0 : (CB == 0 ? 2 : CB * 2); //marche pas si 2 groupes de même couleur simultanément, see that LATER
					B += group.getCount();
					GB += (group.getCount() - 4); //pas quand count > 11, see that LATER
				}
			}
		}
	}
	for (int x = 0; x < WIDTH; ++x) {
		for (int y = 0; y < HEIGHT; ++y) {
			grouped[x][y] = false; //reset status
		}
	}

	int factor = (CP + CB + GB);
	factor = factor > 1 ? (factor < 999 ? factor : 999) : 1;
	score = (10 * B) * factor;

	if (score > 0) {
		this->down();
		CP = 8;
		score += this->findExplosionSub(CP);
	}
	return score;
}

int Grid::findExplosionSub(int& CP) {
	int score = 0;
	int CB = 0; // color Bonus = nb couleurs distinctes
	int B = 0; // nombre de boules détruites dans cette étape
	int GB = 0; // group Bonus

	for (int x = 0; x < WIDTH; ++x) {
		for (int y = 0; y < HEIGHT; ++y) {
			if (this->changed[x][y] && this->color[x][y] != '.' && this->color[x][y] != '0') { //If changed : flood fill !
				Group group(x, y, color[x][y]);
				this->changed[x][y] = false;
				this->checkNeighbors(group, x, y);
				if (group.getCount() >= 4) {
					for (int c = 0; c < group.getCount(); ++c) {
						this->color[group.getPos(c, 0)][group.getPos(c, 1)] = '.';
						this->destroySkulls(group.getPos(c, 0), group.getPos(c, 1));
					}
					CB = B == 0 ? 0 : (CB == 0 ? 2 : CB * 2); //marche pas si 2 groupes de même couleur simultanément, see that LATER
					B += group.getCount();
					GB += (group.getCount() - 4); //pas quand count > 11, see that LATER
				}
			}
		}
	}
	for (int x = 0; x < WIDTH; ++x) {
		for (int y = 0; y < HEIGHT; ++y) {
			grouped[x][y] = false; //reset status
		}
	}

	int factor = (CP + CB + GB);
	factor = factor > 1 ? (factor < 999 ? factor : 999) : 1;
	score = (10 * B) * factor;

	if (score > 0) {
		this->down();
		CP *= 2;
		score += this->findExplosionSub(CP);
	}
	return score;
}

void Grid::checkNeighbor(Group& group, int i, int j) {
	char color = group.getColor();
	if (this->color[i][j] == color && !this->grouped[i][j]) {
		group.addPos(i, j);
		this->changed[i][j] = false; // even if has already been treated
		this->grouped[i][j] = true; // even if has already been treated
		checkNeighbors(group, i, j);
	}
}

void Grid::checkNeighbors(Group& group, int i, int j) {
	//middle position i, j
	if (i < WIDTH - 1)checkNeighbor(group, i + 1, j);
	if (i > 0)checkNeighbor(group, i - 1, j);
	if (j < HEIGHT - 1)checkNeighbor(group, i, j + 1);
	if (j > 0)checkNeighbor(group, i, j - 1);
}

void Grid::destroySkulls(int i, int j) {
	//middle position i, j
	if (i < WIDTH - 1 && this->color[i + 1][j] == '0') {
		this->color[i + 1][j] = '.';
	}
	if (i > 0 && this->color[i - 1][j] == '0') {
		this->color[i - 1][j] = '.';
	}
	if (j < HEIGHT - 1 && this->color[i][j + 1] == '0') {
		this->color[i][j + 1] = '.';
	}
	if (j > 0 && this->color[i][j - 1] == '0') {
		this->color[i][j - 1] = '.';
	}
}

void Grid::down() {
	for (int i = 0; i < WIDTH; ++i) {
		int firstDot = -1;
		for (int j = 0; j < HEIGHT; ++j) {
			if (getColor(i, j) == '.') {
				//Trouve premier point en partant du bas
				if (firstDot == -1)firstDot = j;
			}
			else if (firstDot != -1) {
				// Si on avait un point trouvé, on décale tout le monde de la hauteur au premier point
				for (int k = firstDot; k < HEIGHT - (j - firstDot); ++k) {
					changeColor(i, k, getColor(i, k + (j - firstDot)));
				}
				for (int k = HEIGHT - (j - firstDot); k < HEIGHT; ++k) {
					this->color[i][k] = '.';
				}
				//On décale de j-firstDot et reset
				j += (j - firstDot) - 1;
				firstDot = -1;
			}
		}
	}
}

struct CachedSimul {
	Grid grid;
	int score;
};

class Cache {
private:
	CachedSimul* cache0[30]; // 0 --> 4*6 + 5                        (24 + 5)
	CachedSimul* cache1[1044]; // 0 --> 4*216 + 4*36 + 5*6 + 5       (864 + 144 + 30 + 5)              1043
public:
	Cache() {
		for (int i = 0; i < 30; ++i) {
			this->cache0[i] = nullptr;
		}
		for (int i = 0; i < 1044; ++i) {
			this->cache1[i] = nullptr;
		}
	}
	static Cache& instance()
	{
		static Cache s;
		return s;
	}
	CachedSimul* getFromCache0(const Gene& gene0) {
		return this->cache0[gene0.rotation * 6 + gene0.column];
	}
	CachedSimul* getFromCache1(const Gene& gene0, const Gene& gene1) {
		return this->cache1[gene0.rotation * 216 + gene1.rotation * 36 + gene0.column * 6 + gene1.column];
	}
	void setInCache0(const Gene& gene0, CachedSimul* value) {
		this->cache0[gene0.rotation * 6 + gene0.column] = value;
	}
	void setInCache1(const Gene& gene0, const Gene& gene1, CachedSimul* value) {
		this->cache1[gene0.rotation * 216 + gene1.rotation * 36 + gene0.column * 6 + gene1.column] = value;
	}
	void clearCache() {
		for (int i = 0; i < 30; ++i) {
			this->cache0[i] = nullptr;
		}
		for (int i = 0; i < 1044; ++i) {
			this->cache1[i] = nullptr;
		}
	}
};

class Board {
private:
	Grid grid[2];
	char nextColors[GENOME_SIZE][2];	
public:
	Grid* getGrid(bool isOpponent) { return isOpponent?&this->grid[1]:&this->grid[0]; }
	char getNextColor(int turn, int i) const { return this->nextColors[turn][i]; }
	void updateInputs();
	void fakeUpdateInputs();
	bool send(bool isOpponent, const Gene& gene, char colorA, char colorB);
	void sendSkulls(bool isOpponent, int lignes);
	int countColor(bool isOpponent, char c) const {
		const Grid* grid = isOpponent ? &this->grid[1] : &this->grid[0];
		int count = 0;
		for (int x = 0; x < WIDTH; ++x) {
			for (int y = 0; y < WIDTH; ++y) {
				if (grid->getColor(x, y) == c)++count;
			}
		}
		return count;
	}
};

bool Board::send(bool isOpponent, const Gene& gene, char colorA, char colorB) {
	Grid* grid = isOpponent ? &this->grid[1] : &this->grid[0];
	//find last empty position in col gene
	if (gene.rotation % 2) {
		// --> impair ==> droit
		int last = 0;
		for (int i = 0; i < HEIGHT; ++i) {
			if (grid->getColor(gene.column, i) == '.')break;
			++last;
		}
		if (last >= HEIGHT - 1) return false;

		grid->changeColor(gene.column, last, gene.rotation == 1 ? colorA : colorB);
		grid->changeColor(gene.column, last + 1, gene.rotation == 3 ? colorA : colorB);
	}
	else {
		//par terre
		int colOther = gene.rotation == 0 ? gene.column + 1 : gene.column - 1;
		if (colOther < 0 || colOther >= WIDTH)return false;

		int lastCol = 0, lastOther = 0;
		for (int i = 0; i < HEIGHT; ++i) {
			if (grid->getColor(gene.column, i) == '.')break;
			++lastCol;
		}
		if (lastCol >= HEIGHT) return false;

		for (int i = 0; i < HEIGHT; ++i) {
			if (grid->getColor(colOther, i) == '.')break;
			++lastOther;
		}
		if (lastOther >= HEIGHT) return false;

		grid->changeColor(gene.column, lastCol, colorA);
		grid->changeColor(colOther, lastOther, colorB);
	}
	return true;
}

void Board::sendSkulls(bool isOpponent, int lignes){
	Grid* grid = isOpponent ? &this->grid[1] : &this->grid[0];
	for (int i = HEIGHT - 1; i > HEIGHT - 1 - lignes; --i) {
		for (int x = 0; x < WIDTH; ++x) {
			if(grid->getColor(x,i) == '.')grid->setColor(x, i, '0');
		}		
	}
	grid->down();
}

void Board::updateInputs() {
	//Get Inputs
	for (int i = 0; i < 8; ++i) {
		int colorA; // color of the first block
		int colorB; // color of the attached block
		cin >> colorA >> colorB; cin.ignore();
		this->nextColors[i][0] = (char)(colorA + 48);
		this->nextColors[i][1] = (char)(colorB + 48);
	}
	// One line of the map ('.' = empty, '0' = skull block, '1' to '5' = colored block)
	for (int i = 0; i < HEIGHT; ++i) {
		string row;
		cin >> row; cin.ignore();
		for (int j = 0; j < WIDTH; ++j) {
			this->grid[0].setColor(j, HEIGHT - i - 1, row[j]);
		}
	}
	for (int i = 0; i < HEIGHT; ++i) {
		string row;
		cin >> row; cin.ignore();
		for (int j = 0; j < WIDTH; ++j) {
			this->grid[1].setColor(j, HEIGHT - i - 1, row[j]);
		}
	}
}

void Board::fakeUpdateInputs() {
	enum couleur { ROUGE, BLEU, ROSE, VERT, JAUNE };
	this->nextColors[0][0] = (char)(ROUGE + 49);
	this->nextColors[0][1] = (char)(BLEU + 49);

	this->nextColors[1][0] = (char)(ROSE + 49);
	this->nextColors[1][1] = (char)(ROUGE + 49);

	this->nextColors[2][0] = (char)(BLEU + 49);
	this->nextColors[2][1] = (char)(ROUGE + 49);

	this->nextColors[3][0] = (char)(ROUGE + 49);
	this->nextColors[3][1] = (char)(ROSE + 49);

	this->nextColors[4][0] = (char)(VERT + 49);
	this->nextColors[4][1] = (char)(VERT + 49);

	this->nextColors[5][0] = (char)(ROSE + 49);
	this->nextColors[5][1] = (char)(VERT + 49);

	this->nextColors[6][0] = (char)(ROSE + 49);
	this->nextColors[6][1] = (char)(JAUNE + 49);

	this->nextColors[7][0] = (char)(VERT + 49);
	this->nextColors[7][1] = (char)(VERT + 49);

	// One line of the map ('.' = empty, '0' = skull block, '1' to '5' = colored block)
	string row[HEIGHT];
	row[0] = "......";
	row[1] = "......";
	row[2] = "......";
	row[3] = ".2.0..";
	row[4] = ".3.3..";
	row[5] = ".2.442";
	row[6] = ".23400";
	row[7] = ".35551";
	row[8] = "021244";
	row[9] = "523113";
	row[10] = "412121";
	row[11] = "253531";

	for (int i = 0; i < HEIGHT; ++i) {
		for (int j = 0; j < WIDTH; ++j) {
			this->grid[0].setColor(j, HEIGHT - i - 1, row[i][j]);
		}
	}
	string row2[HEIGHT];
	row2[0] = "......";
	row2[1] = "......";
	row2[2] = "......";
	row2[3] = "......";
	row2[4] = "......";
	row2[5] = "......";
	row2[6] = "...5..";
	row2[7] = "...5..";
	row2[8] = ".2345.";
	row2[9] = ".2345.";
	row2[10] = ".12345";
	row2[11] = ".12345";
	for (int i = 0; i < HEIGHT; ++i) {		
		for (int j = 0; j < WIDTH; ++j) {
			this->grid[1].setColor(j, HEIGHT - i - 1, row2[i][j]);
		}
	}
}

class Genome {
private:
	Gene genes[GENOME_SIZE];
	int score;
	bool evaluated;
	int lignes[GENOME_SIZE];
public:
	Genome() : score(0), evaluated(false) {
		for (int i = 0; i < GENOME_SIZE; ++i) {
			lignes[i] = 0;
		}
	}
	Gene getGene(int i) const { return this->genes[i]; }
	void setGene(int i, const Gene& val) { this->genes[i] = val; }
	int getScore() const { return this->score; }
	int getEvaluated() const { return this->evaluated; }
	int getLignes(int i) const { return this->lignes[i]; }

	void evaluate(const Board& board, bool isOpponent, const Genome& against);
	int evaluateTurn(Board& board, int turn, bool isOpponent, const Genome& against);
	bool operator<(const Genome& gen) { return this->score < gen.score; }
	static Genome cross(const Genome& gen1, const Genome& gen2) {
		Genome tmp;
		// On prend le debut du gene 1 et la fin du gene 2
		for (int i = 0; i < GENOME_SIZE; ++i) {
			tmp.genes[i] = rand() % 2 ? gen1.genes[i] : gen2.genes[i];
		}
		return tmp;
	}
	void mutate() {
		this->setGene(rand() % GENOME_SIZE, { rand() % 6, rand() % 4 });
	}
	void shift() {
		for (int i = 0; i < GENOME_SIZE - 1; ++i) {
			this->setGene(i, this->genes[i + 1]);
		}
		this->setGene(GENOME_SIZE - 1, { rand() % 6 , rand() % 4 });
		this->evaluated = false;
	}
};

void Genome::evaluate(const Board& board, bool isOpponent, const Genome& against) {
	Board simulated(board);
	int score = 0;
	for (int i = 0; i < GENOME_SIZE; ++i) {
		int scoreOnTurn = 0;
		/*if (!isOpponent && i == 0) { //only cache ours
			CachedSimul* cb = Cache::instance().getFromCache0(this->genes[0]);
			if (cb == nullptr) {
				scoreOnTurn = this->evaluateTurn(simulated, i, isOpponent, against);
				cb = new CachedSimul({ *simulated.getGrid(isOpponent),scoreOnTurn }); //never destroy?
				Cache::instance().setInCache0(this->genes[0],cb);
			}
			else{//apply on simulated from cache
				*simulated.getGrid(isOpponent) = cb->grid;
				scoreOnTurn = cb->score;
			}
		}
		else if (!isOpponent && i == 1) { //only cache ours
			CachedSimul* cb = Cache::instance().getFromCache1(this->genes[0], this->genes[1]);
			if (cb == nullptr) {
				scoreOnTurn = this->evaluateTurn(simulated, i, isOpponent, against);
				cb = new CachedSimul({*simulated.getGrid(isOpponent),scoreOnTurn }); //never destroy?
				Cache::instance().setInCache1(this->genes[0], this->genes[1], cb);
			}
			else {//apply on simulated from cache
				*simulated.getGrid(isOpponent) = cb->grid;
				scoreOnTurn = cb->score;
			}
		}
		else{ //no cache*/
			scoreOnTurn = this->evaluateTurn(simulated, i, isOpponent, against);
		/*}*/

		// Genome Scoring
		if (scoreOnTurn == -1) {
			score = i - GENOME_SIZE;
			break;
		}
		int nuisance = scoreOnTurn / 70;
		int lignes = nuisance / 6;
		this->lignes[i] = lignes;
		int factorAdd = 0;
		int factorMult = 1;

		if (isOpponent) {
			if (lignes > 1) {
				factorAdd = i > 6 ? 0 : (GENOME_SIZE - i) * 100;
			}// lui 2 lignes 6 prochain tour au plus rapide	
			else {
				if (score>0) {
					break;
				}
			}
		}
		else {
			//4 lignes dans les 4 prochains tour
			if (lignes > 4) {
				factorAdd = i > 2 ? (GENOME_SIZE - i) * 10 : (GENOME_SIZE - i) * 100;
			}
			if (board.countColor(true, '.') < 20) {
				if (i == 0)factorMult = 1000;
			}
			if (board.countColor(true, '0') > 15) {
				if (lignes < 4 && lignes>1) {
					factorAdd = (GENOME_SIZE - i) * 1000;
				}				
			}
		}
		score += factorMult*lignes + factorAdd;
	}
	this->evaluated = true;
	this->score = score;
}

int Genome::evaluateTurn(Board& simulated, int turn, bool isOpponent, const Genome& against) {
	if (!isOpponent && against.lignes[turn]<4)simulated.sendSkulls(false, against.lignes[turn]);
	bool possible = simulated.send(isOpponent, this->genes[turn], simulated.getNextColor(turn, 0), simulated.getNextColor(turn, 1));
	if (possible) {
		return simulated.getGrid(isOpponent)->update();
	}
	return -1;
}

class Genetic {
private:
	Genome population[POPULATION_SIZE];
	bool isOpponent;
	void generate() {
		for (int i = 0; i < POPULATION_SIZE; ++i) {
			for (int j = 0; j < GENOME_SIZE; ++j) {
				this->getGenome(i)->setGene(j, { rand() % 6, rand() % 4 });
			}
		}
	}
	void shift() {
		for (int i = 0; i < POPULATION_SIZE; ++i) {
			this->getGenome(i)->shift();
		}
	}
	void evaluate(const Board& board, const Genome& against) {
		for (int i = 0; i < POPULATION_SIZE; ++i) {
			if (!this->population[i].getEvaluated())this->population[i].evaluate(board, this->isOpponent, against);
		}
	}
	void select();
	void cross(int turn);
public:
	Genetic(bool isOpponent) : isOpponent(isOpponent) {
		this->generate();
	}
	Genome* getGenome(int i) { return &this->population[i]; }
	Genome run(const Board& board, const Genome& against);
	void aim(const Board& board, const Genome& gen);

};


void fusion(Genome tableau[], const int debut1, const int fin1, const int fin2)
{
	Genome* tableau2 = new Genome[(fin1 - debut1 + 1)];
	int debut2 = fin1 + 1;
	int compteur1 = debut1;
	int compteur2 = debut2;
	int i;

	// copie des éléments du début de tableau
	for (i = debut1; i <= fin1; i++) {
		tableau2[i - debut1] = tableau[i];
	}

	// fusion des deux tableaux
	for (i = debut1; i <= fin2; i++)
	{
		if (compteur1 == debut2) // éléments du 1er tableau tous utilisés
			break; // éléments tous classés
		else if (compteur2 == (fin2 + 1)) // éléments du 2nd tableau tous utilisés
		{ // copie en fin de tableau des éléments du 1er sous tableau
			tableau[i] = tableau2[compteur1 - debut1];
			compteur1++;
		}
		else if (tableau2[compteur1 - debut1] < tableau[compteur2])
		{ // ajout d'1 élément du 1er sous tableau
			tableau[i] = tableau2[compteur1 - debut1];
			compteur1++; // on avance ds le 1er sous tableau
		}
		else
		{ // copie de l'élément à la suite du tableau
			tableau[i] = tableau[compteur2];
			compteur2++; // on avance ds le 2nd sous tableau
		}
	}
	delete[] tableau2;
}

void triFusionAux(Genome tableau[], const int debut, const int fin)
{
	if (debut != fin) // condition d'arrêt
	{
		int milieu = (debut + fin) / 2;
		triFusionAux(tableau, debut, milieu); // trie partie1
		triFusionAux(tableau, milieu + 1, fin); // trie partie2
		fusion(tableau, debut, milieu, fin); // fusion des deux parties
	}
}

void triFusion(Genome tableau[], const int longueur)
{
	if (longueur > 0)
		triFusionAux(tableau, 0, longueur - 1);
}

//simple tri et on garde les N/2 premiers implicitement on mettra les cross dans les N/2 plus mauvais (tri croissant)
void Genetic::select() {
	//Tri
	triFusion(this->population, POPULATION_SIZE);
}

// Dans les N/2 premiers
void Genetic::cross(int turn) {
	//Nouvelle population
	int turns = this->isOpponent ? OPPONENT_GENERATION_NUMBER : GENERATION_NUMBER;
	double mutationChance = 0.75 - ((turn / (turns - 1.))*0.60); // Goes from 0.75 to 0.15
	Genome croisements[POPULATION_SIZE / 2];
	for (int i = 0; i < POPULATION_SIZE / 2; ++i) {
		Genome* gen1 = this->getGenome(POPULATION_SIZE / 2 + (rand() % POPULATION_SIZE / 2));
		Genome* gen2 = this->getGenome(POPULATION_SIZE / 2 + (rand() % POPULATION_SIZE / 2));
		croisements[i] = Genome::cross(*gen1, *gen2); //do the new
		if (rand() % 100 <= (int)(mutationChance * 100))croisements[i].mutate();
	}
	for (int i = 0; i < POPULATION_SIZE / 2; ++i) {
		this->population[i] = croisements[i];
	}
}

Genome Genetic::run(const Board& board, const Genome& against) {
	//this->generate(); //BEWARE if reuse : dont forget evaluated false !
	this->shift(); // to keep previous strategies !
	this->evaluate(board,against);
	this->select();
	// Generate GENERATION_NUMBER generations
	int turns = this->isOpponent ? OPPONENT_GENERATION_NUMBER : GENERATION_NUMBER;
	for (int i = 0; i < turns; ++i) {
		this->cross(i);
		this->evaluate(board, against);
		this->select();
	}
	// Return the best
	return this->population[POPULATION_SIZE - 1];
}

void Genetic::aim(const Board& board, const Genome& gen) {
	Board simulated(board);
	int score = 0;
	bool possible = simulated.send(false, gen.getGene(0), board.getNextColor(0, 0), board.getNextColor(0, 1));
	if (possible) {
		int scoreOnTurn = simulated.getGrid(false)->update();
		score += scoreOnTurn;
	}
	else {
		score = -1;
	}
	cerr << "I will win " << score << " points with this move ! " << endl;
	cerr << "Genetic scoring : "<< gen.getScore() << endl;
}

int main()
{
	srand(time(NULL));

	Board board;
	Genetic genetic(false);
	Genetic opponent(true);
	while (1) {
		// Wait for inputs
		// board.updateInputs();

		// Fake update -- DEBUG ONLY
		board.fakeUpdateInputs();

		// Run GA
		auto start_time = chrono::high_resolution_clock::now();
		Genome against = opponent.run(board, Genome());
		auto end_time = chrono::high_resolution_clock::now();
		cerr << "Time spent in opponent run : " << chrono::duration_cast<chrono::milliseconds>(end_time - start_time).count() << " ms ! " << endl;

		auto start_time2 = chrono::high_resolution_clock::now();
		Genome winner = genetic.run(board, against);
		auto end_time2 = chrono::high_resolution_clock::now();
		cerr << "Time spent in my run : " << chrono::duration_cast<chrono::milliseconds>(end_time2 - start_time2).count() << " ms ! " << endl;
		
		// Aiming -- DEBUG ONLY
		genetic.aim(board, winner);

		// Print Output
		cout << winner.getGene(0).column << " " << winner.getGene(0).rotation << endl;
		Cache::instance().clearCache();
	}
	return 0;
}