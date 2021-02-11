//using System;
//using System.Diagnostics;

///**
// * Auto-generated code below aims at helping you parse
// * the standard input according to the problem statement.
// **/

//#region Board classes
//enum Direction
//{
//    N,
//    NE,
//    E,
//    SE,
//    S,
//    SW,
//    W,
//    NW
//}

//struct Action
//{
//    public string Type { get; set; }
//    public int Index { get; set; }
//    public int Dir1 { get; set; }
//    public int Dir2 { get; set; }

//    public override string ToString()
//    {
//        return $"{Type} {Index} {((Direction)Dir1).ToString()} {((Direction)Dir2).ToString()}";
//    }
//}

//struct Unit
//{
//    public int X { get; set; }
//    public int Y { get; set; }
//    public bool Updated => X >= 0;
//}

//struct Board
//{
//    public static int[,] Offsets = new int[8, 2] { { -1, 0 }, { -1, 1 }, { 0, 1 }, { 1, 1 }, { 1, 0 }, { 1, -1 }, { 0, -1 }, { -1, -1 } };

//    public int Size { get; set; }
//    public int UnitsPerPlayer { get; set; }

//    public char[,] Cases { get; set; }
//    public Unit[,] Units { get; set; } // 0 mine / 1 ennemy
//    public int Score { get; set; }
//    public int TowersEnded { get; set; }

//    public void Init(int size, int unitsPerPlayer)
//    {
//        Size = size;
//        UnitsPerPlayer = unitsPerPlayer;
//        Cases = new char[size, size];
//        Units = new Unit[2, unitsPerPlayer];
//        Score = 0;
//        TowersEnded = 0;
//    }

//    public void Copy(Board other)
//    {
//        // TODO : put these 2 constants in Player
//        Size = other.Size;
//        UnitsPerPlayer = other.UnitsPerPlayer;
//        for (int i = 0; i < Size; ++i)
//        {
//            for (int j = 0; j < Size; ++j)
//            {
//                Cases[i, j] = other.Cases[i, j];
//            }
//        }
//        for (int i = 0; i < 2; ++i)
//        {
//            for (int j = 0; j < UnitsPerPlayer; ++j)
//            {
//                Units[i, j].X = other.Units[i, j].X;
//                Units[i, j].Y = other.Units[i, j].Y;
//            }
//        }
//        Score = other.Score;
//        TowersEnded = other.TowersEnded;
//    }
//}
//#endregion

//static class Player
//{
//    static Board[] Boards = new Board[Depth + 1];
//    static Action[,] LegalMoves = new Action[Depth, MaxLegalActions];
//    const int Depth = 2;
//    const int MaxLegalActions = 100;

//    static void Main(string[] args)
//    {
//        #region Init
//        string[] inputs;
//        int size = int.Parse(Console.ReadLine());
//        int unitsPerPlayer = int.Parse(Console.ReadLine());

//        for (int d = 0; d < Depth + 1; ++d)
//        {
//            Boards[d].Init(size, unitsPerPlayer);
//        }

//        #endregion

//        // Game loop
//        while (true)
//        {
//            // Reset scores
//            Boards[0].Score = 0;
//            Boards[0].TowersEnded = 0;

//            #region Reading inputs
//            // Feed cases
//            for (int i = 0; i < size; ++i)
//            {
//                string row = Console.ReadLine();
//                for (int j = 0; j < size; ++j)
//                {
//                    Boards[0].Cases[i, j] = row[j];
//                }
//            }
//            // Feed my units
//            for (int i = 0; i < unitsPerPlayer; ++i)
//            {
//                inputs = Console.ReadLine().Split(' ');
//                int unitX = int.Parse(inputs[0]);
//                int unitY = int.Parse(inputs[1]);
//                Boards[0].Units[0, i].X = unitY;
//                Boards[0].Units[0, i].Y = unitX;
//                Console.Error.WriteLine($"My position : {unitY} {unitX}");
//            }
//            // Feed ennemy units
//            for (int i = 0; i < unitsPerPlayer; ++i)
//            {
//                inputs = Console.ReadLine().Split(' ');
//                int otherX = int.Parse(inputs[0]);
//                int otherY = int.Parse(inputs[1]);
//                Boards[0].Units[1, i].X = otherY;
//                Boards[0].Units[1, i].Y = otherX;
//                Console.Error.WriteLine($"Ennemy position : {otherY} {otherX}");
//            }
//            int legalActions = int.Parse(Console.ReadLine());
//            for (int i = 0; i < legalActions; ++i)
//            {
//                // Read but ignore : should read them to find out first round of minmax
//                Console.ReadLine();
//            }
//            #endregion

//            // Find best solution : call minmax
//            Stopwatch sw = new Stopwatch();
//            sw.Start();
//            var best = MinMax();
//            sw.Stop();
//            Console.Error.WriteLine($"MinMax done in {(1000000L * sw.ElapsedTicks / Stopwatch.Frequency)} us ({sw.ElapsedMilliseconds} ms).");

//            Console.WriteLine(best.ToString());
//        }
//    }

//    static Action MinMax()
//    {
//        // Init val
//        int maxVal = -100000;
//        int best = 0;

//        // Generate legal moves        
//        int nbLegalMoves = GenerateLegalMoves(0);

//        //Console.Error.WriteLine($"Legal Moves at depth {0} : {nbLegalMoves}.");
//        //for (int i = 0; i < nbLegalMoves; ++i)
//        //{
//        //    Console.Error.WriteLine($"{LegalMoves[0, i].ToString()}");
//        //}

//        for (int i = 0; i < nbLegalMoves; ++i)
//        {
//            // Simulate turn : TODO
//            Simulate(0, i);

//            int val = Min(1);

//            if (val > maxVal)
//            {
//                maxVal = val;
//                best = i;
//            }
//        }

//        return LegalMoves[0, best];
//    }

//    static int Min(int depth)
//    {
//        if (depth == Depth)
//        {
//            return Eval(depth);
//        }

//        int minVal = 1000000;
//        int nbLegalMoves = GenerateLegalMoves(depth);

//        if (nbLegalMoves == 0)
//        {
//            // Keep playing as ennemy is not visible
//            return Max(depth + 1);
//        }
//        else
//        {
//            for (int i = 0; i < nbLegalMoves; ++i)
//            {
//                Simulate(depth, i);

//                int val = Max(depth + 1);

//                if (val < minVal)
//                {
//                    minVal = val;
//                }
//            }
//        }

//        return minVal;
//    }

//    static int Max(int depth)
//    {
//        if (depth == Depth)
//        {
//            return Eval(depth);
//        }

//        int maxVal = -1000000;
//        int nbLegalMoves = GenerateLegalMoves(depth);

//        if (nbLegalMoves == 0)
//        {
//            // We are blocked !
//            Console.Error.WriteLine("Hi i'm blocked !");
//            return maxVal;
//        }
//        else
//        {
//            for (int i = 0; i < nbLegalMoves; ++i)
//            {
//                Simulate(depth, i);

//                int val = Min(depth + 1);

//                if (val > maxVal)
//                {
//                    maxVal = val;
//                }
//            }
//        }

//        return maxVal;
//    }

//    static int GenerateLegalMoves(int depth)
//    {
//        int number = 0;
//        int playerId = depth % 2;

//        Board board = Boards[depth];

//        // For all units
//        for (int u = 0; u < board.UnitsPerPlayer; ++u)
//        {
//            // Ignore if unit is not updated this turn
//            if (!board.Units[playerId, u].Updated)
//            {
//                continue;
//            }

//            Unit unit = board.Units[playerId, u];
//            for (int dir = 0; dir < 8; ++dir)
//            {
//                // Try MOVE or PUSH all directions
//                int newX = unit.X + Board.Offsets[dir, 0];
//                int newY = unit.Y + Board.Offsets[dir, 1];

//                // Cannot go out of map
//                if (newX < 0 || newX >= board.Size || newY < 0 || newY >= board.Size)
//                {
//                    continue;
//                }

//                var newValue = board.Cases[newX, newY];
//                // Cannot go in hole
//                if (newValue == '.')
//                {
//                    continue;
//                }

//                int newHeight = newValue - 48;

//                // Cannot go over 3
//                if (newHeight > 3)
//                {
//                    continue;
//                }

//                // Is there an opponent ? Decides MOVE or PUSH
//                bool isOccupiedByEnnemy = false;
//                for (int u2 = 0; u2 < board.UnitsPerPlayer; ++u2)
//                {
//                    // Ignore if ennemy unit is not updated this turn
//                    if (!board.Units[1 - playerId, u2].Updated)
//                    {
//                        continue;
//                    }

//                    if (board.Units[1 - playerId, u2].X == newX && board.Units[1 - playerId, u2].Y == newY)
//                    {
//                        isOccupiedByEnnemy = true;
//                    }
//                }

//                if (isOccupiedByEnnemy)
//                {
//                    // Try direction where to push (dir2 = dir | dir - 1 | dir + 1)
//                    for (int dir2 = dir - 1; dir2 < dir + 2; ++dir2)
//                    {
//                        int realDir2 = dir2 < 0 ? dir2 + 8 : (dir2 > 7 ? dir2 - 8 : dir2);
//                        // New position has to be legal position
//                        int pushX = newX + Board.Offsets[realDir2, 0];
//                        int pushY = newY + Board.Offsets[realDir2, 1];

//                        // Cannot go out of map
//                        if (pushX < 0 || pushX >= board.Size || pushY < 0 || pushY >= board.Size)
//                        {
//                            continue;
//                        }

//                        var pushValue = board.Cases[pushX, pushY];
//                        // Cannot go in hole
//                        if (pushValue == '.')
//                        {
//                            continue;
//                        }

//                        int pushHeight = pushValue - 48;
//                        // Cannot go over 3
//                        if (pushHeight > 3)
//                        {
//                            continue;
//                        }

//                        // Has to got no one in there
//                        isOccupiedByEnnemy = false;
//                        for (int u2 = 0; u2 < board.UnitsPerPlayer; ++u2)
//                        {
//                            // Ignore if ennemy unit is not updated this turn
//                            if (!board.Units[1 - playerId, u2].Updated)
//                            {
//                                continue;
//                            }

//                            if (board.Units[1 - playerId, u2].X == pushX && board.Units[1 - playerId, u2].Y == pushY)
//                            {
//                                isOccupiedByEnnemy = true;
//                            }
//                        }

//                        bool isOccupiedByPlayer = false;
//                        for (int u2 = 0; u2 < board.UnitsPerPlayer; ++u2)
//                        {
//                            // Ignore if my unit is not updated this turn
//                            if (!board.Units[playerId, u2].Updated)
//                            {
//                                continue;
//                            }

//                            if (board.Units[playerId, u2].X == pushX && board.Units[playerId, u2].Y == pushY)
//                            {
//                                isOccupiedByPlayer = true;
//                            }
//                        }

//                        if (isOccupiedByEnnemy || isOccupiedByPlayer)
//                        {
//                            continue;
//                        }

//                        // This action is legal !
//                        LegalMoves[depth, number].Type = "PUSH&BUILD";
//                        LegalMoves[depth, number].Index = u;
//                        LegalMoves[depth, number].Dir1 = dir;
//                        LegalMoves[depth, number].Dir2 = realDir2;
//                        ++number;
//                    }
//                }
//                else
//                {
//                    // Cannot go there if too high
//                    int currentHeight = board.Cases[unit.X, unit.Y] - 48;
//                    if (newHeight - currentHeight > 1)
//                    {
//                        continue;
//                    }

//                    // Is it occupied by my units
//                    bool isOccupiedByPlayer = false;
//                    for (int u2 = 0; u2 < board.UnitsPerPlayer; ++u2)
//                    {
//                        // Ignore if my unit is not updated this turn
//                        if (!board.Units[playerId, u2].Updated)
//                        {
//                            continue;
//                        }

//                        if (board.Units[playerId, u2].X == newX && board.Units[playerId, u2].Y == newY)
//                        {
//                            isOccupiedByPlayer = true;
//                        }
//                    }

//                    // Cannot move there
//                    if (isOccupiedByPlayer)
//                    {
//                        continue;
//                    }

//                    // Try MOVE&BUILD in all new directions
//                    for (int dir2 = 0; dir2 < 8; ++dir2)
//                    {
//                        int buildX = newX + Board.Offsets[dir2, 0];
//                        int buildY = newY + Board.Offsets[dir2, 1];

//                        // Cannot build out of map
//                        if (buildX < 0 || buildX >= board.Size || buildY < 0 || buildY >= board.Size)
//                        {
//                            continue;
//                        }

//                        var buildValue = board.Cases[buildX, buildY];
//                        // Cannot build on a hole
//                        if (buildValue == '.')
//                        {
//                            continue;
//                        }

//                        int buildHeight = buildValue - 48;
//                        // Cannot build over 3
//                        if (buildHeight > 3)
//                        {
//                            continue;
//                        }

//                        // Is this occupied ?
//                        isOccupiedByEnnemy = false;
//                        for (int u2 = 0; u2 < board.UnitsPerPlayer; ++u2)
//                        {
//                            // Ignore if ennemy unit is not updated this turn
//                            if (!board.Units[1 - playerId, u2].Updated)
//                            {
//                                continue;
//                            }

//                            if (board.Units[1 - playerId, u2].X == buildX && board.Units[1 - playerId, u2].Y == buildY)
//                            {
//                                isOccupiedByEnnemy = true;
//                            }
//                        }

//                        isOccupiedByPlayer = false;
//                        for (int u2 = 0; u2 < board.UnitsPerPlayer; ++u2)
//                        {
//                            // Ignore if my unit is not updated this turn
//                            if (!board.Units[playerId, u2].Updated)
//                            {
//                                continue;
//                            }

//                            if (board.Units[playerId, u2].X == buildX && board.Units[playerId, u2].Y == buildY)
//                            {
//                                isOccupiedByPlayer = true;
//                            }
//                        }

//                        // Not start position and is occupied, not legal
//                        if ((buildX != unit.X || buildY != unit.Y) && (isOccupiedByEnnemy || isOccupiedByPlayer))
//                        {
//                            continue;
//                        }

//                        // This action is legal !
//                        LegalMoves[depth, number].Type = "MOVE&BUILD";
//                        LegalMoves[depth, number].Index = u;
//                        LegalMoves[depth, number].Dir1 = dir;
//                        LegalMoves[depth, number].Dir2 = dir2;
//                        ++number;
//                    }
//                }
//            }
//        }

//        Console.Error.WriteLine($"{number} legal moves at depth {depth}.");
//        return number;
//    }

//    static void Simulate(int depth, int moveId)
//    {
//        var move = LegalMoves[depth, moveId];
//        if (depth == 0)
//        {
//            Console.Error.WriteLine($"Simulate {move.ToString()} at depth {depth}.");
//        }

//        int playerId = depth % 2;

//        // Copy Current Board in depth + 1
//        Boards[depth + 1].Copy(Boards[depth]);

//        // Start Simulating
//        var unit = Boards[depth + 1].Units[playerId, move.Index];

//        // MOVE&BUILD :
//        if (move.Type == "MOVE&BUILD")
//        {
//            // MOVE
//            unit.X = unit.X + Board.Offsets[move.Dir1, 0];
//            unit.Y = unit.Y + Board.Offsets[move.Dir1, 1];

//            // Does score ?
//            var newValue = Boards[depth + 1].Cases[unit.X, unit.Y];
//            if ((newValue - 48) == 3)
//            {
//                Boards[depth + 1].Score += (playerId == 0 ? +1 : -1);
//            }

//            // BUILD
//            var buildX = unit.X + Board.Offsets[move.Dir2, 0];
//            var buildY = unit.Y + Board.Offsets[move.Dir2, 1];
//            Boards[depth + 1].Cases[buildX, buildY] = (char)(Boards[depth + 1].Cases[buildX, buildY] + 1);

//            if ((Boards[depth + 1].Cases[buildX, buildY] - 48) == 4)
//            {
//                Boards[depth + 1].TowersEnded += playerId == 0 ? +1 : 0;
//            }
//        }
//        else if (move.Type == "PUSH&BUILD")
//        {
//            // PUSH
//            var newX = unit.X + Board.Offsets[move.Dir1, 0];
//            var newY = unit.Y + Board.Offsets[move.Dir1, 1];

//            for (int u = 0; u < Boards[depth + 1].UnitsPerPlayer; ++u)
//            {
//                // Ignore if ennemy unit is not updated this turn
//                if (!Boards[depth + 1].Units[1 - playerId, u].Updated)
//                {
//                    continue;
//                }

//                if (Boards[depth + 1].Units[1 - playerId, u].X == newX && Boards[depth + 1].Units[1 - playerId, u].Y == newY)
//                {
//                    Boards[depth + 1].Units[1 - playerId, u].X = Boards[depth + 1].Units[1 - playerId, u].X + Board.Offsets[move.Dir2, 0];
//                    Boards[depth + 1].Units[1 - playerId, u].Y = Boards[depth + 1].Units[1 - playerId, u].Y + Board.Offsets[move.Dir2, 1];
//                    break;
//                }
//            }

//            // BUILD
//            Boards[depth + 1].Cases[newX, newY] = (char)(Boards[depth + 1].Cases[newX, newY] + 1);

//            if ((Boards[depth + 1].Cases[newX, newY] - 48) == 4)
//            {
//                Boards[depth + 1].TowersEnded += playerId == 0 ? +1 : 0;
//            }
//        }
//    }

//    static int Eval(int depth)
//    {
//        int score = 0;

//        int sumOfHeights = 0;
//        //int sumOfNeighborsHeights = 0;
//        int sumOfEnnemyHeights = 0;
//        for (int u = 0; u < Boards[depth].UnitsPerPlayer; ++u)
//        {
//            var unit = Boards[depth].Units[0, u];
//            sumOfHeights += (Boards[depth].Cases[unit.X, unit.Y] - 48);

//            //for (int dir = 0; dir < 8; ++dir)
//            //{
//            //    var nextX = unit.X + Board.Offsets[dir, 0];
//            //    var nextY = unit.Y + Board.Offsets[dir, 1];

//            //    if (nextX < 0 || nextX >= board.Size || nextY < 0 || nextY >= board.Size)
//            //    {
//            //        continue;
//            //    }

//            //    var nextValue = board.Cases[nextX, nextY];

//            //    if (nextValue == '.')
//            //    {
//            //        continue;
//            //    }

//            //    int nextHeight = nextValue - 48;
//            //    if (nextHeight > 3)
//            //    {
//            //        sumOfNeighborsHeights -= 10;
//            //    }
//            //}

//            var ennemyUnit = Boards[depth].Units[1, u];

//            if (ennemyUnit.Updated)
//            {
//                sumOfEnnemyHeights += (Boards[depth].Cases[ennemyUnit.X, ennemyUnit.Y] - 48);
//            }
//        }

//        score = Boards[depth].Score * 100 - Boards[depth].TowersEnded * 20 + sumOfHeights - sumOfEnnemyHeights * 5;

//        Console.Error.WriteLine($"Evaluate at depth {depth} : {Boards[depth].Score} / {score}");
//        return score;
//    }
//}