//using System;
//using System.Collections.Generic;
//using System.Linq;
//using System.IO;

//namespace v3
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

//            // Actual score :
//            var myProduction = board.MyFactories.Sum(f => f.Production);
//            var ennemyProduction = board.Factories.Where(f => f.Owner == -1).Sum(t => t.Production);

//            todoList.Add(new MessageAction() { Message = $"{myProduction} vs {ennemyProduction}" });

//            var mySideFactories = new List<Factory>();
//            BoardStaticData.MySideIds.ForEach(id => mySideFactories.Add(board.GetFactory(id)));

//            // 1) Defend
//            board.MyFactories.ForEach(f =>
//            {
//                // Check for negative
//                var dangers = new List<Tuple<int, int>>(); // turn, number
//                var availability = f.CountAvailableCyborgs(board);
//                for (int i = 0; i < availability.Count; ++i)
//                {
//                    if (availability[i] < 0)
//                    {
//                        dangers.Add(new Tuple<int, int>(i, -availability[i]));
//                    }
//                }

//                int added = 0;
//                dangers.ForEach(d =>
//                {
//                    int needed = d.Item2 - added;
//                    if (needed > 0)
//                    {
//                        Console.Error.WriteLine($"Has to defend {f.Id} from {needed} cyborgs at turn {d.Item1}.");
//                        added += this.SendClosestCyborgs(board, todoList, f, needed, d.Item1);
//                    }
//                });
//            });

//            // 2) Upgrade and colonize
//            board.MyFactories.ForEach(mf =>
//            {
//                // 1) If prod not maxed, wait to up it.
//                if (mf.Production < 3 && mf.CyborgsNumber > 10)
//                {
//                    Console.Error.WriteLine($"Can increase {mf.Id}.");
//                    mf.NextTroops.Add(new Troop() { Owner = 1, Source = mf, CyborgsNumber = 10 });
//                    todoList.Add(new IncreaseAction() { Factory = mf });
//                }

//                // 2) Find most valuable neutral target in my side
//                var neutralTargets = mySideFactories.Where(f => f.Owner == 0).OrderByDescending(f => f.Production).ThenBy(f => BoardStaticData.GetDistance(f.Id, mf.Id)).ToList();

//                var myFactoryAvailabilities = mf.CountAvailableCyborgs(board);
//                var available = myFactoryAvailabilities.Min();

//                int i = 0;
//                while (available > 0 && i < neutralTargets.Count)
//                {
//                    Console.Error.WriteLine($"Still {available} cyborgs available on {mf.Id}.");
//                    var target = neutralTargets[i];
//                    var targetAvailabilities = target.CountAvailableCyborgs(board);
//                    var distance = BoardStaticData.GetDistance(mf.Id, target.Id);

//                    int needed = 1;
//                    if(targetAvailabilities.Count == 1)
//                    {
//                        needed -= targetAvailabilities.First();
//                    }
//                    else if(distance >= targetAvailabilities.Count) // > 1
//                    {
//                        needed -= targetAvailabilities.Last();
//                    }
//                    else
//                    {
//                        needed -= targetAvailabilities.Skip(distance).Min();
//                    }

//                    Console.Error.WriteLine($"{needed} cyborgs needed on {target.Id} : ");
//                    targetAvailabilities.ForEach(a => Console.Error.Write($"{a} "));

//                    if (needed > 0)
//                    {
//                        var sent = Math.Min(needed, available);

//                        Console.Error.WriteLine($"Send {sent} cyborgs on {target.Id}.");
//                        mf.NextTroops.Add(new Troop() { Owner = 1, Source = mf, Destination = target, CyborgsNumber = sent });
//                        todoList.Add(new MoveAction() { Source = mf, Destination = target, CyborgsNumber = sent });

//                        available -= sent;
//                    }
                                        
//                    ++i;
//                }                
//            });

//            // 3) Find most valuable ennemy target -- better scoring here
//            var ennemyTargets = board.Factories.Where(f => f.Owner == -1).OrderByDescending(f => f.Production).ThenBy(f => (f.Id == BoardStaticData.MyStartId) ? 0 : BoardStaticData.GetDistance(f.Id, BoardStaticData.MyStartId)).ToList();

//            if(ennemyTargets.Count > 0)
//            {
//                ennemyTargets.ForEach(target =>
//                {
//                   Console.Error.WriteLine($"Target is {target.Id}.");
//                   var targetAvailabilities = target.CountAvailableCyborgs(board);
//                   if (targetAvailabilities.Last() > 0)
//                   {
//                        // will be mine
//                        Console.Error.WriteLine($"{target.Id} will become mine in the end.");
//                   }
//                   else
//                   {
//                        // should compute whole avail
//                       var needed = 10 - targetAvailabilities.Min();
//                       this.SendClosestCyborgs(board, todoList, target, needed, 1000);
//                   }
//                });
//            }         

//            if (todoList.Count == 0)
//            {
//                todoList.Add(new WaitAction());
//            }           

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
//                .Where(f => f.Id != BoardStaticData.MyStartId && f.Id != BoardStaticData.MiddleId && (f.Id % 2) == (BoardStaticData.MyStartId % 2) ) // my side
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
//            List<int> available = new List<int>();
//            var owner = this.Owner;

//            // Current - Sent this turn        or     - ennemies
//            int start = this.Owner == 1 ? this.CyborgsNumber - this.NextTroops.Sum(t => t.CyborgsNumber) : -this.CyborgsNumber;
            
//            // Coming troops
//            var coming = board.Troops.Where(t => t.Destination.Id == this.Id).OrderBy(t => t.TurnsBeforeArrival).ToList();

//            if(coming.Count > 0)
//            {
//                int maxDistance = coming.Max(t => t.TurnsBeforeArrival);

//                int flow = start;

//                for (int d = 1; d <= maxDistance; ++d)
//                {
//                    // + Production each turn
//                    flow += owner * this.Production;

//                    // + - Coming troops
//                    var comingEnnemies = coming.Where(t => t.TurnsBeforeArrival == d && t.Owner == -1).Sum(t => t.CyborgsNumber);
//                    var comingMine = coming.Where(t => t.TurnsBeforeArrival == d && t.Owner == 1).Sum(t => t.CyborgsNumber);

//                    int fightWinner;
//                    if (comingMine > comingEnnemies)
//                        fightWinner = 1;
//                    else if (comingMine == comingEnnemies)
//                        fightWinner = 0;
//                    else
//                        fightWinner = -1;

//                    // fight the flow
//                    flow += comingMine - comingEnnemies;

//                    if(fightWinner != 0)
//                    {
//                        // check who win
//                        if (fightWinner == 1 && flow > 0)
//                            owner = 1;

//                        if (fightWinner == -1 && flow < 0)
//                            owner = -1;
//                    }                    

//                    available.Add(flow);
//                }
//            }
//            else
//            {
//                available.Add(start);
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