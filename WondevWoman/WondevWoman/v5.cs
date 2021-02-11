using System;
using System.Linq;
using System.IO;
using System.Text;
using System.Collections;
using System.Collections.Generic;
using System.Diagnostics;

/**
 * Auto-generated code below aims at helping you parse
 * the standard input according to the problem statement.
 **/

#region Board classes
enum Direction
{
    N,
    NE,
    E,
    SE,
    S,
    SW,
    W,
    NW
}

public class Action
{
    public string Type { get; set; }
    public int Index { get; set; }
    public int Dir1 { get; set; }
    public int Dir2 { get; set; }

    //Scoring
    public bool PointScored { get; set; }
    public int HeightDifference { get; set; }
    public int EnnemyPushedHeightDifference { get; set; }

    public int FinalScore { get; set; }

    public override string ToString()
    {
        return $"{Type} {Index} {((Direction)Dir1).ToString()} {((Direction)Dir2).ToString()} {FinalScore}";
    }
}

public class Unit
{
    public int X { get; set; }
    public int Y { get; set; }
    public int Index { get; set; }
    public int PlayerId { get; set; }

    public int LastUpdate { get; set; }

    public Unit Copy()
    {
        Unit unit = new Unit();
        unit.X = X;
        unit.Y = Y;
        unit.Index = Index;
        unit.PlayerId = PlayerId;
        unit.LastUpdate = LastUpdate;
        return unit;
    }
}

public class Case
{
    public int X { get; set; }
    public int Y { get; set; }
    public bool IsHole { get; set; }
    public int Height { get; set; }

    public bool IsBlocked => Height > 3 || IsHole;

    public Case Copy()
    {
        Case other = new Case();
        other.X = X;
        other.Y = Y;
        other.IsHole = IsHole;
        other.Height = Height;
        return other;
    }

    public bool Visited { get; set; }
    public int Distance { get; set; }
}

public class Board
{
    public static readonly int[,] Offsets = new int[8, 2] { { -1, 0 }, { -1, 1 }, { 0, 1 }, { 1, 1 }, { 1, 0 }, { 1, -1 }, { 0, -1 }, { -1, -1 } };

    public static int Size { get; set; }
    public static int UnitsPerPlayer { get; set; }

    public Case[,] Cases { get; set; }
    public Unit[,] Units { get; set; } // 0 mine / 1 ennemy

    public Board()
    {
        Cases = new Case[Size, Size];
        Units = new Unit[2, UnitsPerPlayer];
    }

    public Board Copy()
    {
        Board other = new Board();
        for (int i = 0; i < Size; ++i)
        {
            for (int j = 0; j < Size; ++j)
            {
                other.Cases[i, j] = Cases[i, j].Copy();
            }
        }
        for (int i = 0; i < 2; ++i)
        {
            for (int j = 0; j < UnitsPerPlayer; ++j)
            {
                other.Units[i, j] = Units[i, j].Copy();
            }
        }
        return other;
    }

    public bool IsOutOfMap(int x, int y)
    {
        // Check if out of map
        if (x < 0 || x >= Board.Size || y < 0 || y >= Board.Size)
        {
            return true;
        }

        return false;
    }

    public bool CanMoveTo(int x, int y, int dir)
    {
        var currentHeight = Cases[x, y].Height;
        var newX = x + Board.Offsets[dir, 0];
        var newY = y + Board.Offsets[dir, 1];

        // Check if out of map
        if (IsOutOfMap(newX, newY))
        {
            return false;
        }

        // Check if is a hole
        if (Cases[newX, newY].IsBlocked)
        {
            return false;
        }

        var newHeight = Cases[newX, newY].Height;

        // Can't go to the sky
        if (newHeight > 3)
        {
            return false;
        }

        // Can't go on me or ennemy
        for (int u = 0; u < Board.UnitsPerPlayer; ++u)
        {
            Unit myUnit = Units[0, u];
            if (myUnit.X == newX && myUnit.Y == newY)
            {
                return false;
            }

            Unit ennemyUnit = Units[1, u];
            if (ennemyUnit.LastUpdate == 0 && ennemyUnit.X == newX && ennemyUnit.Y == newY) // TODO only if updated or sure estimation
            {
                return false;
            }
        }

        // Check legal move up of 1 or go down
        return newHeight - currentHeight <= 1;
    }

    public List<Action> GenerateLegalActions(int playerId, Unit unit)
    {
        var actions = new List<Action>();

        for (int dir = 0; dir < 8; ++dir)
        {
            // Try MOVE or PUSH all directions
            int newX = unit.X + Board.Offsets[dir, 0];
            int newY = unit.Y + Board.Offsets[dir, 1];

            // Cannot go out of map
            if (IsOutOfMap(newX, newY))
            {
                continue;
            }

            // Cannot go in hole
            if (Cases[newX, newY].IsBlocked)
            {
                continue;
            }

            var newHeight = Cases[newX, newY].Height;
            // Cannot go over 3
            if (newHeight > 3)
            {
                continue;
            }

            // Is there an opponent ? Decides MOVE or PUSH
            bool isOccupiedByEnnemy = false;
            for (int u2 = 0; u2 < Board.UnitsPerPlayer; ++u2)
            {
                // TODO Ignore if ennemy unit is not updated this turn
                if (Units[1 - playerId, u2].LastUpdate != 0)
                {
                    continue;
                }

                if (Units[1 - playerId, u2].X == newX && Units[1 - playerId, u2].Y == newY)
                {
                    isOccupiedByEnnemy = true;
                }
            }

            if (isOccupiedByEnnemy)
            {
                // Try direction where to push (dir2 = dir | dir - 1 | dir + 1)
                for (int dir2 = dir - 1; dir2 < dir + 2; ++dir2)
                {
                    int realDir2 = dir2 < 0 ? dir2 + 8 : (dir2 > 7 ? dir2 - 8 : dir2);
                    // New position has to be legal position
                    int pushX = newX + Board.Offsets[realDir2, 0];
                    int pushY = newY + Board.Offsets[realDir2, 1];

                    // Cannot go out of map
                    if (IsOutOfMap(pushX, pushY))
                    {
                        continue;
                    }

                    // Cannot go in hole
                    if (Cases[pushX, pushY].IsBlocked)
                    {
                        continue;
                    }

                    var pushHeight = Cases[pushX, pushY].Height;
                    // Cannot go over 3
                    if (pushHeight > 3)
                    {
                        continue;
                    }

                    // Can't move of more than 1 up
                    if (pushHeight - newHeight > 1)
                    {
                        continue;
                    }

                    // Has to got no one in there
                    isOccupiedByEnnemy = false;
                    for (int u2 = 0; u2 < Board.UnitsPerPlayer; ++u2)
                    {
                        // TODO Ignore if ennemy unit is not updated this turn
                        if (Units[1 - playerId, u2].LastUpdate != 0)
                        {
                            continue;
                        }

                        if (Units[1 - playerId, u2].X == pushX && Units[1 - playerId, u2].Y == pushY)
                        {
                            isOccupiedByEnnemy = true;
                        }
                    }

                    bool isOccupiedByPlayer = false;
                    for (int u2 = 0; u2 < Board.UnitsPerPlayer; ++u2)
                    {
                        // Ignore if my unit is not updated this turn
                        if (Units[playerId, u2].LastUpdate != 0)
                        {
                            continue;
                        }

                        if (Units[playerId, u2].X == pushX && Units[playerId, u2].Y == pushY)
                        {
                            isOccupiedByPlayer = true;
                        }
                    }

                    if (isOccupiedByEnnemy || isOccupiedByPlayer)
                    {
                        continue;
                    }

                    // This action is legal !
                    actions.Add(new Action
                    {
                        Type = "PUSH&BUILD",
                        Index = unit.Index,
                        Dir1 = dir,
                        Dir2 = realDir2,
                        EnnemyPushedHeightDifference = pushHeight - newHeight
                    });
                }
            }
            else
            {
                // Cannot go there if too high
                int currentHeight = Cases[unit.X, unit.Y].Height;
                if (newHeight - currentHeight > 1)
                {
                    continue;
                }

                // Is it occupied by my units
                bool isOccupiedByPlayer = false;
                for (int u2 = 0; u2 < Board.UnitsPerPlayer; ++u2)
                {
                    // TODO Ignore if my unit is not updated this turn
                    if (Units[playerId, u2].LastUpdate != 0)
                    {
                        continue;
                    }

                    if (Units[playerId, u2].X == newX && Units[playerId, u2].Y == newY)
                    {
                        isOccupiedByPlayer = true;
                    }
                }

                // Cannot move there
                if (isOccupiedByPlayer)
                {
                    continue;
                }

                // Try MOVE&BUILD in all new directions
                for (int dir2 = 0; dir2 < 8; ++dir2)
                {
                    int buildX = newX + Board.Offsets[dir2, 0];
                    int buildY = newY + Board.Offsets[dir2, 1];

                    // Cannot build out of map
                    if (IsOutOfMap(buildX, buildY))
                    {
                        continue;
                    }

                    // Cannot build on a hole
                    if (Cases[buildX, buildY].IsBlocked)
                    {
                        continue;
                    }

                    int buildHeight = Cases[buildX, buildY].Height;
                    // Cannot build over 3
                    if (buildHeight > 3)
                    {
                        continue;
                    }

                    // Is this occupied ?
                    isOccupiedByEnnemy = false;
                    for (int u2 = 0; u2 < Board.UnitsPerPlayer; ++u2)
                    {
                        // TODO Ignore if ennemy unit is not updated this turn
                        if (Units[1 - playerId, u2].LastUpdate != 0)
                        {
                            continue;
                        }

                        if (Units[1 - playerId, u2].X == buildX && Units[1 - playerId, u2].Y == buildY)
                        {
                            isOccupiedByEnnemy = true;
                        }
                    }

                    isOccupiedByPlayer = false;
                    for (int u2 = 0; u2 < Board.UnitsPerPlayer; ++u2)
                    {
                        // Ignore if my unit is not updated this turn
                        if (Units[playerId, u2].LastUpdate != 0)
                        {
                            continue;
                        }

                        if (Units[playerId, u2].X == buildX && Units[playerId, u2].Y == buildY)
                        {
                            isOccupiedByPlayer = true;
                        }
                    }

                    // Not start position and is occupied, not legal
                    if ((buildX != unit.X || buildY != unit.Y) && (isOccupiedByEnnemy || isOccupiedByPlayer))
                    {
                        continue;
                    }

                    // This action is legal !
                    actions.Add(new Action
                    {
                        Type = "MOVE&BUILD",
                        Index = unit.Index,
                        Dir1 = dir,
                        Dir2 = dir2,
                        PointScored = newHeight == 3,
                        HeightDifference = newHeight - currentHeight
                    });
                }
            }
        }

        return actions;
    }

    public int FloodFillPath(Unit unit)
    {
        // Reset
        for (int i = 0; i < Board.Size; ++i)
        {
            for (int j = 0; j < Board.Size; ++j)
            {
                Cases[i,j].Visited = false;
            }
        }

        // Rec floodfill from unit position
        return FloodFill(unit.X, unit.Y);
    }

    private int FloodFill(int x, int y)
    {
        if (!Cases[x, y].Visited)
        {
            Cases[x, y].Visited = true;
            int total = 0;
            for (int dir = 0; dir < 8; ++dir)
            {
                int newX = x + Board.Offsets[dir,0];
                int newY = y + Board.Offsets[dir,1];

                // Cannot go out of map
                if(IsOutOfMap(newX, newY))
                {
                    continue;
                }

                // Cannot go in hole
                if (Cases[newX, newY].IsBlocked)
                {
                    continue;
                }

                int newHeight = Cases[newX, newY].Height;
                // Cannot go over 3
                if (newHeight > 3)
                {
                    continue;
                }

                int currentHeight = Cases[x,y].Height;
                if (newHeight - currentHeight > 1)
                {
                    continue;
                }

                total += FloodFill(newX, newY);
            }
            return total;
        }
        return 0;
    }

    public void DijkstraPath(Unit unit)
    {
        // Reset
        for (int i = 0; i < Board.Size; ++i)
        {
            for (int j = 0; j < Board.Size; ++j)
            {
                Cases[i, j].Distance = int.MaxValue;
            }
        }

        // Rec Dijskstra
        Cases[unit.X, unit.Y].Distance = 0;
        Dijkstra(unit.X, unit.Y);
    }

    private void Dijkstra(int x, int y)
    {
        // Fill neighbors if distance is lower than saved one
        for(int dir = 0; dir < 8; ++dir)
        {
            // Possible move
            if (CanMoveTo(x, y, dir))
            {
                var neighborX = x + Board.Offsets[dir, 0];
                var neighborY = y + Board.Offsets[dir, 1];

                // Closer than previous runs
                if(Cases[neighborX, neighborY].Distance > Cases[x, y].Distance + 1)
                {
                    Cases[neighborX, neighborY].Distance = Cases[x, y].Distance + 1;
                    // Refresh everyone with this new value
                    Dijkstra(neighborX, neighborY);
                }                
            }
        }
    }
}
#endregion

static class Player
{
    static Board CurrentBoard { get; set; }
    static Board PreviousBoard { get; set; }

    static List<Action> LegalActions = new List<Action>();

    static void Main(string[] args)
    {
        #region Init
        string[] inputs;
        Board.Size = int.Parse(Console.ReadLine());
        Board.UnitsPerPlayer = int.Parse(Console.ReadLine());
        #endregion

        // Game loop
        while (true)
        {
            PreviousBoard = CurrentBoard;
            CurrentBoard = new Board();
            LegalActions.Clear();

            #region Reading inputs
            // Feed cases
            for (int i = 0; i < Board.Size; ++i)
            {
                string row = Console.ReadLine();
                for (int j = 0; j < Board.Size; ++j)
                {
                    CurrentBoard.Cases[i, j] = new Case
                    {
                        X = i,
                        Y = j,
                        IsHole = row[j] == '.',
                        Height = row[j] - '0'
                    };
                }
            }
            // Feed my units
            for (int i = 0; i < Board.UnitsPerPlayer; ++i)
            {
                inputs = Console.ReadLine().Split(' ');
                int unitX = int.Parse(inputs[0]);
                int unitY = int.Parse(inputs[1]);
                CurrentBoard.Units[0, i] = new Unit
                {
                    X = unitY,
                    Y = unitX,
                    Index = i,
                    PlayerId = 0,
                    LastUpdate = 0
                };
                Console.Error.WriteLine($"My position : {unitY} {unitX}");
            }
            // Feed ennemy units
            for (int i = 0; i < Board.UnitsPerPlayer; ++i)
            {
                inputs = Console.ReadLine().Split(' ');
                int otherX = int.Parse(inputs[0]);
                int otherY = int.Parse(inputs[1]);
                CurrentBoard.Units[1, i] = new Unit
                {
                    X = otherY >= 0 ? otherY : (PreviousBoard?.Units[1, i].X ?? otherY),
                    Y = otherX >= 0 ? otherX : (PreviousBoard?.Units[1, i].Y ?? otherX),
                    Index = i,
                    PlayerId = 1,
                    LastUpdate = otherX >= 0 ? 0 : (PreviousBoard?.Units[1, i].LastUpdate + 1 ?? 10)
                };
                Console.Error.WriteLine($"Ennemy position : {otherY} {otherX}");
            }
            int legalActions = int.Parse(Console.ReadLine());
            for (int i = 0; i < legalActions; ++i)
            {
                inputs = Console.ReadLine().Split(' ');
                string atype = inputs[0];
                int index = int.Parse(inputs[1]);
                string dir1 = inputs[2];
                string dir2 = inputs[3];
                LegalActions.Add(new Action
                {
                    Type = atype,
                    Index = index,
                    Dir1 = (int)Enum.Parse(typeof(Direction), dir1),
                    Dir2 = (int)Enum.Parse(typeof(Direction), dir2)
                });
            }
            #endregion

            // Find best solution
            Stopwatch sw = new Stopwatch();
            sw.Start();
            var best = FindBestChoice();
            sw.Stop();
            Console.Error.WriteLine($"Choice made in {(1000000L * sw.ElapsedTicks / Stopwatch.Frequency)} us ({sw.ElapsedMilliseconds} ms).");

            Console.WriteLine(best?.ToString() ?? "I lost the game");
        }
    }

    static Action FindBestChoice()
    {
        LegalActions.ForEach(a =>
        {
            Board simulated = Simulate(a);
            a.FinalScore = Evaluate(simulated, a);
        });
        return LegalActions.OrderByDescending(a => a.FinalScore).FirstOrDefault();
    }

    static Board Simulate(Action move)
    {
        var board = CurrentBoard.Copy();
        var playerId = 0;

        // Start Simulating
        var unit = board.Units[playerId, move.Index];

        // MOVE&BUILD :
        if (move.Type == "MOVE&BUILD")
        {
            // MOVE
            int oldHeight = board.Cases[unit.X, unit.Y].Height;
            unit.X = unit.X + Board.Offsets[move.Dir1, 0];
            unit.Y = unit.Y + Board.Offsets[move.Dir1, 1];

            // Scores data
            move.HeightDifference = board.Cases[unit.X, unit.Y].Height - oldHeight;
            move.PointScored = board.Cases[unit.X, unit.Y].Height == 3;

            // BUILD
            var buildX = unit.X + Board.Offsets[move.Dir2, 0];
            var buildY = unit.Y + Board.Offsets[move.Dir2, 1];

            //TODO : check if occupied by a potential ennemy position
            board.Cases[buildX, buildY].Height++;
        }
        else if (move.Type == "PUSH&BUILD")
        {
            // PUSH
            var newX = unit.X + Board.Offsets[move.Dir1, 0];
            var newY = unit.Y + Board.Offsets[move.Dir1, 1];
            int oldEnnemyHeight = board.Cases[newX, newY].Height;

            for (int u = 0; u < Board.UnitsPerPlayer; ++u)
            {
                Unit ennemyUnit = board.Units[1 - playerId, u];
                if (ennemyUnit.X == newX && ennemyUnit.Y == newY)
                {
                    ennemyUnit.X = ennemyUnit.X + Board.Offsets[move.Dir2, 0];
                    ennemyUnit.Y = ennemyUnit.Y + Board.Offsets[move.Dir2, 1];
                    move.EnnemyPushedHeightDifference = board.Cases[ennemyUnit.X, ennemyUnit.Y].Height - oldEnnemyHeight;
                    break;
                }
            }

            // BUILD
            board.Cases[newX, newY].Height++;
        }

        return board;
    }

    static int Evaluate(Board board, Action move)
    {
        // Base score data
        int score = move.PointScored ? 600 : 0                  // Scoring a point
            - move.EnnemyPushedHeightDifference * 500           // Pushing an ennemy away
            + move.HeightDifference * 100                       // Better go up than down
            ;

        // Check neighbors cases
        for (int u = 0; u < Board.UnitsPerPlayer; ++u)
        {
            Unit unit = board.Units[0, u];
            int unitEscapes = 0;
            for (int dir = 0; dir < 8; ++dir)
            {
                bool isValid = board.CanMoveTo(unit.X, unit.Y, dir);
                if (isValid)
                {
                    ++unitEscapes;
                }

                var newX = unit.X + Board.Offsets[dir, 0];
                var newY = unit.Y + Board.Offsets[dir, 1];
                if (!board.IsOutOfMap(newX, newY) && board.Cases[newX, newY].Height == 4)
                {
                    score -= 10;    // Not good if too much 4 around                   
                }
            }

            if (unitEscapes < 3)
            {
                score -= (1000 - unitEscapes * 20);    //It is wrong if one unit is going to be trapped
            }
        }

        // Check positionning
        for (int u = 0; u < Board.UnitsPerPlayer; ++u)
        {
            Unit unit = board.Units[0, u];
            int distance2 = Convert.ToInt32(Math.Round(((Board.Size / 2.0) - unit.X) * ((Board.Size / 2.0) - unit.X) + ((Board.Size / 2.0) - unit.Y) * ((Board.Size / 2.0) - unit.Y)));

            score -= distance2 * 10;     // Better stay around the middle

            //Areas:
            board.DijkstraPath(unit);
            int reachable = 0;
            int sumOfMinDistance = 0;
            for(int i = 0; i < Board.Size; ++i)
            {
                for(int j = 0; j < Board.Size; ++j)
                {
                    if(board.Cases[i,j].Distance < int.MaxValue)
                    {
                        ++reachable;
                        sumOfMinDistance += board.Cases[i, j].Distance;
                    }
                }
            }
            score -= sumOfMinDistance; // around 100 in early game
            score += reachable * 20;
        }

        // Check what can ennemy do next
        for (int u = 0; u < Board.UnitsPerPlayer; ++u)
        {
            Unit unit = board.Units[1, u];
            if (unit.LastUpdate == 0) // TODO : or well estimated
            {
                List<Action> ennemyActions = board.GenerateLegalActions(1, unit);
                score += ennemyActions.Where(a => a.Type == "PUSH&BUILD" && a.EnnemyPushedHeightDifference < 0).Sum(a => a.EnnemyPushedHeightDifference) * 500; // Not good if we let ennemy kicking us

                var unitMoves = ennemyActions.Count;
                if (unitMoves < 6)
                {
                    score += (600 - unitMoves * 100);    //It is cool if ennemy can't do much, means almost trapped
                }
            }
        }

        // Check what i could do next
        for (int u = 0; u < Board.UnitsPerPlayer; ++u)
        {
            Unit unit = board.Units[0, u];
            List<Action> myActions = board.GenerateLegalActions(0, unit);
            score += myActions.Count(a => a.Type == "MOVE&BUILD" && a.HeightDifference > 0) * 10 //Better if we can go up after
                    + myActions.Count(a => a.Type == "MOVE&BUILD" && a.PointScored) * 20 //Better if we can score
                    ;
        }

        return score;
    }
}