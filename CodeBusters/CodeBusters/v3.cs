//using System;
//using System.Linq;
//using System.IO;
//using System.Text;
//using System.Collections;
//using System.Collections.Generic;

//namespace v3
//{
//    // Gameplay Constants
//    public static class GameConstants
//    {
//        public const int XMax = 16000;
//        public const int YMax = 9000;

//        public const int VisionRange = 2200;

//        public const int BustMin = 900;
//        public const int BustMax = 1760;

//        public const int BaseRadius = 1600;

//        public const int BusterMove = 800;
//        public const int GhostMove = 400;

//        public const int ReloadTurns = 20;
//    }

//    // Consider all coordinates and their mirror like if we were top left (team 0)
//    public static class ImportantCoordinates
//    {
//        // List of Coordinates :
//        public static Coordinate MyBase
//        {
//            get;
//            private set;
//        }
//        public static Coordinate EnnemyBase
//        {
//            get;
//            private set;
//        }

//        public static Coordinate AroundEnnemyBase1
//        {
//            get;
//            private set;
//        }
//        public static Coordinate AroundEnnemyBase2
//        {
//            get;
//            private set;
//        }

//        public static Coordinate BottomMiddle
//        {
//            get;
//            private set;
//        }
//        public static Coordinate TopMiddle
//        {
//            get;
//            private set;
//        }
//        public static Coordinate RightMiddle
//        {
//            get;
//            private set;
//        }
//        public static Coordinate LeftMiddle
//        {
//            get;
//            private set;
//        }


//        // Strategy Path :
//        public static List<Coordinate> StrategyPath
//        {
//            get;
//            set;
//        }

//        public static void InstantiateCoordinates()
//        {
//            // Fill coordinates
//            ImportantCoordinates.MyBase = new Coordinate(
//                                Game.MyTeamId == 0 ? 0 : GameConstants.XMax,
//                                Game.MyTeamId == 0 ? 0 : GameConstants.YMax);
//            ImportantCoordinates.EnnemyBase = new Coordinate(
//                                Game.MyTeamId == 1 ? 0 : GameConstants.XMax,
//                                Game.MyTeamId == 1 ? 0 : GameConstants.YMax);


//            ImportantCoordinates.AroundEnnemyBase1 = new Coordinate(
//                            (int)(Game.MyTeamId == 1 ? (GameConstants.BaseRadius + GameConstants.VisionRange) * Math.Cos(30.0 * Math.PI / 180) : GameConstants.XMax - (GameConstants.BaseRadius + GameConstants.VisionRange) * Math.Cos(30.0 * Math.PI / 180)),
//                            (int)(Game.MyTeamId == 1 ? (GameConstants.BaseRadius + GameConstants.VisionRange) * Math.Sin(30.0 * Math.PI / 180) : GameConstants.YMax - (GameConstants.BaseRadius + GameConstants.VisionRange) * Math.Sin(30.0 * Math.PI / 180)));
//            ImportantCoordinates.AroundEnnemyBase2 = new Coordinate(
//                            (int)(Game.MyTeamId == 1 ? (GameConstants.BaseRadius + GameConstants.VisionRange) * Math.Cos(60.0 * Math.PI / 180) : GameConstants.XMax - (GameConstants.BaseRadius + GameConstants.VisionRange) * Math.Cos(60.0 * Math.PI / 180)),
//                            (int)(Game.MyTeamId == 1 ? (GameConstants.BaseRadius + GameConstants.VisionRange) * Math.Sin(60.0 * Math.PI / 180) : GameConstants.YMax - (GameConstants.BaseRadius + GameConstants.VisionRange) * Math.Sin(60.0 * Math.PI / 180)));


//            ImportantCoordinates.BottomMiddle = new Coordinate(
//                                GameConstants.XMax / 2,
//                                Game.MyTeamId == 1 ? GameConstants.VisionRange : GameConstants.YMax - GameConstants.VisionRange);
//            ImportantCoordinates.TopMiddle = new Coordinate(
//                                GameConstants.XMax / 2,
//                                Game.MyTeamId == 0 ? GameConstants.VisionRange : GameConstants.YMax - GameConstants.VisionRange);
//            ImportantCoordinates.RightMiddle = new Coordinate(
//                                Game.MyTeamId == 1 ? GameConstants.VisionRange : GameConstants.XMax - GameConstants.VisionRange,
//                                GameConstants.YMax / 2);
//            ImportantCoordinates.LeftMiddle = new Coordinate(
//                                Game.MyTeamId == 0 ? GameConstants.VisionRange : GameConstants.XMax - GameConstants.VisionRange,
//                                GameConstants.YMax / 2);


//            // Fill strategy path
//            ImportantCoordinates.StrategyPath = new List<Coordinate>()
//            {
//                ImportantCoordinates.BottomMiddle,
//                ImportantCoordinates.RightMiddle,
//                ImportantCoordinates.TopMiddle,
//                ImportantCoordinates.AroundEnnemyBase1
//            };
//        }
//    }

//    public static class MyExtensions
//    {
//        public static void AddEntity(this List<Ghost> list, Ghost ghost)
//        {
//            Ghost found = list.Where(g => g.Id == ghost.Id).FirstOrDefault();
//            if (found != null)
//            {
//                found.Update(ghost);
//            }
//            else
//            {
//                list.Add(ghost);
//            }
//        }

//        public static void AddEntity(this List<Buster> list, Buster buster)
//        {
//            Buster found = list.Where(b => b.Id == buster.Id).FirstOrDefault();
//            if (found != null)
//            {
//                found.Update(buster);
//            }
//            else
//            {
//                list.Add(buster);
//            }
//        }
//    }

//    public class Coordinate
//    {
//        // Geographic properties
//        public int x
//        {
//            get;
//            set;
//        }
//        public int y
//        {
//            get;
//            set;
//        }

//        public Coordinate(int x, int y)
//        {
//            this.x = x;
//            this.y = y;
//        }

//        // Get Distance
//        public double DistanceTo(Coordinate coord)
//        {
//            return Math.Sqrt((this.x - coord.x) * (this.x - coord.x) + (this.y - coord.y) * (this.y - coord.y));
//        }

//        // Final printing
//        public override string ToString()
//        {
//            return this.x + " " + this.y;
//        }
//    }

//    public abstract class Entity
//    {
//        // Game Properties
//        public int Id
//        {
//            get;
//            set;
//        }

//        public Coordinate Position
//        {
//            get;
//            set;
//        }

//        public int TeamId
//        {
//            get;
//            set;
//        }

//        public int State
//        {
//            get;
//            set;
//        }

//        public int Value
//        {
//            get;
//            set;
//        }

//        // Last update
//        public int LastUpdateTurn
//        {
//            get;
//            set;
//        }

//        public Entity(string[] prop)
//        {
//            this.Id = int.Parse(prop[0]);
//            this.Position = new Coordinate(int.Parse(prop[1]), int.Parse(prop[2]));
//            this.TeamId = int.Parse(prop[3]);
//            this.State = int.Parse(prop[4]);
//            this.Value = int.Parse(prop[5]);
//            this.LastUpdateTurn = Game.Turn;
//        }

//        public void Update(Entity e)
//        {
//            this.Position = e.Position;
//            this.State = e.State;
//            this.Value = e.Value;
//            this.LastUpdateTurn = Game.Turn;
//        }

//        // Just clearer functions for distance
//        public double DistanceTo(Entity e)
//        {
//            return this.Position.DistanceTo(e.Position);
//        }

//        public double DistanceTo(Coordinate p)
//        {
//            return this.Position.DistanceTo(p);
//        }
//    }

//    public class Ghost : Entity
//    {
//        // Properties to find out how many people are on this ghost
//        public int MyBustersCount
//        {
//            get;
//            set;
//        }

//        public int EnnemyBustersCount
//        {
//            get
//            {
//                return this.Value - this.MyBustersCount;
//            }
//        }

//        public Ghost(string[] prop) : base(prop)
//        {
//            this.MyBustersCount = 0;
//        }

//        public void Update(Ghost g)
//        {
//            base.Update(g);
//        }

//        // Functions for getting closest entities
//        public static Ghost GetClosest(List<Ghost> list, Entity e)
//        {
//            double minDistance = double.MaxValue;
//            Ghost final = null;
//            list.ForEach(buster =>
//            {
//                double distance = buster.DistanceTo(e);
//                if (distance < minDistance)
//                {
//                    final = buster;
//                    minDistance = distance;
//                }
//            });
//            return final;
//        }
//    }

//    public class Buster : Entity
//    {
//        // Specific properties
//        public int Weapon
//        {
//            get;
//            set;
//        }

//        //Have to do better here in case genetic algo ?
//        //public string Decision
//        //{
//        //    get;
//        //    set;
//        //}

//        // Strategic properties
//        public bool HasWarnedAvenge
//        {
//            get;
//            set;
//        }

//        public Buster Target
//        {
//            get;
//            set;
//        }

//        public Buster(string[] prop) : base(prop)
//        {
//            this.Weapon = 0;
//            this.Target = null;
//            this.HasWarnedAvenge = false;
//        }

//        public void Update(Buster b)
//        {
//            base.Update(b);
//            if (this.Weapon > 0)
//            {
//                this.Weapon--;
//            }

//            // Strategic updates
//            if (this.State == 2 && b.State != 2)
//            {
//                // I have been freed, no more warning
//                this.HasWarnedAvenge = false;
//            }
//        }

//        // Functions for getting closest entities
//        public static Buster GetClosest(List<Buster> list, Entity e)
//        {
//            double minDistance = double.MaxValue;
//            Buster final = null;
//            list.ForEach(buster =>
//            {
//                double distance = buster.DistanceTo(e);
//                if (distance < minDistance)
//                {
//                    final = buster;
//                    minDistance = distance;
//                }
//            });
//            return final;
//        }

//        // Strategic functions
//        public string FindDecision()
//        {
//            Console.Error.WriteLine("My decision :");
//            if (this.State == 2)
//            {
//                Console.Error.WriteLine("I'm stunned :'(");
//                return "MOVE " + this.Position.ToString();
//            }
//            if (this.State == 1)
//            {
//                // Coming Back with a Ghost
//                Console.Error.WriteLine("Coming back with a ghost");
//                double baseDistance = this.DistanceTo(ImportantCoordinates.MyBase);
//                if (baseDistance < GameConstants.BaseRadius)
//                {
//                    return "RELEASE";
//                }
//                else
//                {
//                    return "MOVE " + ImportantCoordinates.MyBase.ToString();
//                }
//            }
//            else
//            {
//                Ghost weakest = this.FindWeakestGhost();
//                Buster carryingEnnemy = Buster.GetClosest(Game.EnnemyBusters.Where(b => b.LastUpdateTurn == Game.Turn && b.State == 1 && b.DistanceTo(this) < GameConstants.BustMax).ToList(), this);
//                // If ennemy on range, get his ghost
//                if (carryingEnnemy != null)
//                {
//                    if (this.Weapon == 0)
//                    {
//                        this.Weapon = GameConstants.ReloadTurns;
//                        return "STUN " + carryingEnnemy.Id;
//                    }
//                }

//                // Have to find what to do
//                if (this.Target != null)
//                {
//                    // Go there ! (position depends on last update turns, turns to go to base etc...)
//                    if (Game.Turn - this.Target.LastUpdateTurn > 5 || this.Target.State != 2)
//                    {
//                        this.Target = null;
//                    }
//                    else
//                    {
//                        Console.Error.WriteLine("Go avenge !");
//                        return "MOVE " + this.Target.Position.ToString();
//                    }
//                }

//                if (weakest == null)
//                {
//                    //If no ghost
//                    if (carryingEnnemy == null)
//                    {
//                        Console.Error.WriteLine("No one around, explore");
//                        return "MOVE " + this.FindAreaToExplore().ToString();
//                    }
//                    else
//                    {
//                        Console.Error.WriteLine("Ennemy to catch, follow or stun");
//                        if (this.Weapon == 0)
//                        {
//                            this.Weapon = GameConstants.ReloadTurns;
//                            return "STUN " + carryingEnnemy.Id;
//                        }
//                        else // do check if we will stun him before base
//                        {
//                            return "MOVE " + carryingEnnemy.Position.ToString();
//                        }
//                    }
//                }
//                else
//                {
//                    double distance = weakest.DistanceTo(this);
//                    if (distance > GameConstants.BustMax)
//                    {
//                        //Get Closer
//                        Console.Error.WriteLine("Get closer");
//                        return "MOVE " + weakest.Position.ToString();
//                    }
//                    if (distance < GameConstants.BustMin)
//                    {
//                        //Stay, the Ghost will fear
//                        Console.Error.WriteLine("He will fear me");
//                        return "MOVE " + new Coordinate(
//                                            weakest.Position.x + (Game.MyTeamId == 0 ? -GameConstants.BustMin : GameConstants.BustMin) / 2,
//                                            weakest.Position.y + (Game.MyTeamId == 0 ? -GameConstants.BustMin : GameConstants.BustMin) / 2
//                                                        ).ToString();
//                    }
//                    // Else we can Bust
//                    Console.Error.WriteLine("We can Bust");
//                    return "BUST " + weakest.Id;
//                }
//            }
//        }

//        public Coordinate FindAreaToExplore()
//        {
//            Coordinate result = new Coordinate(0, 0);
//            int myId = Game.MyTeamId == 0 ? 0 : -Game.BustersPerPlayer;
//            myId += this.Id;
//            switch (myId)
//            {
//                case 0:
//                    // Opponent Base 1
//                    result = ImportantCoordinates.AroundEnnemyBase1;
//                    break;
//                case 1:
//                    // Top Middle
//                    result = ImportantCoordinates.BottomMiddle;
//                    break;
//                case 2:
//                    // Right Middle
//                    result = ImportantCoordinates.RightMiddle;
//                    break;
//                default:
//                    // Opponent Base 2
//                    result = ImportantCoordinates.AroundEnnemyBase2;
//                    break;
//            }
//            return result;
//        }

//        public Ghost FindWeakestGhost()
//        {
//            int minStamina = int.MaxValue;
//            Ghost final = null;
//            foreach (Ghost ghost in Game.Ghosts.Where(g => g.LastUpdateTurn == Game.Turn).ToList())
//            {
//                if (ghost.State < minStamina)
//                {
//                    final = ghost;
//                    minStamina = ghost.State;
//                }
//            }
//            return final;
//        }

//        public void WarnClosestToAvenge()
//        {
//            // Check if I am stunned !
//            if (this.State == 2 && !this.HasWarnedAvenge)
//            {
//                Buster ennemyBuster = Game.EnnemyBusters.Where(b => b.LastUpdateTurn == Game.Turn && //Updated
//                                                                b.State == 1 && //Carrying
//                                                                this.Position.DistanceTo(b.Position) < GameConstants.VisionRange //Around me
//                                                                ).FirstOrDefault();
//                if (ennemyBuster != null)
//                {
//                    // Warn him to our closest friend !
//                    Buster closestFriend = Buster.GetClosest(Game.MyBusters.Where(b => b.Id != this.Id // Not me
//                                                                                    && b.Target == null && // Not chasing someone
//                                                                                    b.Weapon < 4 && // Got some hope
//                                                                                    b.State != 2).ToList(), this);
//                    if (closestFriend != null)
//                    {
//                        this.HasWarnedAvenge = true;
//                        closestFriend.Target = ennemyBuster;
//                    }
//                }
//            }
//        }
//    }

//    // Game Engine
//    public static class Game
//    {
//        public static List<Ghost> Ghosts
//        {
//            get;
//            set;
//        }

//        public static List<Buster> MyBusters
//        {
//            get;
//            set;
//        }

//        public static List<Buster> EnnemyBusters
//        {
//            get;
//            set;
//        }

//        public static int MyTeamId
//        {
//            get;
//            set;
//        }

//        public static int Turn
//        {
//            get;
//            set;
//        }

//        public static int BustersPerPlayer
//        {
//            get;
//            set;
//        }

//        public static int GhostCount
//        {
//            get;
//            set;
//        }

//        static void Main(string[] args)
//        {
//            // Game Data initialization
//            Game.Turn = 0;
//            Game.BustersPerPlayer = int.Parse(Console.ReadLine()); // the amount of busters you control
//            Game.GhostCount = int.Parse(Console.ReadLine()); // the amount of ghosts on the map
//            Game.MyTeamId = int.Parse(Console.ReadLine()); // if this is 0, your base is on the top left of the map, if it is one, on the bottom right
//            Game.Ghosts = new List<Ghost>();
//            Game.MyBusters = new List<Buster>();
//            Game.EnnemyBusters = new List<Buster>();
//            ImportantCoordinates.InstantiateCoordinates();

//            // game loop
//            while (true)
//            {
//                Game.Turn++;

//                int entities = int.Parse(Console.ReadLine()); // the number of busters and ghosts visible to you
//                for (int i = 0; i < entities; i++)
//                {
//                    string[] inputs = Console.ReadLine().Split(' ');
//                    int type = int.Parse(inputs[3]);
//                    if (type == -1)
//                    {
//                        //Ghost
//                        Game.Ghosts.AddEntity(new Ghost(inputs));
//                    }
//                    else
//                    {
//                        //Buster
//                        if (Game.MyTeamId == type)
//                        {
//                            Game.MyBusters.AddEntity(new Buster(inputs));
//                        }
//                        else
//                        {
//                            Game.EnnemyBusters.AddEntity(new Buster(inputs));
//                        }
//                    }
//                }

//                // Compute Data
//                foreach (Buster buster in Game.MyBusters)
//                {
//                    buster.WarnClosestToAvenge();
//                }

//                // Take Decision
//                foreach (Buster buster in Game.MyBusters)
//                {
//                    Console.WriteLine(buster.FindDecision());
//                }
//            }
//        }
//    }
//}