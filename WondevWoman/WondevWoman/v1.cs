//using System;
//using System.Linq;
//using System.IO;
//using System.Text;
//using System.Collections;
//using System.Collections.Generic;

///**
// * Auto-generated code below aims at helping you parse
// * the standard input according to the problem statement.
// **/

//namespace v1
//{
//    enum Direction
//    {
//        N,
//        NE,
//        E,
//        SE,
//        NW,
//        W,
//        SW,
//        S
//    }

//    class Unit
//    {
//        public int X { get; set; }
//        public int Y { get; set; }
//    }

//    class Board
//    {
//        public static int[,] Offsets = new int[8, 2] { { -1, 0 }, { -1, 1 }, { 0, 1 }, { 1, 1 }, { -1, -1 }, { 0, -1 }, { 1, -1 }, { 1, 0 } };
//        public char[,] Cases { get; set; }

//        public Board(int size)
//        {
//            Cases = new char[size, size];
//        }
//    }

//    class Action
//    {
//        public string Type { get; set; }
//        public int Index { get; set; }
//        public Direction Dir1 { get; set; }
//        public Direction Dir2 { get; set; }
//        public int Score { get; set; }

//        public Action(string type, int index, string dir1, string dir2)
//        {
//            Type = type;
//            Index = index;
//            Dir1 = (Direction)Enum.Parse(typeof(Direction), dir1);
//            Dir2 = (Direction)Enum.Parse(typeof(Direction), dir2);
//            Score = int.MinValue;
//        }

//        public override string ToString()
//        {
//            return $"{Type} {Index} {Dir1.ToString()} {Dir2.ToString()} {Score}";
//        }
//    }

//    class Player
//    {
//        static void Main(string[] args)
//        {
//            string[] inputs;
//            int size = int.Parse(Console.ReadLine());
//            int unitsPerPlayer = int.Parse(Console.ReadLine());

//            var board = new Board(size);

//            // game loop
//            while (true)
//            {
//                var myUnits = new List<Unit>();
//                var actions = new List<Action>();

//                for (int i = 0; i < size; i++)
//                {
//                    string row = Console.ReadLine();
//                    for (int j = 0; j < size; ++j)
//                    {
//                        board.Cases[i, j] = row[j];
//                    }
//                }
//                for (int i = 0; i < unitsPerPlayer; i++)
//                {
//                    inputs = Console.ReadLine().Split(' ');
//                    int unitX = int.Parse(inputs[0]);
//                    int unitY = int.Parse(inputs[1]);
//                    myUnits.Add(new Unit { X = unitY, Y = unitX });
//                    Console.Error.WriteLine($"My position : {unitY} {unitX}");
//                }
//                for (int i = 0; i < unitsPerPlayer; i++)
//                {
//                    inputs = Console.ReadLine().Split(' ');
//                    int otherX = int.Parse(inputs[0]);
//                    int otherY = int.Parse(inputs[1]);
//                }
//                int legalActions = int.Parse(Console.ReadLine());
//                for (int i = 0; i < legalActions; i++)
//                {
//                    inputs = Console.ReadLine().Split(' ');
//                    string atype = inputs[0];
//                    int index = int.Parse(inputs[1]);
//                    string dir1 = inputs[2];
//                    string dir2 = inputs[3];
//                    actions.Add(new Action(atype, index, dir1, dir2));
//                }

//                Action best = null;

//                for (int i = 0; i < unitsPerPlayer; ++i)
//                {
//                    var unit = myUnits[i];
//                    var unitActions = actions.Where(a => a.Index == i).ToList();
//                    if (unitActions.Count == 0)
//                    {
//                        continue;
//                    }

//                    foreach (var act in unitActions)
//                    {
//                        if (act.Type == "MOVE&BUILD")
//                        {
//                            // Move
//                            var moveX = unit.X + Board.Offsets[(int)act.Dir1, 0];
//                            var moveY = unit.Y + Board.Offsets[(int)act.Dir1, 1];

//                            if (moveX < 0 || moveX >= size || moveY < 0 || moveY >= size)
//                            {
//                                continue;
//                            }

//                            var moveValue = board.Cases[moveX, moveY];
//                            if (moveValue == '.')
//                            {
//                                continue;
//                            }
//                            int moveHeight = int.Parse(moveValue.ToString());

//                            var currentValue = board.Cases[unit.X, unit.Y];
//                            int currentHeight = int.Parse(currentValue.ToString());

//                            // Build
//                            var buildX = moveX + Board.Offsets[(int)act.Dir2, 0];
//                            var buildY = moveY + Board.Offsets[(int)act.Dir2, 1];

//                            if (buildX < 0 || buildX >= size || buildY < 0 || buildY >= size)
//                            {
//                                continue;
//                            }

//                            var buildValue = board.Cases[buildX, buildY];
//                            if (buildValue == '.')
//                            {
//                                continue;
//                            }
//                            int buildHeight = int.Parse(buildValue.ToString());

//                            act.Score = (moveHeight - currentHeight)            // Better go higher
//                                + (moveHeight == 3 ? 1000 : 0)                  // Better have points
//                                + (buildHeight == 3 ? -300 : 0);                 // Don't prevent scoring free points

//                            Console.Error.WriteLine($"{act.ToString()} / Move To : {moveX} {moveY} / Build On : {buildX} {buildY} ");
//                        }
//                        else if (act.Type == "PUSH&BUILD")
//                        {

//                        }
//                    }

//                    unitActions = unitActions.OrderByDescending(a => a.Score).ToList();

//                    var unitBest = unitActions.First();
//                    if (best == null)
//                    {
//                        best = unitBest;
//                    }
//                    else if (unitBest.Score > best.Score)
//                    {
//                        best = unitBest;
//                    }
//                }
//                Console.WriteLine(best?.ToString());
//            }
//        }
//    }
//}