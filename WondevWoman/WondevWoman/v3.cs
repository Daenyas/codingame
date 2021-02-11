//// Top 2 Silver :@

//using System;
//using System.Linq;
//using System.IO;
//using System.Text;
//using System.Collections;
//using System.Collections.Generic;
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

//public class Action
//{
//    public string Type { get; set; }
//    public int Index { get; set; }
//    public int Dir1 { get; set; }
//    public int Dir2 { get; set; }

//    //Scoring
//    public bool PointScored { get; set; }
//    public int HeightDifference { get; set; }
//    public int EnnemyPushedHeightDifference { get; set; }

//    public int FinalScore { get; set; }

//    public override string ToString()
//    {
//        return $"{Type} {Index} {((Direction)Dir1).ToString()} {((Direction)Dir2).ToString()} {FinalScore}";
//    }
//}

//public class Unit
//{
//    public int X { get; set; }
//    public int Y { get; set; }
//    public int Index { get; set; }
//    public int PlayerId { get; set; }

//    public int LastUpdate { get; set; }

//    public Unit Copy()
//    {
//        Unit unit = new Unit();
//        unit.X = X;
//        unit.Y = Y;
//        unit.Index = Index;
//        unit.PlayerId = PlayerId;
//        unit.LastUpdate = LastUpdate;
//        return unit;
//    }
//}

//public class Case
//{
//    public int X { get; set; }
//    public int Y { get; set; }
//    public bool IsHole { get; set; }
//    public int Height { get; set; }

//    public bool IsBlocked => Height > 3 || IsHole;

//    public Case Copy()
//    {
//        Case other = new Case();
//        other.X = X;
//        other.Y = Y;
//        other.IsHole = IsHole;
//        other.Height = Height;
//        return other;
//    }
//}

//public class Board
//{
//    public static readonly int[,] Offsets = new int[8, 2] { { -1, 0 }, { -1, 1 }, { 0, 1 }, { 1, 1 }, { 1, 0 }, { 1, -1 }, { 0, -1 }, { -1, -1 } };

//    public static int Size { get; set; }
//    public static int UnitsPerPlayer { get; set; }

//    public Case[,] Cases { get; set; }
//    public Unit[,] Units { get; set; } // 0 mine / 1 ennemy

//    public Board()
//    {
//        Cases = new Case[Size, Size];
//        Units = new Unit[2, UnitsPerPlayer];
//    }

//    public Board Copy()
//    {
//        Board other = new Board();
//        for (int i = 0; i < Size; ++i)
//        {
//            for (int j = 0; j < Size; ++j)
//            {
//                other.Cases[i, j] = Cases[i, j].Copy();
//            }
//        }
//        for (int i = 0; i < 2; ++i)
//        {
//            for (int j = 0; j < UnitsPerPlayer; ++j)
//            {
//                other.Units[i, j] = Units[i, j].Copy();
//            }
//        }
//        return other;
//    }

//    public bool IsOutOfMap(int x, int y)
//    {
//        // Check if out of map
//        if (x < 0 || x >= Board.Size || y < 0 || y >= Board.Size)
//        {
//            return true;
//        }

//        return false;
//    }

//    public bool CanMoveTo(int x, int y, int dir)
//    {
//        var currentHeight = Cases[x, y].Height;
//        var newX = x + Board.Offsets[dir, 0];
//        var newY = y + Board.Offsets[dir, 1];

//        // Check if out of map
//        if (IsOutOfMap(newX, newY))
//        {
//            return false;
//        }

//        // Check if is a hole
//        if (Cases[newX, newY].IsBlocked)
//        {
//            return false;
//        }

//        var newHeight = Cases[newX, newY].Height;

//        // Can't go to the sky
//        if (newHeight > 3)
//        {
//            return false;
//        }

//        // Can't go on me or ennemy
//        for (int u = 0; u < Board.UnitsPerPlayer; ++u)
//        {
//            Unit myUnit = Units[0, u];
//            if (myUnit.X == newX && myUnit.Y == newY)
//            {
//                return false;
//            }

//            Unit ennemyUnit = Units[1, u];
//            if (ennemyUnit.LastUpdate == 0 && ennemyUnit.X == newX && ennemyUnit.Y == newY) // TODO only if updated or sure estimation
//            {
//                return false;
//            }
//        }

//        // Check legal move up of 1 or go down
//        return newHeight - currentHeight <= 1;
//    }
//}
//#endregion

//static class Player
//{
//    static Board CurrentBoard { get; set; }
//    static Board PreviousBoard { get; set; }

//    static List<Action> LegalActions = new List<Action>();

//    static void Main(string[] args)
//    {
//        #region Init
//        string[] inputs;
//        Board.Size = int.Parse(Console.ReadLine());
//        Board.UnitsPerPlayer = int.Parse(Console.ReadLine());
//        #endregion

//        // Game loop
//        while (true)
//        {
//            PreviousBoard = CurrentBoard;
//            CurrentBoard = new Board();
//            LegalActions.Clear();

//            #region Reading inputs
//            // Feed cases
//            for (int i = 0; i < Board.Size; ++i)
//            {
//                string row = Console.ReadLine();
//                for (int j = 0; j < Board.Size; ++j)
//                {
//                    CurrentBoard.Cases[i, j] = new Case
//                    {
//                        X = i,
//                        Y = j,
//                        IsHole = row[j] == '.',
//                        Height = row[j] - '0'
//                    };
//                }
//            }
//            // Feed my units
//            for (int i = 0; i < Board.UnitsPerPlayer; ++i)
//            {
//                inputs = Console.ReadLine().Split(' ');
//                int unitX = int.Parse(inputs[0]);
//                int unitY = int.Parse(inputs[1]);
//                CurrentBoard.Units[0, i] = new Unit
//                {
//                    X = unitY,
//                    Y = unitX,
//                    Index = i,
//                    PlayerId = 0,
//                    LastUpdate = 0
//                };
//                Console.Error.WriteLine($"My position : {unitY} {unitX}");
//            }
//            // Feed ennemy units
//            for (int i = 0; i < Board.UnitsPerPlayer; ++i)
//            {
//                inputs = Console.ReadLine().Split(' ');
//                int otherX = int.Parse(inputs[0]);
//                int otherY = int.Parse(inputs[1]);
//                CurrentBoard.Units[1, i] = new Unit
//                {
//                    X = otherY >= 0 ? otherY : (PreviousBoard?.Units[1, i].X ?? otherY),
//                    Y = otherX >= 0 ? otherX : (PreviousBoard?.Units[1, i].Y ?? otherX),
//                    Index = i,
//                    PlayerId = 1,
//                    LastUpdate = otherX >= 0 ? 0 : (PreviousBoard?.Units[1, i].LastUpdate + 1 ?? 10)
//                };
//                Console.Error.WriteLine($"Ennemy position : {otherY} {otherX}");
//            }
//            int legalActions = int.Parse(Console.ReadLine());
//            for (int i = 0; i < legalActions; ++i)
//            {
//                inputs = Console.ReadLine().Split(' ');
//                string atype = inputs[0];
//                int index = int.Parse(inputs[1]);
//                string dir1 = inputs[2];
//                string dir2 = inputs[3];
//                LegalActions.Add(new Action
//                {
//                    Type = atype,
//                    Index = index,
//                    Dir1 = (int)Enum.Parse(typeof(Direction), dir1),
//                    Dir2 = (int)Enum.Parse(typeof(Direction), dir2)
//                });
//            }
//            #endregion

//            // Find best solution
//            Stopwatch sw = new Stopwatch();
//            sw.Start();
//            var best = FindBestChoice();
//            sw.Stop();
//            Console.Error.WriteLine($"Choice made in {(1000000L * sw.ElapsedTicks / Stopwatch.Frequency)} us ({sw.ElapsedMilliseconds} ms).");

//            Console.WriteLine(best?.ToString() ?? "I lost the game");
//        }
//    }

//    static Action FindBestChoice()
//    {
//        LegalActions.ForEach(a =>
//        {
//            Board simulated = Simulate(a);
//            a.FinalScore = Evaluate(simulated, a);
//        });
//        return LegalActions.OrderByDescending(a => a.FinalScore).FirstOrDefault();
//    }

//    static Board Simulate(Action move)
//    {
//        var board = CurrentBoard.Copy();
//        var playerId = 0;

//        // Start Simulating
//        var unit = board.Units[playerId, move.Index];

//        // MOVE&BUILD :
//        if (move.Type == "MOVE&BUILD")
//        {
//            // MOVE
//            int oldHeight = board.Cases[unit.X, unit.Y].Height;
//            unit.X = unit.X + Board.Offsets[move.Dir1, 0];
//            unit.Y = unit.Y + Board.Offsets[move.Dir1, 1];

//            // Scores data
//            move.HeightDifference = board.Cases[unit.X, unit.Y].Height - oldHeight;
//            move.PointScored = board.Cases[unit.X, unit.Y].Height == 3;

//            // BUILD
//            var buildX = unit.X + Board.Offsets[move.Dir2, 0];
//            var buildY = unit.Y + Board.Offsets[move.Dir2, 1];

//            //TODO : check if occupied by a potential ennemy position
//            board.Cases[buildX, buildY].Height++;
//        }
//        else if (move.Type == "PUSH&BUILD")
//        {
//            // PUSH
//            var newX = unit.X + Board.Offsets[move.Dir1, 0];
//            var newY = unit.Y + Board.Offsets[move.Dir1, 1];
//            int oldEnnemyHeight = board.Cases[newX, newY].Height;

//            for (int u = 0; u < Board.UnitsPerPlayer; ++u)
//            {
//                Unit ennemyUnit = board.Units[1 - playerId, u];
//                if (ennemyUnit.X == newX && ennemyUnit.Y == newY)
//                {
//                    ennemyUnit.X = ennemyUnit.X + Board.Offsets[move.Dir2, 0];
//                    ennemyUnit.Y = ennemyUnit.Y + Board.Offsets[move.Dir2, 1];
//                    move.EnnemyPushedHeightDifference = board.Cases[ennemyUnit.X, ennemyUnit.Y].Height - oldEnnemyHeight;
//                    break;
//                }
//            }

//            // BUILD
//            board.Cases[newX, newY].Height++;
//        }

//        return board;
//    }

//    static int Evaluate(Board board, Action move)
//    {
//        // Base score data
//        int score = move.PointScored ? 200 : 0                  // Scoring a point
//            - move.EnnemyPushedHeightDifference * 200           // Pushing an ennemy away
//            + move.HeightDifference * 100                       // Better go up than down
//            ;

//        // Check neighbors
//        for (int u = 0; u < Board.UnitsPerPlayer; ++u)
//        {
//            Unit unit = board.Units[0, u];
//            int unitEscapes = 0;
//            for (int dir = 0; dir < 8; ++dir)
//            {
//                bool isValid = board.CanMoveTo(unit.X, unit.Y, dir);
//                if (isValid)
//                {
//                    ++unitEscapes;
//                }

//                var newX = unit.X + Board.Offsets[dir, 0];
//                var newY = unit.Y + Board.Offsets[dir, 1];
//                if (!board.IsOutOfMap(newX, newY) && board.Cases[newX, newY].Height == 4)
//                {  
//                   score -= 10;    // Not good if too much 4 around                   
//                }
//            }

//            if (unitEscapes < 3)
//            {
//                score -= (400 - unitEscapes * 10);    //It is wrong if one unit is going to be trapped
//            }
//        }

//        for (int u = 0; u < Board.UnitsPerPlayer; ++u)
//        {
//            Unit unit = board.Units[0, u];
//            int distance2 = Convert.ToInt32(Math.Round(((Board.Size / 2.0) - unit.X) * ((Board.Size / 2.0) - unit.X) + ((Board.Size / 2.0) - unit.Y) * ((Board.Size / 2.0) - unit.Y)));

//            score -= distance2 * 10;     // Better stay around the middle
//        }

//        return score;
//    }
//}