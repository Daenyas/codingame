#include "stdafx.h"
#include <math.h>
#include <iostream>
#include <string>
#include <time.h>
#include <limits>
#include <chrono>

#define GEN_TURNS 6
#define POP_SIZE 150
#define SIMUL_NB 300

#define MAX_GHOST 28
#define MAX_BUSTER 5
#define MAX_TURN 200

#define VISION_RANGE 2200
#define BUST_MIN 900
#define BUST_MAX 1760
#define STUN_MAX 1760
#define STUN_DURATION 10
#define RELOAD_DURATION 20
#define BASE_RADIUS 1600
#define BUSTER_MOVE 800
#define GHOST_MOVE 400

#pragma GCC optimize ("O3")

using namespace std;

enum Order
{
	Move,
	Bust,
	Release,
	Stun
};

class Gene
{
public:
	int order;
	int x;
	int y;
	Gene() : order(Move), x(0), y(0)
	{
	}
	void generate()
	{
		this->order = rand() % 4;
		if (this->order == Move)
		{
			this->x = rand() % 16000;
			this->y = rand() % 9000;
		}
	}
};

class Coordinate {
public:
	int x;
	int y;
	Coordinate(int x, int y) : x(x), y(y) {}
	double distanceTo(const Coordinate& coord)
	{
		return sqrt((this->x - coord.x) * (this->x - coord.x) + (this->y - coord.y) * (this->y - coord.y));
	}
	string toString()
	{
		return this->x + " " + this->y;
	}
};

class Entity
{
public:
	int id;
	Coordinate position;
	int teamId;
	int state;
	int value;
	int lastUpdateTurn;
	/*Entity(int properties[], const Game& game) : id(properties[0]), position(Coordinate(properties[1], properties[2])), teamId(properties[3]), state(properties[4]), value(properties[5]), lastUpdateTurn(game.turn)
	{
	}*/
	Entity() : id(-1) , position(Coordinate(0,0)), lastUpdateTurn(-1) {}
	virtual void update(int properties[], int turn)
	{
		this->id = properties[0];
		this->position = Coordinate(properties[1], properties[2]);
		this->teamId = properties[3];
		this->state = properties[4];
		this->value = properties[5];
		this->lastUpdateTurn = turn;
	}
};

class Ghost : public Entity
{
public:
	int myBustersCount;
	/*Ghost(int properties[], const Game& game) : Entity(properties, game), myBustersCount(0)
	{
	}*/
	Ghost() : Entity() , myBustersCount(0) {}
	virtual void update(int properties[], int turn)
	{
		this->Entity::update(properties, turn);
	}
};

class Buster : public Entity
{
public:
	int weapon;
	int targetBust; // can do the same for target stun
	/*Buster(int properties[], const Game& game) : Entity(properties, game), weapon(0)
	{
	}*/
	Gene decision;
	Buster() : Entity(), weapon(0), targetBust(-1) {}
	virtual void update(int properties[], int turn)
	{
		this->Entity::update(properties, turn);
		if (this->weapon > 0)
		{
			--this->weapon;
		}
	}	
};

class Game
{
public:
	int myTeamId;
	int bustersPerPayer;
	int ghostCount;
	int turn;
	Ghost ghosts[MAX_GHOST];
	Buster myBusters[MAX_BUSTER];
	Buster ennemyBusters[MAX_BUSTER];
	
	Coordinate myBase;
	Coordinate ennemyBase;

	Game(int myTeamId, int bustersPerPayer, int ghostCount, int turn = 0) : myTeamId(myTeamId), bustersPerPayer(bustersPerPayer), ghostCount(ghostCount), turn(turn),
		myBase(myTeamId == 0 ? Coordinate(0,0) : Coordinate(16000,9000)), ennemyBase(myTeamId == 0 ? Coordinate(16000, 9000) : Coordinate(0, 0))
	{
	}

	void addEntity(int inputs[])
	{
		int type = inputs[3];
		int id = inputs[0];
		if (type == -1)
		{
			this->ghosts[id].update(inputs, this->turn);			
		}
		else
		{
			int myArrayId = type == 1 ? id - this->bustersPerPayer : id;
			if (this->myTeamId == type)
			{
				this->myBusters[myArrayId].update(inputs, this->turn);
			}
			else
			{
				this->ennemyBusters[myArrayId].update(inputs, this->turn);
			}
		}
	}

	// Simulation part

	void simulateMyStuns(int busterId, const Gene& action, int& score)
	{
		Buster& buster = this->myBusters[busterId]; // should never throw
		if (action.order != Stun)
		{
			// Just compute Stun here
			return;
		}

		if (buster.weapon > 0)
		{
			// If no : invalid, bad score
			score = -1000000;
		}
		else
		{
			// Find nearest ennemy
			int ennemyId = -1;
			double minDistance = numeric_limits<int>::max();
			for (int i = 0; i < this->bustersPerPayer; ++i)
			{
				double distance = this->ennemyBusters[i].position.distanceTo(buster.position);
				// Consider it only if it has been updated and bustable
				if (this->ennemyBusters[i].lastUpdateTurn == this->turn &&
					distance < STUN_MAX &&
					distance < minDistance)
				{
					ennemyId = i;
					minDistance = distance;
				}
			}

			if (ennemyId == -1)
			{
				// If no : invalid, bad score
				score = -1000000;
			}
			else
			{
				// stun him 
				Buster& ennemy = this->ennemyBusters[ennemyId];
				if (ennemy.state == 1)
				{
					//free ghost
					score += 100;
					Ghost& ghost = this->ghosts[ennemy.value];
					ghost.lastUpdateTurn = this->turn + 1;
					ghost.position = ennemy.position;
				}
				ennemy.state = 2;
				ennemy.value = STUN_DURATION;
				ennemy.lastUpdateTurn++;
			}
			// free ghost -- not that good
			if (buster.state == 1)
			{
				Ghost& ghost = this->ghosts[buster.value];
				ghost.lastUpdateTurn = this->turn + 1;
				ghost.position = buster.position;
				score -= 100;
				buster.state = 0;
				buster.weapon = RELOAD_DURATION;
			}
		}
		buster.lastUpdateTurn++;
	}

	//TODO : update ghost : beware only ones visible
	void simulateAction(int busterId, const Gene& action, int& score)
	{		
		Buster& buster = this->myBusters[busterId]; // should never throw

		// Reset targets
		buster.targetBust = -1;

		// First check if i am stunned, decrement value in that case
		if (buster.state == 2)
		{
			if (buster.value == 0)
			{
				buster.state = 0;
			}
			buster.value--; // to have value = -1
		}
		else
		{
			// Else i do what i have to do
			// If carrying a ghost, should go home
			if (buster.state == 2)
			{
				if (action.order != Move)
				{
					score = -1000000;
				}						
			}
			
			if (action.order == Move)
			{
				Coordinate finalPoint(action.x, action.y);
				double distance = buster.position.distanceTo(finalPoint);
				
				if (distance > BUSTER_MOVE)
				{
					// In that case, cut move at 800 of distance - Thales saves me
					finalPoint.x = round((BUSTER_MOVE*finalPoint.x + (distance - BUSTER_MOVE)*buster.position.x) / distance);
					finalPoint.y = round((BUSTER_MOVE*finalPoint.y + (distance - BUSTER_MOVE)*buster.position.y) / distance);
				}

				if (buster.state == 2)
				{
					double baseDistance = buster.position.distanceTo(myBase);
					double finalBaseDistance = finalPoint.distanceTo(myBase);
					if (finalBaseDistance < baseDistance - 600)
					{
						score += 5000;
					}
					else
					{
						score -= 5000;
					}
				}

				buster.position.x = finalPoint.x;
				buster.position.y = finalPoint.y;
				// TODO : update a map of explored positions - score will depends on that

				

			}
			else if (action.order == Bust)
			{
				// Find nearest ghost
				int ghostId = -1;
				double minDistance = numeric_limits<int>::max();
				for (int i = 0; i < this->ghostCount; ++i)
				{
					double distance = this->ghosts[i].position.distanceTo(buster.position);
					// Consider it only if it has been updated and bustable
					if (this->ghosts[i].lastUpdateTurn == this->turn &&
						distance < BUST_MAX && distance > BUST_MIN &&
						distance < minDistance)
					{
						ghostId = i;
						minDistance = distance;
					}
				}

				if (ghostId == -1)
				{
					// If no : invalid, bad score
					score = -1000000;
				}
				else
				{
					Ghost& ghost = this->ghosts[ghostId];
					// aim it !
					buster.targetBust = ghostId;
					buster.state = 3;
					//increment counts
					ghost.myBustersCount++;
					ghost.value++;
				}
			}
			else if (action.order == Release)
			{
				if (buster.state != 1)
				{
					// If no : invalid, bad score
					score = -1000000;
				}
				else
				{
					//If in base : increment points : good score
					if (buster.position.distanceTo(this->myBase) < BASE_RADIUS)
					{
						score += 10000;
					}
					else
					{
						//If no, liberate : not good
						score -= 1000;
					}
					buster.state = 0;
					buster.value = -1;
				}
				// TODO : Far more better if fast
			}
			/*
			case Stun:
				// Already computed
				break;

			default:
				// Should never happen
				break;*/			
		}
		// all case : i have been updated
		if (buster.weapon > 0)
		{
			buster.weapon--;
		}
		buster.lastUpdateTurn++;
	}

	void simulateGhost(int j, int& score)
	{
		Ghost& ghost = this->ghosts[j];
		if (ghost.lastUpdateTurn != this->turn)
		{
			//skip, nothing to do with him
			return;
		}
		
		// First decrement
		if (ghost.state > 0)
		{
			ghost.state -= ghost.value;
			if (ghost.state < 0)
			{
				ghost.state = 0;
			}
		}

		if (ghost.state == 0)
		{
			// no stamina, i can get taken			
			if (ghost.myBustersCount > ghost.value / 2)
			{
				// My nearest get it !
				score += 1000;

				int busterId = -1;
				double minDistance = numeric_limits<int>::max();
				for (int i = 0; i < this->bustersPerPayer; ++i)
				{					
					double distance = this->myBusters[i].position.distanceTo(ghost.position);
					if (distance < minDistance && this->myBusters[i].state == 3 && this->myBusters[i].targetBust == ghost.id)
					{
						minDistance = distance;
						busterId = i;
					}					
				}
				//update them
				Buster& buster = this->myBusters[busterId];
				buster.state = 1;
				buster.value = ghost.id;
				ghost.lastUpdateTurn = -1;
				for (int i = 0; i < this->bustersPerPayer; ++i)
				{
					if ( i != busterId && this->myBusters[i].state == 3 && this->myBusters[i].targetBust == ghost.id)
					{
						this->myBusters[i].state = 0;
					}
				}
				// TODO : update ennemy
			}
			else if (ghost.myBustersCount == ghost.value / 2)
			{
				// Nothing happen
				// Maybe can be fine if have another buster around to help us
			}
			else
			{
				// Ennemy got it
				score = -1000;

				//update them
		/*	SHOULD BE ENNEMY	Buster& buster = this->myBusters[busterId];
				buster.state = 1;
				buster.value = ghost.id;*/
				ghost.lastUpdateTurn = -1;
				for (int i = 0; i < this->bustersPerPayer; ++i)
				{
					if (this->myBusters[i].state == 3 && this->myBusters[i].targetBust == ghost.id)
					{
						this->myBusters[i].state = 0;
					}
				}
			}
			// Better if fast
		}
	}

	void printDecision(int i)
	{
		Buster& buster = this->myBusters[i];

		string result = "";
		if (buster.decision.order == Move)
		{
			result = "MOVE " + to_string(buster.decision.x) + " " + to_string(buster.decision.y);
		}
		else if (buster.decision.order == Bust)
		{
			// Find nearest ghost
			int ghostId = -1;
			double minDistance = numeric_limits<int>::max();
			for (int i = 0; i < this->ghostCount; ++i)
			{
				double distance = this->ghosts[i].position.distanceTo(buster.position);
				// Consider it only if it has been updated and bustable
				if (this->ghosts[i].lastUpdateTurn == this->turn &&
					distance < BUST_MAX && distance > BUST_MIN &&
					distance < minDistance)
				{
					ghostId = i;
					minDistance = distance;
				}
			}
			if (ghostId != -1)
			{
				result = "BUST " + to_string(this->ghosts[ghostId].id);
				// TODO : use this to fill count on ghosts on update
				buster.targetBust = ghostId;
			}
			else
			{
				cerr << "Not normal, ghost disapeared :(" << endl;
				result = "MOVE " + to_string(buster.position.x) + " " + to_string(buster.position.y);
			}
		}
		else if (buster.decision.order == Release)
		{
			result = "RELEASE";
		}
		else if (buster.decision.order == Stun)
		{
			// Find nearest ennemy
			int ennemyId = -1;
			double minDistance = numeric_limits<int>::max();
			for (int i = 0; i < this->bustersPerPayer; ++i)
			{
				double distance = this->ennemyBusters[i].position.distanceTo(buster.position);
				// Consider it only if it has been updated and bustable
				if (this->ennemyBusters[i].lastUpdateTurn == this->turn &&
					distance < STUN_MAX &&
					distance < minDistance)
				{
					ennemyId = i;
					minDistance = distance;
				}
			}

			if (ennemyId != -1)
			{
				result = "STUN " + to_string(this->ennemyBusters[ennemyId].id);
			}
			else
			{
				cerr << "Not normal, ennemy disapeared :(" << endl;
				result = "MOVE " + to_string(buster.position.x) + " " + to_string(buster.position.y);
			}
		}

		/*
		default:
			//should not be possible
			break;
		*/
		cout << result << endl; // << result << endl; to see what i am doing on screen ??
	}
};

class Genome
{
public:
	Gene genes[GEN_TURNS];
	void generate()
	{
		for (int i = 0; i < GEN_TURNS; ++i)
		{
			this->genes[i].generate();
		}
	}
	void shift()
	{
		for (int i = 0; i < GEN_TURNS - 1; ++i) {
			this->genes[i] = this->genes[i + 1];
		}
		this->genes[GEN_TURNS - 1].generate();
	}
	// random cross
	void cross(const Genome& g1, const Genome& g2)
	{
		for (int i = 0; i < GEN_TURNS; ++i)
		{
			this->genes[i] = (rand() % 2) ? g1.genes[i] : g2.genes[i];
		}
	}
	void mutate()
	{
		for (int i = 0; i < GEN_TURNS; ++i)
		{
			if (rand() % 100 < 70) // Mutation chance : 70%
			{
				this->genes[i].generate();
			}
		}
	}
};

class SharedGenome
{
public:
	int nbBusters;
	int score;
	Genome genomes[MAX_BUSTER];
	SharedGenome() : nbBusters(0) , score(0) {}
	void generate(int nbBusters)
	{
		this->nbBusters = nbBusters;
		for (int i = 0; i < this->nbBusters; ++i)
		{
			this->genomes[i].generate();
		}
	}
	void shift()
	{
		for (int i = 0; i < MAX_BUSTER; ++i) {
			this->genomes[i].shift();
		}
	}
	// cross corresponding genomes
	void cross(const SharedGenome& sg1, const SharedGenome& sg2)
	{
		for (int i = 0; i < nbBusters; ++i)
		{
			this->genomes[i].cross(sg1.genomes[i], sg2.genomes[i]);
		}
	}
	void mutate()
	{
		for (int i = 0; i < nbBusters; ++i)
		{
			this->genomes[i].mutate();
		}
	}
	void evaluate(const Game& game)
	{
		Game simulatedGame(game);
		// Run simulation on each turn + feed score
		int totalScore = 0;
		for (int i = 0; i < GEN_TURNS; ++i)
		{
			int turnScore = 0;
			// Decrement Ghost counts
			for (int j = 0; j < simulatedGame.ghostCount; ++j)
			{
				//TODO : in function resetGhost
				Ghost& ghost = simulatedGame.ghosts[j];
				ghost.value -= ghost.myBustersCount;
				ghost.myBustersCount = 0;
			}

			// Compute stuns
			for (int j = 0; j < this->nbBusters; ++j) // TODO : Later will take in count ennemy busters
			{
				simulatedGame.simulateMyStuns(j, this->genomes[j].genes[i], turnScore); // BusterId, Gene
			}

			// Other Actions
			for (int j = 0; j < this->nbBusters; ++j)
			{
				simulatedGame.simulateAction(j, this->genomes[j].genes[i], turnScore); // BusterId, Gene
			}

			// Compute Ghosts
			for (int j = 0; j < simulatedGame.ghostCount; ++j)
			{
				simulatedGame.simulateGhost(j, turnScore);
			}
			// Score
			totalScore += turnScore;
			simulatedGame.turn++;
		}
		this->score = totalScore;
	}
	bool operator<(const SharedGenome& gen) { return this->score < gen.score; }
};

class Genetic
{
public:
	SharedGenome population[POP_SIZE];
	Genetic(int nbBusters)
	{
		for (int i = 0; i < POP_SIZE; ++i)
		{
			population[i].generate(nbBusters);
		}
	}

	void computeStrategy(Game& game)
	{
		// Shift first 2 elements to keep previous strategy in mind and regenerate the rest
		this->shift();
		// First score / sort
		this->score(game, true);
		this->sort();
		// for SIMUL_NB : cross and mutate on second half / score / sort
		for (int i = 0; i < SIMUL_NB; ++i)
		{
			this->cross();
			this->mutate();
			this->score(game);
			this->sort();
		}
		// Feed myBusters decision with top 1 element
		this->feedWithBest(game);
	}

	void shift()
	{
		for (int i = 0; i < 2; ++i)
		{
			this->population[i].shift();
		}
		for (int i = 2; i < POP_SIZE; ++i)
		{

		}
	}
	void score(const Game& game, bool first = false)
	{
		int limit = first ? POP_SIZE : POP_SIZE/2;
		for (int i = 0; i < limit; ++i)
		{
			this->population[i].evaluate(game);
		}
	}

	void sort()
	{
		mergeSort(this->population, POP_SIZE);
	}

	void mergeSort(SharedGenome tableau[], const int longueur)
	{
		if (longueur > 0)
			mergeSortAux(tableau, 0, longueur - 1);
	}

	void mergeSortAux(SharedGenome tableau[], const int debut, const int fin)
	{
		if (debut != fin) // condition d'arrêt
		{
			int milieu = (debut + fin) / 2;
			mergeSortAux(tableau, debut, milieu); // trie partie1
			mergeSortAux(tableau, milieu + 1, fin); // trie partie2
			merge(tableau, debut, milieu, fin); // fusion des deux parties
		}
	}

	void merge(SharedGenome tableau[], const int debut1, const int fin1, const int fin2)
	{
		SharedGenome* tableau2 = new SharedGenome[(fin1 - debut1 + 1)];
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

	void cross()
	{
		for (int i = 0; i < POP_SIZE/2; ++i)
		{
			this->population[i].cross(population[(rand() % POP_SIZE / 2) + POP_SIZE/2], population[(rand() % POP_SIZE / 2) + POP_SIZE / 2]);
		}
	}
	void mutate()
	{
		for (int i = 0; i < POP_SIZE / 2; ++i)
		{
			this->population[i].mutate();
		}
	}
	void feedWithBest(Game& game)
	{
		cerr << "SharedGenome score : " << this->population[POP_SIZE - 1].score << endl;
		for (int i = 0; i < game.bustersPerPayer; ++i)
		{
			game.myBusters[i].decision = this->population[POP_SIZE-1].genomes[i].genes[0];
		}
	}
};

int main()
{
	/* initialize random seed: */
	srand(time(NULL));

	int bustersPerPlayer; // the amount of busters you control
	cin >> bustersPerPlayer; cin.ignore();
	int ghostCount; // the amount of ghosts on the map
	cin >> ghostCount; cin.ignore();
	int myTeamId; // if this is 0, your base is on the top left of the map, if it is one, on the bottom right
	cin >> myTeamId; cin.ignore();

	Game game(myTeamId,bustersPerPlayer,ghostCount);
	Genetic genetic(bustersPerPlayer);
	
	// game loop
	while (1) {
		++game.turn;
		int entities; // the number of busters and ghosts visible to you
		cin >> entities; cin.ignore();
		for (int i = 0; i < entities; i++) {
			int entityId; // buster id or ghost id
			int x;
			int y; // position of this buster / ghost
			int entityType; // the team id if it is a buster, -1 if it is a ghost.
			int state; // For busters: 0=idle, 1=carrying a ghost. For ghosts: remaining stamina points.
			int value; // For busters: Ghost id being carried/busted or number of turns left when stunned. For ghosts: number of busters attempting to trap this ghost.
			cin >> entityId >> x >> y >> entityType >> state >> value; cin.ignore();
			int inputs[] = { entityId, x, y, entityType, state, value };
			cerr << entityId << " " << x << " " << y << " " << entityType << " " << state << " " << value << endl;
			game.addEntity(inputs);
		}
		auto start_time = chrono::high_resolution_clock::now();
		genetic.computeStrategy(game);
		auto end_time = chrono::high_resolution_clock::now();
		cerr << "i did it  in : " << chrono::duration_cast<chrono::milliseconds>(end_time - start_time).count() << " milliseconds!" << endl;
		for (int i = 0; i < bustersPerPlayer; i++) {
			game.printDecision(i);
			// Write an action using cout. DON'T FORGET THE "<< endl"
			// To debug: cerr << "Debug messages..." << endl;

			//cout << "MOVE 8000 4500" << endl; // MOVE x y | BUST id | RELEASE | STUN id
		}
	}
}
