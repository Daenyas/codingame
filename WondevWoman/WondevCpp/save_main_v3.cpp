//// Top 40 gold / Top 100 Global
//#include <iostream>
//#include <string>
//#include <ctime>
//#include <chrono>
//
//#pragma GCC optimize "Ofast,omit-frame-pointer,inline,unroll-loops"
//
//#define MAX_SIZE 7
//#define MAX_UNIT 3
//#define DEPTH 2
//#define MAX_LEGAL_ACTIONS 176
//
//using namespace std;
///**
//* Auto-generated code below aims at helping you parse
//* the standard input according to the problem statement.
//**/
//
//typedef struct Unit
//{
//public:
//	int X;
//	int Y;
//	int LastUpdated;
//
//	inline bool CanBeUsed()
//	{
//		return LastUpdated == 0; // For now
//	}
//} Unit;
//
//typedef struct Board
//{
//public:
//	static const string Directions[8];
//	static const int Offsets[8][2];
//	static int Size;
//	static int UnitsPerPlayer;
//
//	static bool NoEnnemy;
//	char Cases[MAX_SIZE][MAX_SIZE];
//	Unit Units[2][MAX_UNIT]; // 0 mine / 1 ennemy
//
//	int PointsScored[2];
//	int PossibleMoves[2][MAX_UNIT];
//	int PushedHeightDifference[2];
//	int HeightDifference[2];
//
//	Board& operator=(const Board& other)
//	{
//		for (int i = 0; i < Size; ++i)
//		{
//			for (int j = 0; j < Size; ++j)
//			{
//				this->Cases[i][j] = other.Cases[i][j];
//			}
//		}
//
//		for (int i = 0; i < 2; ++i)
//		{
//			for (int j = 0; j < UnitsPerPlayer; ++j)
//			{
//				this->Units[i][j].X = other.Units[i][j].X;
//				this->Units[i][j].Y = other.Units[i][j].Y;
//				this->Units[i][j].LastUpdated = other.Units[i][j].LastUpdated;
//			}
//		}
//
//		// Scoring
//		this->PointsScored[0] = other.PointsScored[0];
//		this->PointsScored[1] = other.PointsScored[1];
//
//		for (int i = 0; i < 2; ++i)
//		{
//			for (int j = 0; j < UnitsPerPlayer; ++j)
//			{
//				this->PossibleMoves[i][j] = 0;
//			}
//		}
//
//		this->PushedHeightDifference[0] = other.PushedHeightDifference[0];
//		this->PushedHeightDifference[1] = other.PushedHeightDifference[1];
//
//		this->HeightDifference[0] = other.HeightDifference[0];
//		this->HeightDifference[1] = other.HeightDifference[1];
//
//		return *this;
//	}
//
//	inline void Reset()
//	{
//		this->PointsScored[0] = 0;
//		this->PointsScored[1] = 0;
//
//		for (int i = 0; i < 2; ++i)
//		{
//			for (int j = 0; j < UnitsPerPlayer; ++j)
//			{
//				this->PossibleMoves[i][j] = 0;
//			}
//		}
//
//		this->PushedHeightDifference[0] = 0;
//		this->PushedHeightDifference[1] = 0;
//
//		this->HeightDifference[0] = 0;
//		this->HeightDifference[1] = 0;
//	}
//
//	inline void Debug()
//	{
//		cerr << "Board : " << endl;
//		for (int i = 0; i < Board::Size; ++i)
//		{
//			string line = "";
//			for (int j = 0; j < Board::Size; ++j)
//			{
//				line += Cases[i][j];
//			}
//			cerr << line << endl;
//		}
//	}
//
//	inline bool IsOutOfMap(int x, int y) const
//	{
//		// Check if out of map
//		if (x < 0 || x >= Board::Size || y < 0 || y >= Board::Size)
//		{
//			return true;
//		}
//
//		return false;
//	}
//
//	bool CanDoMove(int x, int y, int dir) const
//	{
//		int currentHeight = Cases[x][y] - 48;
//		int newX = x + Board::Offsets[dir][0];
//		int newY = y + Board::Offsets[dir][1];
//
//		// Check if out of map
//		if (IsOutOfMap(newX, newY))
//		{
//			return false;
//		}
//
//		char newValue = Cases[newX][newY];
//		// Cannot go in hole
//		if (newValue == '.')
//		{
//			return false;
//		}	
//
//		int newHeight = newValue - 48;
//		// Can't go to the sky
//		if (newHeight > 3)
//		{
//			return false;
//		}
//
//		// Can't go on me or ennemy
//		for (int u = 0; u < Board::UnitsPerPlayer; ++u)
//		{
//			Unit myUnit = Units[0][u];
//			if (myUnit.X == newX && myUnit.Y == newY)
//			{
//				return false;
//			}
//
//			Unit ennemyUnit = Units[1][u];
//			if (ennemyUnit.CanBeUsed() && ennemyUnit.X == newX && ennemyUnit.Y == newY) // TODO only if updated or sure estimation
//			{
//				return false;
//			}
//		}
//
//		// Check legal move up of 1 or go down
//		return newHeight - currentHeight <= 1;
//	}
//} Board;
//
//const string Board::Directions[8] = { "N", "NE", "E", "SE", "S", "SW", "W", "NW" };
//const int Board::Offsets[8][2] = { { -1, 0 },{ -1, 1 },{ 0, 1 },{ 1, 1 },{ 1, 0 },{ 1, -1 },{ 0, -1 },{ -1, -1 } };
//int Board::Size = 0;
//int Board::UnitsPerPlayer = 0;
//bool Board::NoEnnemy = false;
//
//typedef struct Action
//{
//public:
//	string Type;
//	int Index;
//	int Dir1;
//	int Dir2;
//
//	inline void print(string additionalText)
//	{
//		cout << Type << " " << Index << " " << Board::Directions[Dir1] << " " << Board::Directions[Dir2] << " " << additionalText << endl;
//	}
//	inline void Debug()
//	{
//		cerr << Type << " " << Index << " " << Board::Directions[Dir1] << " " << Board::Directions[Dir2] << endl;
//	}
//} Action;
//
//// For MinMax perfs
//Board Boards[DEPTH + 1];
//Action LegalMoves[DEPTH + 1][MAX_UNIT][MAX_LEGAL_ACTIONS];
//
//// MinMax Helpers
//int GenerateLegalMoves(int depth, int playerId, int u)
//{
//	int number = 0;
//
//	Board& board = Boards[depth];
//	Unit& unit = board.Units[playerId][u];
//
//	// Ignore if unit is not updated this turn
//	if (!unit.CanBeUsed())
//	{
//		return 0;
//	}
//
//	for (int dir = 0; dir < 8; ++dir)
//	{
//		// Try MOVE or PUSH all directions
//		int newX = unit.X + Board::Offsets[dir][0];
//		int newY = unit.Y + Board::Offsets[dir][1];
//
//		// Cannot go out of map
//		if (newX < 0 || newX >= Board::Size || newY < 0 || newY >= Board::Size)
//		{
//			continue;
//		}
//
//		char newValue = board.Cases[newX][newY];
//		// Cannot go in hole
//		if (newValue == '.')
//		{
//			continue;
//		}
//
//		int newHeight = newValue - 48;
//
//		// Cannot go over 3
//		if (newHeight > 3)
//		{
//			continue;
//		}
//
//		// Is there an opponent ? Decides MOVE or PUSH
//		bool isOccupiedByEnnemy = false;
//		for (int u2 = 0; u2 < Board::UnitsPerPlayer; ++u2)
//		{
//			// Ignore if ennemy unit is not updated this turn
//			if (!board.Units[1 - playerId][u2].CanBeUsed())
//			{
//				continue;
//			}
//
//			if (board.Units[1 - playerId][u2].X == newX && board.Units[1 - playerId][u2].Y == newY)
//			{
//				isOccupiedByEnnemy = true;
//			}
//		}
//
//		if (isOccupiedByEnnemy)
//		{
//			// Try direction where to push (dir2 = dir | dir - 1 | dir + 1)
//			for (int dir2 = dir - 1; dir2 < dir + 2; ++dir2)
//			{
//				int realDir2 = dir2 < 0 ? dir2 + 8 : (dir2 > 7 ? dir2 - 8 : dir2);
//				// New position has to be legal position
//				int pushX = newX + Board::Offsets[realDir2][0];
//				int pushY = newY + Board::Offsets[realDir2][1];
//
//				// Cannot go out of map
//				if (pushX < 0 || pushX >= Board::Size || pushY < 0 || pushY >= Board::Size)
//				{
//					continue;
//				}
//
//				char pushValue = board.Cases[pushX][pushY];
//				// Cannot go in hole
//				if (pushValue == '.')
//				{
//					continue;
//				}
//
//				int pushHeight = pushValue - 48;
//				// Cannot go over 3
//				if (pushHeight > 3)
//				{
//					continue;
//				}
//
//				if (pushHeight - newHeight > 1)
//				{
//					continue;
//				}
//
//				// Has to got no one in there
//				isOccupiedByEnnemy = false;
//				for (int u2 = 0; u2 < Board::UnitsPerPlayer; ++u2)
//				{
//					// Ignore if ennemy unit is not updated this turn
//					if (!board.Units[1 - playerId][u2].CanBeUsed())
//					{
//						continue;
//					}
//
//					if (board.Units[1 - playerId][u2].X == pushX && board.Units[1 - playerId][u2].Y == pushY)
//					{
//						isOccupiedByEnnemy = true;
//					}
//				}
//
//				bool isOccupiedByPlayer = false;
//				for (int u2 = 0; u2 < Board::UnitsPerPlayer; ++u2)
//				{
//					// Ignore if my unit is not updated this turn
//					if (!board.Units[playerId][u2].CanBeUsed())
//					{
//						continue;
//					}
//
//					if (board.Units[playerId][u2].X == pushX && board.Units[playerId][u2].Y == pushY)
//					{
//						isOccupiedByPlayer = true;
//					}
//				}
//
//				if (isOccupiedByEnnemy || isOccupiedByPlayer)
//				{
//					continue;
//				}
//
//				// This action is legal !
//				LegalMoves[depth][u][number].Type = "PUSH&BUILD";
//				LegalMoves[depth][u][number].Index = u;
//				LegalMoves[depth][u][number].Dir1 = dir;
//				LegalMoves[depth][u][number].Dir2 = realDir2;
//				++number;
//			}
//		}
//		else
//		{
//			// Cannot go there if too high
//			int currentHeight = board.Cases[unit.X][unit.Y] - 48;
//			if (newHeight - currentHeight > 1)
//			{
//				continue;
//			}
//
//			// Is it occupied by my units
//			bool isOccupiedByPlayer = false;
//			for (int u2 = 0; u2 < Board::UnitsPerPlayer; ++u2)
//			{
//				// Ignore if my unit is not updated this turn
//				if (!board.Units[playerId][u2].CanBeUsed())
//				{
//					continue;
//				}
//
//				if (board.Units[playerId][u2].X == newX && board.Units[playerId][u2].Y == newY)
//				{
//					isOccupiedByPlayer = true;
//				}
//			}
//
//			// Cannot move there
//			if (isOccupiedByPlayer)
//			{
//				continue;
//			}
//
//			// Try MOVE&BUILD in all new directions
//			for (int dir2 = 0; dir2 < 8; ++dir2)
//			{
//				int buildX = newX + Board::Offsets[dir2][0];
//				int buildY = newY + Board::Offsets[dir2][1];
//
//				// Cannot build out of map
//				if (buildX < 0 || buildX >= Board::Size || buildY < 0 || buildY >= Board::Size)
//				{
//					continue;
//				}
//
//				char buildValue = board.Cases[buildX][buildY];
//				// Cannot build on a hole
//				if (buildValue == '.')
//				{
//					continue;
//				}
//
//				int buildHeight = buildValue - 48;
//				// Cannot build over 3
//				if (buildHeight > 3)
//				{
//					continue;
//				}
//
//				// Is this occupied ?
//				isOccupiedByEnnemy = false;
//				for (int u2 = 0; u2 < Board::UnitsPerPlayer; ++u2)
//				{
//					// Ignore if ennemy unit is not updated this turn
//					if (!board.Units[1 - playerId][u2].CanBeUsed())
//					{
//						continue;
//					}
//
//					if (board.Units[1 - playerId][u2].X == buildX && board.Units[1 - playerId][u2].Y == buildY)
//					{
//						isOccupiedByEnnemy = true;
//					}
//				}
//
//				isOccupiedByPlayer = false;
//				for (int u2 = 0; u2 < Board::UnitsPerPlayer; ++u2)
//				{
//					// Ignore if my unit is not updated this turn
//					if (!board.Units[playerId][u2].CanBeUsed())
//					{
//						continue;
//					}
//
//					if (board.Units[playerId][u2].X == buildX && board.Units[playerId][u2].Y == buildY)
//					{
//						isOccupiedByPlayer = true;
//					}
//				}
//
//				// Not start position and is occupied, not legal
//				if ((buildX != unit.X || buildY != unit.Y) && (isOccupiedByEnnemy || isOccupiedByPlayer))
//				{
//					continue;
//				}
//
//				// This action is legal !
//				LegalMoves[depth][u][number].Type = "MOVE&BUILD";
//				LegalMoves[depth][u][number].Index = u;
//				LegalMoves[depth][u][number].Dir1 = dir;
//				LegalMoves[depth][u][number].Dir2 = dir2;
//
//				board.PossibleMoves[playerId][u]++;			
//				++number;
//			}
//		}
//	}
//
//	// cerr << number << " legal moves at depth " << depth << " for unit " << u << endl;
//	return number;
//}
//
//void Simulate(int depth, int unitId, int moveId)
//{
//	Action& move = LegalMoves[depth][unitId][moveId];
//	int playerId = depth % 2;
//	Board& board = Boards[depth + 1];
//
//	// Copy Current Board in depth + 1	
//	board = Boards[depth];
//
//	// Start Simulating
//	Unit& unit = board.Units[playerId][move.Index];
//
//	// MOVE&BUILD :
//	if (move.Type == "MOVE&BUILD")
//	{
//		// MOVE
//		int oldHeight = Boards[depth].Cases[unit.X][unit.Y] - 48;
//		unit.X = unit.X + Board::Offsets[move.Dir1][0];
//		unit.Y = unit.Y + Board::Offsets[move.Dir1][1];
//		board.HeightDifference[playerId] = (int)((Boards[depth].Cases[unit.X][unit.Y] - 48)) - oldHeight;
//
//		// Does score ?
//		char newValue = board.Cases[unit.X][unit.Y];
//		if (newValue == '3')
//		{
//			board.PointsScored[playerId]++;
//		}
//
//		// BUILD
//		int buildX = unit.X + Board::Offsets[move.Dir2][0];
//		int buildY = unit.Y + Board::Offsets[move.Dir2][1];
//		board.Cases[buildX][buildY]++;
//	}
//	else if (move.Type == "PUSH&BUILD")
//	{
//		// PUSH
//		int newX = unit.X + Board::Offsets[move.Dir1][0];
//		int newY = unit.Y + Board::Offsets[move.Dir1][1];
//
//		for (int u = 0; u < Board::UnitsPerPlayer; ++u)
//		{
//			Unit& ennemyUnit = board.Units[1 - playerId][u];
//
//			// Ignore if ennemy unit is not updated this turn
//			if (!ennemyUnit.CanBeUsed())
//			{
//				continue;
//			}
//
//			if (ennemyUnit.X == newX && ennemyUnit.Y == newY)
//			{
//				int oldHeight = (int)((Boards[depth].Cases[ennemyUnit.X][ennemyUnit.Y] - 48));
//				ennemyUnit.X = ennemyUnit.X + Board::Offsets[move.Dir2][0];
//				ennemyUnit.Y = ennemyUnit.Y + Board::Offsets[move.Dir2][1];
//				board.PushedHeightDifference[playerId] = (int)((Boards[depth].Cases[ennemyUnit.X][ennemyUnit.Y] - 48)) - oldHeight;
//				break;
//			}
//		}
//
//		// BUILD
//		board.Cases[newX][newY]++;
//	}
//}
//
//bool Visited[MAX_SIZE][MAX_SIZE];
//
//int FloodFill(const Board& board, int x, int y)
//{
//
//	if (!Visited[x][y])
//	{
//		Visited[x][y] = true;
//		int total = 0;
//		for (int dir = 0; dir < 8; ++dir)
//		{
//			int newX = x + Board::Offsets[dir][0];
//			int newY = y + Board::Offsets[dir][1];
//
//			if (board.CanDoMove(x, y, dir))
//			{
//				++total;
//				total += FloodFill(board, newX, newY);
//			}			 
//		}
//		return total;
//	}
//	return 0;
//}
//
//int FloodFillPath(const Board& board, int playerId, int index)
//{
//	// Reset
//	for (int i = 0; i < Board::Size; ++i)
//	{
//		for (int j = 0; j < Board::Size; ++j)
//		{
//			Visited[i][j] = false;
//		}
//	}
//
//	// Rec floodfill from unit position
//	const Unit& unit = board.Units[playerId][index];
//	return FloodFill(board, unit.X, unit.Y);
//}
//
//int Eval(int depth)
//{
//	// Evaluate my scoring
//	Board& board = Boards[depth];	
//
//	// Possible moves
//	int blockedUnits = 0;
//	int ennemyBlockedUnits = 0;
//
//	int totalMoves = 0;
//	int ennemyTotalMoves = 0;
//	
//	int escapeMalus = 0;
//	int skyAround = 0;
//
//	int totalReachable = 0;
//
//	for (int u = 0; u < Board::UnitsPerPlayer; ++u)
//	{		
//		// Mine regenerated at this depth because I have to know if ennemy move fucks me up
//		GenerateLegalMoves(depth, 0, u);		
//		if (board.PossibleMoves[0][u] == 0)
//		{
//			++blockedUnits;
//		}
//		totalMoves += board.PossibleMoves[0][u];
//
//		// That will check if our previous move had blocked an ennemy
//		Unit& ennemy = board.Units[1][u];
//		// Ignore if unit is not updated this turn
//		if (ennemy.CanBeUsed())
//		{
//			if (Boards[depth - 1].PossibleMoves[1][u] == 0)
//			{
//				++ennemyBlockedUnits;
//			}
//			ennemyTotalMoves += Boards[depth - 1].PossibleMoves[1][u];
//		}
//
//		// Check my neighbors
//		Unit& myUnit = board.Units[0][u];
//		int unitEscapes = 0;
//		for (int dir = 0; dir < 8; ++dir)
//		{
//			bool isValid = board.CanDoMove(myUnit.X, myUnit.Y, dir);
//			if (isValid)
//			{
//				++unitEscapes;
//			}
//
//			int newX = myUnit.X + Board::Offsets[dir][0];
//			int newY = myUnit.Y + Board::Offsets[dir][1];
//			if (!board.IsOutOfMap(newX, newY) && board.Cases[newX][newY] == '4')
//			{
//				++skyAround;
//			}
//		}
//
//		if (unitEscapes < 3)
//		{
//			escapeMalus += (1000 - unitEscapes * 20);    
//		}
//
//		// Reachable areas
//		totalReachable += FloodFillPath(board, 0, u);
//	}
//
//	// Score calculation
//	int score = (Boards[depth].PointsScored[0] - Boards[depth].PointsScored[1]) * 600						// Scoring a point
//		- (Boards[depth].PushedHeightDifference[0] - Boards[depth].PushedHeightDifference[1]) * 500			// Pushing an ennemy away (-3 is good)
//		+ (2 * Boards[depth].HeightDifference[0] - Boards[depth].HeightDifference[1]) * 100                 // Better go up than down
//		- (blockedUnits - ennemyBlockedUnits) * 10000														// Really wrong if we are blocked
//		+ (totalMoves - ennemyTotalMoves) * 20																// Better not block moves
//		- skyAround	* 10																					// Not good if too much 4 around   
//		- escapeMalus																						// It is wrong if one unit is going to be trapped
//		+ totalReachable * 20																				// Better if chose a bigger area
//		;
//
//	return score;
//}
//
//// MinMax Funcs
//int Max(int depth);
//
//int Min(int depth)
//{
//	if (depth == DEPTH)
//	{
//		return Eval(depth);
//	}
//
//	if (Board::NoEnnemy)
//	{
//		// Keep playing as ennemy is not visible
//		// Copy Current Board in depth + 1	
//		Boards[depth + 1] = Boards[depth];
//		return Max(depth + 1);
//	}
//
//	int minVal = 33000000;
//
//	int nbLegalMoves[MAX_UNIT];
//	int blocked = 0;
//	for (int u = 0; u < Board::UnitsPerPlayer; ++u)
//	{
//		nbLegalMoves[u] = GenerateLegalMoves(depth, 1, u);		
//		if (nbLegalMoves[u] == 0)
//		{
//			++blocked;
//		}
//	}
//
//	if (blocked == Board::UnitsPerPlayer)
//	{
//		// Keep playing as ennemy can't play anymore
//		// Copy Current Board in depth + 1	
//		Boards[depth + 1] = Boards[depth];
//		return Max(depth + 1);
//	}
//	else
//	{
//		for (int u = 0; u < Board::UnitsPerPlayer; ++u)
//		{
//			for (int i = 0; i < nbLegalMoves[u]; ++i)
//			{
//				Simulate(depth, u, i);
//
//				int val = Max(depth + 1);
//
//				if (val < minVal)
//				{
//					minVal = val;
//				}
//			}
//		}
//	}
//
//	return minVal;
//}
//
//int Max(int depth)
//{
//	if (depth == DEPTH)
//	{
//		return Eval(depth);
//	}
//
//	int maxVal = -33000000;
//
//	int nbLegalMoves[MAX_UNIT];
//	int blocked = 0;
//	for (int u = 0; u < Board::UnitsPerPlayer; ++u)
//	{
//		nbLegalMoves[u] = GenerateLegalMoves(depth, 0, u);
//		if (nbLegalMoves[u] == 0)
//		{
//			++blocked;
//		}
//	}
//
//	if (blocked == Board::UnitsPerPlayer)
//	{
//		// We are going to be blocked !
//		return maxVal;
//	}
//	else
//	{
//		for (int u = 0; u < Board::UnitsPerPlayer; ++u)
//		{
//			for (int i = 0; i < nbLegalMoves[u]; ++i)
//			{
//				Simulate(depth, u, i);
//
//				int val = Min(depth + 1);
//
//				if (val > maxVal)
//				{
//					maxVal = val;
//				}
//			}
//		}
//	}
//
//	return maxVal;
//}
//
//void MinMax()
//{
//	// Init val
//	int maxVal = -100000;
//	Action* best = nullptr;
//
//	// Generate legal moves
//	int nbLegalMoves[MAX_UNIT];
//	int blocked = 0;
//	for (int u = 0; u < Board::UnitsPerPlayer; ++u)
//	{
//		nbLegalMoves[u] = GenerateLegalMoves(0, 0, u);
//		if (nbLegalMoves[u] == 0)
//		{
//			++blocked;
//		}
//	}
//
//	for (int u = 0; u < Board::UnitsPerPlayer; ++u)
//	{
//		for (int i = 0; i < nbLegalMoves[u]; ++i)
//		{
//			//LegalMoves[0][u][i].Debug();
//			// Simulate turn 
//			Simulate(0, u, i);
//			//Boards[1].Debug();
//
//			int val = Min(1);
//
//			if (val > maxVal)
//			{
//				maxVal = val;
//				best = &LegalMoves[0][u][i];
//			}
//		}
//	}
//
//	// Print best one
//	if (blocked == Board::UnitsPerPlayer)
//	{
//		cout << "I lost the game" << endl;
//	}
//	else
//	{
//		best->print(to_string(maxVal));
//	}
//}
//
//int main()
//{
//	int size;
//	cin >> size; cin.ignore();
//	Board::Size = size;
//	int unitsPerPlayer;
//	cin >> unitsPerPlayer; cin.ignore();
//	Board::UnitsPerPlayer = unitsPerPlayer;
//
//	for (int i = 0; i < unitsPerPlayer; i++)
//	{
//		Boards[0].Units[1][i].X = -1;
//		Boards[0].Units[1][i].Y = -1;
//		Boards[0].Units[1][i].LastUpdated = 1000;
//	}
//
//	// game loop
//	while (1)
//	{
//		// Reset scores
//		Boards[0].Reset();
//
//		for (int i = 0; i < size; i++)
//		{
//			string row;
//			cin >> row; cin.ignore();
//			for (int j = 0; j < size; ++j)
//			{
//				Boards[0].Cases[i][j] = row[j];
//			}
//		}
//
//		for (int i = 0; i < unitsPerPlayer; i++)
//		{
//			int unitX;
//			int unitY;
//			cin >> unitX >> unitY; cin.ignore();
//			Boards[0].Units[0][i].X = unitY;
//			Boards[0].Units[0][i].Y = unitX;
//			Boards[0].Units[0][i].LastUpdated = 0;
//			cerr << "My unit " << i << " : " << unitY << " / " << unitX << endl;
//		}
//
//		bool hasOneUpdated = false;
//		for (int i = 0; i < unitsPerPlayer; i++)
//		{
//			int otherX;
//			int otherY;
//			cin >> otherX >> otherY; cin.ignore();
//			if (otherX >= 0)
//			{
//				Boards[0].Units[1][i].X = otherY;
//				Boards[0].Units[1][i].Y = otherX;
//				Boards[0].Units[1][i].LastUpdated = 0;
//			}			
//			else
//			{
//				Boards[0].Units[1][i].LastUpdated++;
//			}
//			cerr << "Ennemy unit " << i << " from last " << Boards[0].Units[1][i].LastUpdated << " turns : " << otherY << " / " << otherX << endl;
//
//			if (otherX >= 0)
//			{
//				hasOneUpdated = true;
//			}
//		}
//		Board::NoEnnemy = !hasOneUpdated;
//
//		int legalActions;
//		cin >> legalActions; cin.ignore();
//		for (int i = 0; i < legalActions; i++)
//		{
//			string atype;
//			int index;
//			string dir1;
//			string dir2;
//			cin >> atype >> index >> dir1 >> dir2; cin.ignore();
//		}
//
//		// Write an action using cout. DON'T FORGET THE "<< endl"
//		// To debug: cerr << "Debug messages..." << endl;
//		auto start = std::chrono::high_resolution_clock::now();
//		MinMax();
//		auto end = std::chrono::high_resolution_clock::now();
//		auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
//		cerr << "It tooked me " << elapsed.count() << " milliseconds." << endl;
//	}
//}