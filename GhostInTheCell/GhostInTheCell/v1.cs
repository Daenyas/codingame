//using System;
//using System.Collections.Generic;
//using System.Diagnostics;
//using System.Linq;

//namespace v1
//{
//    /**
//     * Auto-generated code below aims at helping you parse
//     * the standard input according to the problem statement.
//     **/
//    public static class Constants
//    {
//        public const int MonteCarloDepth = 8;
//        public const int MonteCarloTime = 45;
//    }

//    public class Player
//    {

//        public static void Main(string[] args)
//        {
//            bool fakeInputs = true;
//            if (fakeInputs)
//            {
//                BoardStaticData.Distances.Add(new Tuple<int, int>(1, 2), 3);
//                BoardStaticData.Distances.Add(new Tuple<int, int>(2, 1), 3);

//                BoardStaticData.Distances.Add(new Tuple<int, int>(1, 3), 5);
//                BoardStaticData.Distances.Add(new Tuple<int, int>(3, 1), 5);

//                BoardStaticData.Distances.Add(new Tuple<int, int>(2, 3), 3);
//                BoardStaticData.Distances.Add(new Tuple<int, int>(3, 2), 3);
//            }
//            else
//            {
//                string[] inputs;
//                int factoryCount = int.Parse(Console.ReadLine()); // the number of factories
//                int linkCount = int.Parse(Console.ReadLine()); // the number of links between factories
//                for (int i = 0; i < linkCount; ++i)
//                {
//                    inputs = Console.ReadLine().Split(' ');
//                    int factory1 = int.Parse(inputs[0]);
//                    int factory2 = int.Parse(inputs[1]);
//                    int distance = int.Parse(inputs[2]);
//                    BoardStaticData.Distances.Add(new Tuple<int, int>(factory1, factory2), distance);
//                    BoardStaticData.Distances.Add(new Tuple<int, int>(factory2, factory1), distance);
//                }
//            }


//            var strategy = new MonteCarlo();

//            // game loop
//            while (true)
//            {
//                var board = new Board();

//                if (fakeInputs)
//                {
//                    Factory factory = new Factory()
//                    {
//                        Id = 1,
//                        Owner = 1,
//                        CyborgsNumber = 15,
//                        Production = 2
//                    };
//                    board.Factories.Add(factory);
//                    factory = new Factory()
//                    {
//                        Id = 2,
//                        Owner = -1,
//                        CyborgsNumber = 15,
//                        Production = 2
//                    };
//                    board.Factories.Add(factory);
//                    factory = new Factory()
//                    {
//                        Id = 3,
//                        Owner = 0,
//                        CyborgsNumber = 7,
//                        Production = 3
//                    };
//                    board.Factories.Add(factory);
//                }
//                else
//                {
//                    int entityCount = int.Parse(Console.ReadLine()); // the number of entities (e.g. factories and troops)
//                    for (int i = 0; i < entityCount; ++i)
//                    {
//                        string[] inputs = Console.ReadLine().Split(' ');
//                        int entityId = int.Parse(inputs[0]);
//                        string entityType = inputs[1];
//                        int arg1 = int.Parse(inputs[2]);
//                        int arg2 = int.Parse(inputs[3]);
//                        int arg3 = int.Parse(inputs[4]);
//                        int arg4 = int.Parse(inputs[5]);
//                        int arg5 = int.Parse(inputs[6]);

//                        if (entityType == "FACTORY")
//                        {
//                            Factory factory = new Factory()
//                            {
//                                Id = entityId,
//                                Owner = arg1,
//                                CyborgsNumber = arg2,
//                                Production = arg3
//                            };
//                            board.Factories.Add(factory);
//                        }
//                        else //TROOP
//                        {
//                            Troop troop = new Troop()
//                            {
//                                Id = entityId,
//                                Owner = arg1,
//                                Source = board.Factories.First(f => f.Id == arg2),
//                                Destination = board.Factories.First(f => f.Id == arg3),
//                                CyborgsNumber = arg4,
//                                TurnsBeforeArrival = arg5
//                            };
//                            board.Troops.Add(troop);
//                        }
//                    }
//                }

//                // Write an action using Console.WriteLine()
//                // To debug: Console.Error.WriteLine("Debug messages...");

//                // Any valid action, such as "WAIT" or "MOVE source destination cyborgs"
//                //Console.WriteLine("WAIT");
//                var action = strategy.GetBestActions(board);
//                action[0].Print();

//                ++board.GameTurn;

//                if (fakeInputs)
//                    break;
//            }
//        }
//    }

//    #region Strategy
//    public interface IStrategy
//    {
//        Action[] GetBestActions(Board board);
//    }

//    public class MonteCarlo : IStrategy
//    {
//        public Action[] GetBestActions(Board board)
//        {
//            // TODO : Should MultiThread that !

//            Action[] bestActions = new Action[Constants.MonteCarloDepth];
//            int bestScore = int.MinValue;

//            var random = new Random();
//            Action[] currentActions = new Action[Constants.MonteCarloDepth];

//            // Create new stopwatch.
//            Stopwatch stopwatch = new Stopwatch();
//            // Begin timing.
//            stopwatch.Start();

//            int count = 0;

//            while (stopwatch.ElapsedMilliseconds < Constants.MonteCarloTime)
//            {
//                ++count;
//                // Copy board
//                Board simulationBoard = board.Copy();
//                int score = 0;

//                // Evaluate
//                for (int i = 0; i < Constants.MonteCarloDepth; ++i)
//                {
//                    // Generate random action
//                    currentActions[i] = this.Generate(random, simulationBoard);

//                    // Run action
//                    score += simulationBoard.RunTurn(currentActions[i]);
//                }

//                // Score = cyborgs difference + Cyborgs production
//                // Malus if lost etc ...

//                // Replace if better
//                if (score > bestScore)
//                    bestActions = currentActions;
//            }

//            stopwatch.Stop();

//            Console.Error.WriteLine($"{count} simulations.");

//            return bestActions;
//        }

//        private Action Generate(Random random, Board board)
//        {
//            if (random.Next(2) == 0)
//            {
//                return new WaitAction();
//            }
//            else
//            {
//                // TODO : care if simulation where i have no more factories.
//                var action = new MoveAction();
//                action.Source = board.MyFactories.ElementAt(random.Next(board.MyFactories.Count));
//                action.Destination = action.Source;
//                while (action.Destination.Id == action.Source.Id)
//                {
//                    action.Destination = board.Factories.ElementAt(random.Next(board.Factories.Count));
//                };
//                action.CyborgsNumber = random.Next(action.Source.CyborgsNumber) + 1;

//                return action;
//            }
//        }
//    }
//    #endregion

//    #region Board
//    public static class BoardStaticData
//    {
//        // Source, Destination => Distance
//        public static Dictionary<Tuple<int, int>, int> Distances = new Dictionary<Tuple<int, int>, int>();
//    }

//    public class Board
//    {
//        public List<Factory> Factories { get; set; }
//        public List<Troop> Troops { get; set; }
//        public int GameTurn { get; set; }

//        // Cached
//        public List<Factory> MyFactories { get; set; }

//        public Board()
//        {
//            this.Factories = new List<Factory>();
//            this.Troops = new List<Troop>();
//        }

//        public Board Copy()
//        {
//            Board board = new Board()
//            {
//                GameTurn = this.GameTurn
//            };

//            this.Factories.ForEach(f =>
//            {
//                board.Factories.Add(f.Copy());
//            });

//            this.Troops.ForEach(t =>
//            {
//                board.Troops.Add(t.Copy(board.Factories));
//            });

//            board.MyFactories = board.Factories.Where(f => f.Owner == 1).ToList();

//            return board;
//        }

//        public int RunTurn(Action action)
//        {
//            // Avancée des troupes existantes
//            this.Troops.ForEach(t =>
//            {
//                --t.TurnsBeforeArrival;
//            });

//            // Exécution des ordres de chaque joueur
//            action.Execute(this);

//            // Production de nouveaux cyborgs dans chaque usine
//            this.Factories.ForEach(f =>
//            {
//                f.CyborgsNumber += f.Production;
//            });

//            // Résolution des combats
//            this.Factories.ForEach(f =>
//            {
//                var active = this.Troops.Where(t => t.Destination.Id == f.Id && t.TurnsBeforeArrival == 0).ToList();
//                int winnerId = 0;
//                int winnerCount = 0;
//                active.ForEach(t =>
//                {
//                    if (winnerId != t.Owner)
//                    {
//                        // Fight
//                        if (t.CyborgsNumber > winnerCount)
//                        {
//                            // Coming win
//                            winnerId = t.Owner;
//                            winnerCount = t.CyborgsNumber - winnerCount;
//                        }
//                        else if (t.CyborgsNumber == winnerCount)
//                        {
//                            // No one win
//                            winnerCount = 0;
//                            winnerId = 0;
//                        }
//                        else
//                        {
//                            // Coming lose
//                            winnerCount = winnerCount - t.CyborgsNumber;
//                        }
//                    }
//                    else
//                    {
//                        // Add
//                        winnerCount += t.CyborgsNumber;
//                    }
//                });

//                // TODO : Factorize fight system in upper class of Factory and Troop
//                // Fight the base
//                if (f.Owner != winnerId)
//                {
//                    // Fight
//                    if (f.CyborgsNumber > winnerCount)
//                    {
//                        // Factory win
//                        f.CyborgsNumber = f.CyborgsNumber - winnerCount;
//                    }
//                    else if (f.CyborgsNumber == winnerCount)
//                    {
//                        // No one win
//                        winnerCount = 0;
//                        winnerId = 0;
//                    }
//                    else
//                    {
//                        // Factory lose
//                        f.Owner = winnerId;
//                        f.CyborgsNumber = winnerCount - f.CyborgsNumber;
//                    }
//                }
//                else
//                {
//                    // Add
//                    f.CyborgsNumber += winnerCount;
//                }

//                // Remove troops
//                active.ForEach(t =>
//                {
//                    this.Troops.Remove(t);
//                });
//            });

//            // Vérification des conditions de fin
//            ++this.GameTurn;

//            return 0;
//        }
//    }

//    public class Factory
//    {
//        public int Id { get; set; }
//        public int Owner { get; set; }
//        public int CyborgsNumber { get; set; }
//        public int Production { get; set; }
//        // Neighbors for graph ?

//        public Factory Copy()
//        {
//            return new Factory()
//            {
//                Id = this.Id,
//                Owner = this.Owner,
//                CyborgsNumber = this.CyborgsNumber,
//                Production = this.Production
//            };
//        }
//    }

//    public class Troop
//    {
//        public int Id { get; set; }
//        public int Owner { get; set; } // TODO : Factorize
//        public Factory Source { get; set; }
//        public Factory Destination { get; set; }
//        public int CyborgsNumber { get; set; }
//        public int TurnsBeforeArrival { get; set; }

//        public Troop Copy(List<Factory> newFactories)
//        {
//            return new Troop()
//            {
//                Owner = this.Owner,
//                CyborgsNumber = this.CyborgsNumber,
//                TurnsBeforeArrival = this.TurnsBeforeArrival,
//                Source = newFactories.First(f => f.Id == this.Source.Id),
//                Destination = newFactories.First(f => f.Id == this.Destination.Id)
//            };
//        }
//    }
//    #endregion

//    #region Actions
//    public abstract class Action
//    {
//        public void Print()
//        {
//            Console.WriteLine(this.GetActionString());
//        }

//        public abstract string GetActionString();
//        public abstract void Execute(Board board);
//    }

//    public class MoveAction : Action
//    {
//        public Factory Source { get; set; }
//        public Factory Destination { get; set; }
//        public int CyborgsNumber { get; set; }

//        public override string GetActionString()
//        {
//            return $"MOVE {this.Source.Id} {this.Destination.Id} {this.CyborgsNumber}";
//        }

//        public override void Execute(Board board)
//        {
//            Troop troop = new Troop()
//            {
//                Owner = 1,
//                Source = this.Source,
//                Destination = this.Destination,
//                CyborgsNumber = this.CyborgsNumber,
//                TurnsBeforeArrival = BoardStaticData.Distances[new Tuple<int, int>(this.Source.Id, this.Destination.Id)]
//            };
//            board.Troops.Add(troop);
//        }
//    }

//    public class WaitAction : Action
//    {
//        public override void Execute(Board board)
//        {
//            // Nothing to do
//        }

//        public override string GetActionString()
//        {
//            return "WAIT";
//        }
//    }
//    #endregion
//}