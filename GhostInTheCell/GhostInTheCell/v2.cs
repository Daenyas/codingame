//using System;
//using System.Collections.Generic;
//using System.Linq;
//using System.IO;

//namespace v2
//{
//    public class Player
//    {
//        public static void Main(string[] args)
//        {
//            // For Debug
//            bool readFromFile = false;

//            BoardStaticData.Load(readFromFile);

//            var manager = new StrategyManager();
                      
//            // game loop
//            while (true)
//            {
//                var board = new Board();

//                board.LoadTurn(readFromFile);

//                if (manager.GameTurn == 0)
//                    BoardStaticData.Init(board); // Init my start position etc...

//                List<IAction> actions = manager.ComputeActions(board);

//                Console.WriteLine(string.Join(" ; ", actions.Select(a => a.GetActionString()).ToArray()));

//                ++manager.GameTurn;
//            }
//        }
//    }

//    #region Strategy
//    public class StrategyManager
//    {
//        public int GameTurn { get; set; }

//        public List<IAction> ComputeActions(Board board)
//        {
//            List<IAction> todoList = new List<IAction>();
//            if (this.GameTurn < 6)
//            {
//                todoList.AddRange(this.TakeMySideFactories(board));
//            }
//            else
//            {
//                todoList.AddRange(this.StandardStrategy(board));
//            }

//            //todoList.AddRange(this.AggressiveStrategy(board));

//            if (todoList.Count == 0)
//            {
//                todoList.Add(new WaitAction());
//            }           

//            return todoList;
//        }

//        private List<IAction> AggressiveStrategy(Board board)
//        {
//            List<IAction> todoList = new List<IAction>();

//            // First 5 rounds, look for 2 best out of 
//            // TODO : 4 closest that have production and get them
//            if(this.GameTurn < 5)
//            {
//                List<Factory> mySideFactories = new List<Factory>();
//                BoardStaticData.MySideIds.ForEach(id =>
//                {
//                    Factory factory = board.GetFactory(id);
//                    mySideFactories.Add(factory);
//                });

//                var ordered = mySideFactories.Where(f => f.Id != BoardStaticData.MyStartId).OrderByDescending(f => f.Production).ToList();
//                var targets = new List<Factory>() { ordered[0], ordered[1] };

//                targets.ForEach(f =>
//                {
//                    // Check for negative
//                    var dangers = new List<Tuple<int, int>>(); // turn, number
//                    var availability = f.CountAvailableCyborgs(board);
//                    int needed = -availability.Min() + 1;
//                    this.SendClosestCyborgs(board, todoList, f, needed, 1000);                       
//                });
//            }
//            // Turn 6 : attack 2 best ennemies productions with bomb
//            else if(this.GameTurn == 6)
//            {
//                var ennemies = board.Factories.Where(f => f.Owner == -1).OrderByDescending(f => f.Production).ToList();
//                var targets = new List<Factory>() { ennemies[0], ennemies[1] };

//                List<Factory> sources = new List<Factory>();
//                targets.ForEach(f =>
//                {
//                    var source = board.MyFactories.OrderBy(mf => BoardStaticData.GetDistance(mf.Id, f.Id)).First(mf => !sources.Contains(mf));
//                    var launchBomb = new BombAction()
//                    {
//                        Destination = f,
//                        Source = source
//                    };
//                    sources.Add(source);
//                    todoList.Add(launchBomb);
//                });
//            }
//            // Turn 7 : send troups there
//            else if(this.GameTurn == 7)
//            {
//                var ennemies = board.Factories.Where(f => f.Owner == -1).OrderByDescending(f => f.Production).ToList();
//                var targets = new List<Factory>() { ennemies[0], ennemies[1] };

//                targets.ForEach(f => 
//                {
//                    var availability = f.CountAvailableCyborgs(board);
//                    int needed = -availability.Min();
//                    this.SendClosestCyborgs(board, todoList, f, needed, 1000);
//                });
//            }
//            // Then Attack closest ennemy / colonize most valuable around / upgrade
//            else
//            {
//                // Best ennemy
//                var ennemy = board.Factories.Where(f => f.Owner == -1).OrderByDescending(f => f.Production).First();
//                var availability = ennemy.CountAvailableCyborgs(board);
//                int needed = -availability.Min();
//                this.SendClosestCyborgs(board, todoList, ennemy, needed, 1000);

//                // Closest to colonize
//                var colonies = board.Factories.Where(f => f.Owner == 0).OrderByDescending(f => f.Production).ToList();

//                colonies.ForEach(f =>
//                {
//                    var availability2 = f.CountAvailableCyborgs(board);
//                    int needed2 = -availability2.Min();
//                    this.SendClosestCyborgs(board, todoList, f, needed2, 1000);
//                });
//            }

//            return todoList;
//        }

//        private List<IAction> StandardStrategy(Board board)
//        {
//            List<IAction> todoList = new List<IAction>();

//            // 1) Defend my points
//            board.MyFactories.ForEach(f =>
//            {
//                // Check for negative
//                var dangers = new List<Tuple<int, int>>(); // turn, number
//                var availability = f.CountAvailableCyborgs(board);
//                for(int i = 0; i < availability.Count; ++i)
//                {
//                    if (availability[i] < 0)
//                    {
//                        dangers.Add(new Tuple<int, int>(i, - availability[i]));
//                    }
//                }

//                int added = 0;
//                dangers.ForEach(d =>
//                {
//                    int needed = d.Item2 - added;
//                    if(needed > 0)
//                    {
//                        Console.Error.WriteLine($"Has to defend {f.Id} from {needed} cyborgs at turn {d.Item1}.");
//                        added += this.SendClosestCyborgs(board, todoList, f, needed, d.Item1);
//                    }
//                });
//            });

//            // 2) Increase Production
//            board.MyFactories.Where(f => f.Production < 3).ToList().ForEach(f =>
//            {
//                // Check
//                var availability = f.CountAvailableCyborgs(board);
//                int min = availability.Min();

//                if(min >= 10)
//                {
//                    Console.Error.WriteLine($"Can increase production of factory {f.Id}.");

//                    Troop fakeTroop = new Troop()
//                    {
//                        CyborgsNumber = 10,
//                        Source = f,
//                        Destination = null,
//                        Owner = 1,
//                        TurnsBeforeArrival = 50
//                    };
//                    f.NextTroops.Add(fakeTroop);

//                    var action = new IncreaseAction()
//                    {
//                        Factory = f
//                    };
//                    todoList.Add(action);
//                }
//            });

//            // 3) Send to increase (if not already possible)
//            board.MyFactories.Where(f => f.Production < 3 &&
//                                            todoList.FirstOrDefault(a => (a is IncreaseAction && (a as IncreaseAction).Factory.Id == f.Id)) == null)
//            .ToList()
//            .ForEach(f =>
//            {
//                // Check if others can send
//                var availability = f.CountAvailableCyborgs(board);
//                int min = availability.Min();

//                int needed = 10 - min;

//                if (needed > 0)
//                {
//                    Console.Error.WriteLine($"Need {needed} to increase {f.Id}.");
//                    this.SendClosestCyborgs(board, todoList, f, needed, 1000);
//                    // If enough, will auto increase next round
//                }
//            });

//            // 4) Take my side points ==>  = Attack ? ==> find closest instead
//            todoList.AddRange(this.TakeMySideFactories(board));

//            // TODO : redefine side points with distance and not odd/pair
            
//            // 4) Attack if possible

//            // TODO : Check if can repartitate
//            // TODO : Check if can avoid bombs ?

//            // TODO : check to send bomb if > 18 ennemies

//            // TRY early rush : first take best production, then rush ennemies one
//            return todoList;
//        }

//        private List<IAction> TakeMySideFactories(Board board)
//        {
//            List<IAction> todoList = new List<IAction>();


//            List<Factory> mySideFactoriesToTake = new List<Factory>();
//            BoardStaticData.MySideIds.ForEach(id => 
//            {
//                Factory factory = board.GetFactory(id);
//                if(factory.Owner != 1)
//                {
//                    mySideFactoriesToTake.Add(factory);
//                    Console.Error.WriteLine($"{factory.Id} is in my side to take.");
//                }
//            });            

//            mySideFactoriesToTake.ForEach(f =>
//            {
//                // TODO : Will do a better count
//                int neededCyborgs = f.CyborgsNumber;

//                if (f.Owner == -1)
//                    neededCyborgs += (5 * f.Production);

//                board.Troops.Where(t => t.Destination.Id == f.Id).ToList().ForEach(t =>
//                {
//                    if (t.Owner == 1)
//                        neededCyborgs -= t.CyborgsNumber;
//                    else
//                        neededCyborgs += t.CyborgsNumber;
//                });

//                neededCyborgs += 1;

//                if(neededCyborgs > 0)
//                {
//                    this.SendClosestCyborgs(board, todoList, f, neededCyborgs, 1000);                    
//                }
//            });           

//            return todoList;
//        }

//        private int SendClosestCyborgs(Board board, List<IAction> todoList, Factory destination, int neededCyborgs, int turn)
//        {
//            int sent = 0;
//            Console.Error.WriteLine($"{neededCyborgs} cyborgs needed to go to {destination.Id}, within {turn} turns.");

//            var orderedSenders = board.MyFactories.Where(f => f.Id != destination.Id && BoardStaticData.GetDistance(f.Id, destination.Id) < turn).OrderBy(f => BoardStaticData.GetDistance(f.Id, destination.Id)).ToList();

//            for (int i = 0; i < orderedSenders.Count; ++i)
//            {
//                Factory factory = orderedSenders[i];
//                int available = factory.CountAvailableCyborgs(board).Min();

//                Console.Error.WriteLine($"{available} cyborgs available from {factory.Id}.");
//                if (available >= neededCyborgs)
//                {
//                    //send needed
//                    Troop troop = new Troop()
//                    {
//                        CyborgsNumber = neededCyborgs,
//                        Source = factory,
//                        Destination = destination,
//                        Owner = 1,
//                        TurnsBeforeArrival = BoardStaticData.GetDistance(factory.Id, destination.Id)
//                    };
//                    factory.NextTroops.Add(troop);

//                    var action = new MoveAction()
//                    {
//                        CyborgsNumber = neededCyborgs,
//                        Source = factory,
//                        Destination = destination
//                    };
//                    todoList.Add(action);

//                    Console.Error.WriteLine($"{troop.CyborgsNumber} full cyborgs sent from {factory.Id}.");
//                    sent += troop.CyborgsNumber;
//                    break;
//                }
//                else if (available > 0)
//                {
//                    //send available
//                    Troop troop = new Troop()
//                    {
//                        CyborgsNumber = available,
//                        Source = factory,
//                        Destination = destination,
//                        Owner = 1,
//                        TurnsBeforeArrival = BoardStaticData.GetDistance(factory.Id, destination.Id)
//                    };
//                    factory.NextTroops.Add(troop);

//                    var action = new MoveAction()
//                    {
//                        CyborgsNumber = available,
//                        Source = factory,
//                        Destination = destination
//                    };
//                    todoList.Add(action);

//                    Console.Error.WriteLine($"{troop.CyborgsNumber} available cyborgs sent from {factory.Id}.");
//                    sent += troop.CyborgsNumber;
//                    neededCyborgs -= available;
//                }
//            }
//            return sent;
//        }
//    }
//    #endregion

//    #region Board
//    public static class BoardStaticData
//    {
//        // Source, Destination => Distance
//        public static Dictionary<Tuple<int, int>, int> Distances = new Dictionary<Tuple<int, int>, int>();
//        public static int MyStartId;
//        public static int EnnemyStartId;
//        public static int MiddleId;
//        public static List<int> MySideIds;

//        public static void Load(bool readFromFile)
//        {
//            string[] inputs;
//            if (!readFromFile)
//            {                
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
//            else
//            {
//                // Read the file
//                StreamReader file =  new StreamReader(@"E:\VisualStudio2015\GhostInTheCell\GhostInTheCell\Examples\1\static.txt");

//                int factoryCount = int.Parse(file.ReadLine()); // the number of factories
//                int linkCount = int.Parse(file.ReadLine()); // the number of links between factories
//                for (int i = 0; i < linkCount; ++i)
//                {
//                    inputs = file.ReadLine().Split(' ');
//                    int factory1 = int.Parse(inputs[0]);
//                    int factory2 = int.Parse(inputs[1]);
//                    int distance = int.Parse(inputs[2]);
//                    BoardStaticData.Distances.Add(new Tuple<int, int>(factory1, factory2), distance);
//                    BoardStaticData.Distances.Add(new Tuple<int, int>(factory2, factory1), distance);
//                }
         
//                file.Close();
//            }
//        }

//        public static void Init(Board board)
//        {
//            BoardStaticData.MiddleId = 0;
//            BoardStaticData.MyStartId = board.Factories.First(f => f.Owner == 1).Id;
//            BoardStaticData.EnnemyStartId = board.Factories.First(f => f.Owner == -1).Id;

//            BoardStaticData.MySideIds = new List<int>() { BoardStaticData.MyStartId };
//            BoardStaticData.MySideIds.AddRange(
//                board.Factories
//                .Where(f => f.Id != BoardStaticData.MyStartId && (f.Id % 2) == (BoardStaticData.MyStartId % 2) ) // my side
//                .OrderBy(f => BoardStaticData.GetDistance(f.Id, BoardStaticData.MyStartId)) // order by distance
//                .Select(f => f.Id)
//                );
//        }

//        public static int GetDistance(int id1, int id2)
//        {
//            //Console.Error.WriteLine($"Asked distance {id1} to {id2}.");
//            return BoardStaticData.Distances[new Tuple<int, int>(id1, id2)];
//        }
//    }

//    public class Board
//    {
//        public List<Factory> Factories { get; set; }
//        public List<Troop> Troops { get; set; }
//        public List<Bomb> Bombs { get; set; }
//        public int GameTurn { get; set; }

//        public List<Factory> MyFactories { get { return this.Factories.Where(f => f.Owner == 1).ToList(); } }

//        public Board()
//        {
//            this.Factories = new List<Factory>();
//            this.Troops = new List<Troop>();
//            this.Bombs = new List<Bomb>();
//        }

//        public void LoadTurn(bool readFromFile)
//        {
//            if(!readFromFile)
//            {
//                int entityCount = int.Parse(Console.ReadLine()); // the number of entities (e.g. factories and troops)
//                for (int i = 0; i < entityCount; ++i)
//                {
//                    string[] inputs = Console.ReadLine().Split(' ');
//                    int entityId = int.Parse(inputs[0]);
//                    string entityType = inputs[1];
//                    int arg1 = int.Parse(inputs[2]);
//                    int arg2 = int.Parse(inputs[3]);
//                    int arg3 = int.Parse(inputs[4]);
//                    int arg4 = int.Parse(inputs[5]);
//                    int arg5 = int.Parse(inputs[6]);

//                    if (entityType == "FACTORY")
//                    {
//                        Factory factory = new Factory()
//                        {
//                            Id = entityId,
//                            Owner = arg1,
//                            CyborgsNumber = arg2,
//                            Production = arg3
//                        };
//                        this.Factories.Add(factory);
//                    }
//                    else if (entityType == "TROOP")
//                    {
//                        Troop troop = new Troop()
//                        {
//                            Id = entityId,
//                            Owner = arg1,
//                            Source = this.GetFactory(arg2),
//                            Destination = this.GetFactory(arg3),
//                            CyborgsNumber = arg4,
//                            TurnsBeforeArrival = arg5
//                        };
//                        this.Troops.Add(troop);
//                    }
//                    else if (entityType == "BOMB")
//                    {
//                        Bomb bomb = new Bomb()
//                        {
//                            Id = entityId,
//                            Owner = arg1,
//                            Source = this.GetFactory(arg2),
//                            Destination = arg3 == -1 ? null : this.GetFactory(arg3),
//                            TurnsBeforeArrival = arg4
//                        };
//                        this.Bombs.Add(bomb);
//                    }
//                }
//            }
//            else
//            {
//                StreamReader file = new StreamReader(@"E:\VisualStudio2015\GhostInTheCell\GhostInTheCell\Examples\1\turn.txt");

//                int entityCount = int.Parse(file.ReadLine()); // the number of entities (e.g. factories and troops)
//                for (int i = 0; i < entityCount; ++i)
//                {
//                    string[] inputs = file.ReadLine().Split(' ');
//                    int entityId = int.Parse(inputs[0]);
//                    string entityType = inputs[1];
//                    int arg1 = int.Parse(inputs[2]);
//                    int arg2 = int.Parse(inputs[3]);
//                    int arg3 = int.Parse(inputs[4]);
//                    int arg4 = int.Parse(inputs[5]);
//                    int arg5 = int.Parse(inputs[6]);

//                    if (entityType == "FACTORY")
//                    {
//                        Factory factory = new Factory()
//                        {
//                            Id = entityId,
//                            Owner = arg1,
//                            CyborgsNumber = arg2,
//                            Production = arg3
//                        };
//                        this.Factories.Add(factory);
//                    }
//                    else //TROOP
//                    {
//                        Troop troop = new Troop()
//                        {
//                            Id = entityId,
//                            Owner = arg1,
//                            Source = this.Factories.First(f => f.Id == arg2),
//                            Destination = this.Factories.First(f => f.Id == arg3),
//                            CyborgsNumber = arg4,
//                            TurnsBeforeArrival = arg5
//                        };
//                        this.Troops.Add(troop);
//                    }
//                }

//                file.Close();
//            }
//        }

//        public Factory GetFactory(int id)
//        {
//            return this.Factories.First(f => f.Id == id);
//        }
//    }       
    
//    public class Factory
//    {
//        public int Id { get; set; }
//        public int Owner { get; set; }
//        public int CyborgsNumber { get; set; }
//        public int Production { get; set; }
//        // Neighbors for graph ?

//        public List<Troop> NextTroops { get; set; }

//        public Factory()
//        {
//            this.NextTroops = new List<Troop>();
//        }
//        public List<int> CountAvailableCyborgs(Board board)
//        {
//            //Console.Error.WriteLine($"Count for factory {this.Id}.");
//            List<int> available = new List<int>();

//            // Current - Sent this turn        or     - ennemies
//            int start = this.Owner == 1 ? this.CyborgsNumber - this.NextTroops.Sum(t => t.CyborgsNumber) : -this.CyborgsNumber;
            
//            //Console.Error.WriteLine($"Start is {start}.");
//            available.Add(start);

//            // Coming troops
//            var coming = board.Troops.Where(t => t.Destination.Id == this.Id).OrderBy(t => t.TurnsBeforeArrival).ToList();

//            if(coming.Count > 0)
//            {
//                int maxDistance = coming.Max(t => t.TurnsBeforeArrival);
//                //Console.Error.WriteLine($"Max distance is {maxDistance}.");

//                int flow = start;

//                for (int d = 1; d <= maxDistance; ++d)
//                {
//                    // + Production each turn
//                    if(d>0)
//                        flow += this.Production;

//                    // + - Coming troops
//                    coming.Where(t => t.TurnsBeforeArrival == d).ToList().ForEach(t =>
//                    {
//                        if (t.Owner == 1)
//                            flow += t.CyborgsNumber;
//                        else
//                            flow -= t.CyborgsNumber;
//                    });

//                    //Console.Error.WriteLine($"Flow is {flow}.");
//                    available.Add(flow);
//                }
//            }      

//            return available;
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
//    }

//    public class Bomb
//    {
//        public int Id { get; set; }
//        public int Owner { get; set; }
//        public Factory Source { get; set; }
//        public Factory Destination { get; set; }
//        public int TurnsBeforeArrival { get; set; }
//    }
//    #endregion

//    #region Actions
//    public interface IAction
//    {
//        string GetActionString();
//        void Execute(Board board);
//    }

//    public class MoveAction : IAction
//    {
//        public Factory Source { get; set; }
//        public Factory Destination { get; set; }
//        public int CyborgsNumber { get; set; }

//        public string GetActionString()
//        {
//            return $"MOVE {this.Source.Id} {this.Destination.Id} {this.CyborgsNumber}";
//        }

//        public void Execute(Board board)
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

//    public class WaitAction : IAction
//    {
//        public void Execute(Board board)
//        {
//            // Nothing to do
//        }
        
//        public string GetActionString()
//        {
//            return "WAIT";
//        }
//    }

//    public class MessageAction : IAction
//    {
//        public string Message { get; set; }
//        public void Execute(Board board)
//        {
//            // Nothing to do
//        }

//        public string GetActionString()
//        {
//            return $"MSG {this.Message}";
//        }
//    }

//    public class BombAction : IAction
//    {
//        public Factory Source { get; set; }
//        public Factory Destination { get; set; }

//        public void Execute(Board board)
//        {
//            // Create bomb
//            Bomb bomb = new Bomb()
//            {
//                Owner = 1,
//                Source = this.Source,
//                Destination = this.Destination,
//                TurnsBeforeArrival = BoardStaticData.Distances[new Tuple<int, int>(this.Source.Id, this.Destination.Id)]
//            };
//            board.Bombs.Add(bomb);
//        }

//        public string GetActionString()
//        {
//            return $"BOMB {this.Source.Id} {this.Destination.Id}";
//        }
//    }

//    public class IncreaseAction : IAction
//    {
//        public Factory Factory { get; set; }

//        public void Execute(Board board)
//        {
//            if(this.Factory.CyborgsNumber >= 10 && this.Factory.Production < 3)
//            {
//                this.Factory.CyborgsNumber -= 10;
//                this.Factory.Production += 1;
//            }
//        }

//        public string GetActionString()
//        {
//            return $"INC {this.Factory.Id}";
//        }
//    }
//    #endregion
//}