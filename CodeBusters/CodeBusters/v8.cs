//using System;
//using System.Linq;
//using System.IO;
//using System.Text;
//using System.Collections;
//using System.Collections.Generic;

//namespace v8
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
//        public static Coordinate AroundEnnemyBase3
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
//                            (int)(Game.MyTeamId == 1 ? (GameConstants.BaseRadius + GameConstants.VisionRange) * Math.Cos(20.0 * Math.PI / 180) : GameConstants.XMax - (GameConstants.BaseRadius + GameConstants.VisionRange) * Math.Cos(20.0 * Math.PI / 180)),
//                            (int)(Game.MyTeamId == 1 ? (GameConstants.BaseRadius + GameConstants.VisionRange) * Math.Sin(20.0 * Math.PI / 180) : GameConstants.YMax - (GameConstants.BaseRadius + GameConstants.VisionRange) * Math.Sin(20.0 * Math.PI / 180)));
//            ImportantCoordinates.AroundEnnemyBase2 = new Coordinate(
//                            (int)(Game.MyTeamId == 1 ? (GameConstants.BaseRadius + GameConstants.VisionRange) * Math.Cos(70.0 * Math.PI / 180) : GameConstants.XMax - (GameConstants.BaseRadius + GameConstants.VisionRange) * Math.Cos(70.0 * Math.PI / 180)),
//                            (int)(Game.MyTeamId == 1 ? (GameConstants.BaseRadius + GameConstants.VisionRange) * Math.Sin(70.0 * Math.PI / 180) : GameConstants.YMax - (GameConstants.BaseRadius + GameConstants.VisionRange) * Math.Sin(70.0 * Math.PI / 180)));
//            ImportantCoordinates.AroundEnnemyBase3 = new Coordinate(
//                            (int)(Game.MyTeamId == 1 ? (GameConstants.BaseRadius + GameConstants.VisionRange) * Math.Cos(45.0 * Math.PI / 180) : GameConstants.XMax - (GameConstants.BaseRadius + GameConstants.VisionRange) * Math.Cos(45.0 * Math.PI / 180)),
//                            (int)(Game.MyTeamId == 1 ? (GameConstants.BaseRadius + GameConstants.VisionRange) * Math.Sin(45.0 * Math.PI / 180) : GameConstants.YMax - (GameConstants.BaseRadius + GameConstants.VisionRange) * Math.Sin(45.0 * Math.PI / 180)));


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
//                ImportantCoordinates.TopMiddle
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
//            Game.Areas.ForEach(a =>
//            {
//                Ghost g = a.Ghosts.Where(t => t.Id == ghost.Id).FirstOrDefault();
//                if (g != null)
//                {
//                    a.Ghosts.Remove(g);
//                }
//                if (a.IsInside(ghost.Position))
//                {
//                    a.Ghosts.Add(ghost);
//                }
//            }
//            );
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
//            if (buster.TeamId == Game.MyTeamId)
//            {
//                Game.Areas.ForEach(a =>
//                {
//                    if (a.IsInside(buster.Position))
//                    {
//                        a.VisitedTimes++;
//                        a.LastExploredTurn = Game.Turn;
//                    }
//                }
//                );
//            }
//        }
//    }

//    public class Coordinate
//    {
//        // Geographic properties
//        public int X
//        {
//            get;
//            set;
//        }
//        public int Y
//        {
//            get;
//            set;
//        }

//        public Coordinate(int x, int y)
//        {
//            this.X = x;
//            this.Y = y;
//        }

//        // Get Distance
//        public double DistanceTo(Coordinate coord)
//        {
//            return Math.Sqrt((this.X - coord.X) * (this.X - coord.X) + (this.Y - coord.Y) * (this.Y - coord.Y));
//        }

//        // Final printing
//        public override string ToString()
//        {
//            return this.X + " " + this.Y;
//        }
//    }

//    public class Area
//    {
//        public const int L = 3200;
//        public const int l = 1800;

//        public int Row
//        {
//            get;
//            private set;
//        }
//        public int Column
//        {
//            get;
//            private set;
//        }
//        public Coordinate Middle
//        {
//            get { return new Coordinate(Area.L * this.Column + Area.L / 2, Area.l * this.Row + Area.l / 2); }
//        }

//        //Stats
//        public int VisitedTimes
//        {
//            get;
//            set;
//        }

//        public int Malus
//        {
//            get;
//            set;
//        }

//        public List<Ghost> Ghosts
//        {
//            get;
//            set;
//        }

//        public int LastExploredTurn
//        {
//            get;
//            set;
//        }

//        public Area(int x, int y)
//        {
//            this.Row = x;
//            this.Column = y;
//            this.VisitedTimes = 0;
//            if (Game.MyTeamId == 0)
//            {
//                if (x == 0 && y == 0)
//                {
//                    this.Malus = 500;
//                }
//            }
//            else
//            {
//                if (x == 4 && y == 4)
//                {
//                    this.Malus = 500;
//                }
//            }
//            this.Ghosts = new List<Ghost>();
//        }

//        public int Score
//        {
//            get
//            {
//                int ghostsWeight = 0;
//                foreach (Ghost g in this.Ghosts)
//                {
//                    if (Game.Turn - g.LastUpdateTurn < 10)
//                    {
//                        ghostsWeight += 1;
//                    }
//                }
//                int unvisitedWeight = this.VisitedTimes < 2 ? 100 : 0;
//                int visitedNowMalus = this.LastExploredTurn == Game.Turn ? 500 : 0;
//                //return ghostsWeight + unvisitedWeight - this.VisitedTimes - this.Malus;
//                return unvisitedWeight - visitedNowMalus - this.VisitedTimes - this.Malus;
//            }
//        }

//        public int GroupScore
//        {
//            get
//            {
//                int top = this.Row > 0 ? Game.Areas.Where(a => a.Row == this.Row - 1 && a.Column == this.Column).First().Score : 0;
//                int left = this.Column > 0 ? Game.Areas.Where(a => a.Row == this.Row && a.Column == this.Column - 1).First().Score : 0;
//                int bottom = this.Row < 4 ? Game.Areas.Where(a => a.Row == this.Row + 1 && a.Column == this.Column).First().Score : 0;
//                int right = this.Column < 4 ? Game.Areas.Where(a => a.Row == this.Row && a.Column == this.Column + 1).First().Score : 0;
//                return this.Score + top + left + bottom + right;
//            }
//        }

//        public bool IsInside(Coordinate coord)
//        {
//            return coord.Y >= this.Row * Area.l && coord.Y <= (this.Row + 1) * Area.l &&
//                coord.X >= this.Column * Area.L && coord.X <= (this.Column + 1) * Area.L;
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
//            //list of busters where buster.state == 3 && buster.value = this.Id
//            get
//            {
//                return Game.MyBusters.Where(b => b.State == 3 && b.Value == this.Id).Count();
//            }
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
//            list.ForEach(ghost =>
//            {
//                double distance = ghost.DistanceTo(e);
//                if (distance < minDistance)
//                {
//                    final = ghost;
//                    minDistance = distance;
//                }
//            });
//            return final;
//        }
//        public static Ghost GetWeakest(List<Ghost> list)
//        {
//            int minStamina = int.MaxValue;
//            Ghost final = null;
//            list.ForEach(ghost =>
//            {
//                int stamina = ghost.State;
//                if (stamina < minStamina)
//                {
//                    if (Game.Turn < 20 && stamina > 5 * Game.BustersPerPlayer)
//                    {

//                    }
//                    else
//                    {
//                        final = ghost;
//                        minStamina = stamina;
//                    }
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

//        // Strategic properties
//        public Decision Decision
//        {
//            get;
//            set;
//        }

//        public bool FirstStrategy
//        {
//            get;
//            set;
//        }

//        public Buster(string[] prop) : base(prop)
//        {
//            this.Weapon = 0;
//            this.Decision = new Decision();
//            this.Decision.Mission = Mission.None;
//            this.FirstStrategy = false;
//        }

//        public void Update(Buster b)
//        {
//            base.Update(b);
//            if (this.Weapon > 0)
//            {
//                this.Weapon--;
//            }
//            this.FirstStrategy = false;
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
//        public string PrintDecision()
//        {
//            if (this.State == 2)
//            {
//                Console.Error.WriteLine("I'm stunned :'(");
//                return "MOVE " + this.Position.ToString();
//            }
//            else
//            {
//                switch (this.Decision.Mission)
//                {
//                    case Mission.BringGhostHome:
//                        if (this.DistanceTo(ImportantCoordinates.MyBase) < GameConstants.BaseRadius)
//                        {
//                            Game.MyScore++;
//                            return "RELEASE";
//                        }
//                        else
//                        {
//                            return "MOVE " + ImportantCoordinates.MyBase.ToString();
//                        }
//                    case Mission.Bust:
//                        double distanceBust = this.DistanceTo(this.Decision.Target);
//                        if (distanceBust > GameConstants.BustMax)
//                        {
//                            return "MOVE " + this.Decision.Target.Position.ToString();
//                        }
//                        else if (distanceBust > GameConstants.BustMin)
//                        {
//                            return "BUST " + this.Decision.Target.Id;
//                        }
//                        else
//                        {
//                            Coordinate target = new Coordinate((int)Math.Round(((((GameConstants.BustMin) / distanceBust) * this.Position.X - this.Decision.Target.Position.X) * GameConstants.BustMin / distanceBust)),
//                                               (int)Math.Round((((GameConstants.BustMin) / distanceBust) * this.Position.Y - this.Decision.Target.Position.Y) * GameConstants.BustMin / distanceBust));
//                            return "MOVE " + target.ToString();
//                        }
//                    case Mission.Steal:
//                        double distanceSteal = this.DistanceTo(this.Decision.Target);
//                        if (this.Decision.Target.LastUpdateTurn == Game.Turn && distanceSteal < GameConstants.BustMax)
//                        {
//                            this.Weapon = GameConstants.ReloadTurns;
//                            return "STUN " + this.Decision.Target.Id;
//                        }
//                        else
//                        {
//                            return "MOVE " + ImportantCoordinates.EnnemyBase.ToString(); //this.Decision.Target.Position.ToString();
//                        }
//                    case Mission.SupportHome:
//                        return "MOVE " + this.Decision.Target.Position.ToString();
//                    case Mission.Explore:
//                        return "MOVE " + this.Decision.Place.ToString();
//                    case Mission.GiveVision:
//                        return "MOVE " + this.Decision.Target.Position.ToString();
//                    case Mission.Fight:
//                        if (this.Decision.Target.DistanceTo(this) < GameConstants.BustMax)
//                        {
//                            this.Weapon = GameConstants.ReloadTurns;
//                            return "STUN " + this.Decision.Target.Id;
//                        }
//                        else
//                        {
//                            return "MOVE " + this.Decision.Target.Position.ToString();
//                        }
//                    default:
//                        // default should never happen
//                        return "MOVE " + this.Position.ToString();
//                }
//            }
//        }

//        public Coordinate FindAreaToExplore()
//        {
//            Coordinate result = new Coordinate(0, 0);
//            int myId = Game.MyTeamId == 0 ? 0 : -Game.BustersPerPlayer;
//            myId += this.Id;

//            if (Game.Turn < 10)
//            {
//                switch (myId)
//                {
//                    case 0:
//                        // Opponent Base 1
//                        result = ImportantCoordinates.AroundEnnemyBase1;
//                        break;
//                    case 1:
//                        // Top Middle
//                        result = ImportantCoordinates.BottomMiddle;
//                        break;
//                    case 2:
//                        // Right Middle
//                        result = ImportantCoordinates.RightMiddle;
//                        break;
//                    default:
//                        // Opponent Base 2
//                        result = ImportantCoordinates.AroundEnnemyBase2;
//                        break;
//                }
//            }
//            else if (Game.Turn > 170)
//            {
//                switch (myId)
//                {
//                    case 0:
//                        // Opponent Base 1
//                        result = ImportantCoordinates.AroundEnnemyBase1;
//                        break;
//                    case 1:
//                        // Top Middle
//                        result = ImportantCoordinates.AroundEnnemyBase2;
//                        break;
//                    case 2:
//                        // Right Middle
//                        result = ImportantCoordinates.AroundEnnemyBase3;
//                        break;
//                    default:
//                        // Opponent Base 2
//                        result = ImportantCoordinates.RightMiddle;
//                        break;
//                }
//            }
//            else
//            {
//                Area chosenArea = Game.Areas[12];
//                List<Area> orderedAreas = Game.Areas.OrderByDescending(a => a.GroupScore).ToList();
//                chosenArea = orderedAreas[Game.RandomGenerator.Next() % 5];
//                Console.Error.WriteLine("Area " + chosenArea.Row + " " + chosenArea.Column + " : " + chosenArea.GroupScore + " ( Visited : " + chosenArea.VisitedTimes + " times )");
//                return chosenArea.Middle;
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
//    }

//    public enum Mission
//    {
//        None,
//        BringGhostHome,
//        SupportHome,
//        Steal,
//        Bust,
//        GiveVision,
//        Fight,
//        Explore
//    }

//    public class Decision
//    {
//        public Mission Mission
//        {
//            get;
//            set;
//        }

//        public Entity Target
//        {
//            get;
//            set;
//        }

//        public Coordinate Place
//        {
//            get;
//            set;
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

//        public static int MyScore
//        {
//            get;
//            set;
//        }

//        public static List<Area> Areas
//        {
//            get;
//            set;
//        }

//        public static Random RandomGenerator
//        {
//            get;
//            set;
//        }

//        public static void InstantiateAreas()
//        {
//            Game.Areas = new List<Area>();
//            for (int i = 0; i < 5; ++i)
//            {
//                for (int j = 0; j < 5; ++j)
//                {
//                    Area area = new Area(i, j);
//                    Game.Areas.Add(area);
//                }
//            }
//        }

//        static void Main(string[] args)
//        {
//            Game.RandomGenerator = new Random();
//            // Game Data initialization
//            Game.Turn = 0;
//            Game.MyScore = 0;
//            Game.BustersPerPlayer = int.Parse(Console.ReadLine()); // the amount of busters you control
//            Game.GhostCount = int.Parse(Console.ReadLine()); // the amount of ghosts on the map
//            Game.MyTeamId = int.Parse(Console.ReadLine()); // if this is 0, your base is on the top left of the map, if it is one, on the bottom right
//            Game.Ghosts = new List<Ghost>();
//            Game.MyBusters = new List<Buster>();
//            Game.EnnemyBusters = new List<Buster>();
//            ImportantCoordinates.InstantiateCoordinates();
//            Game.InstantiateAreas();
//            // game loop
//            while (true)
//            {
//                Game.Turn++;
//                Console.Error.WriteLine(Game.Turn);

//                int entities = int.Parse(Console.ReadLine()); // the number of busters and ghosts visible to you
//                for (int i = 0; i < entities; i++)
//                {
//                    string inputLine = Console.ReadLine();
//                    Console.Error.WriteLine(inputLine);
//                    string[] inputs = inputLine.Split(' ');
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

//                // Pre compute Strategy:
//                Game.PreComputeStrategy();

//                // Compute Strategy
//                Game.ComputeFirstStrategy();
//                Game.ComputeSecondStrategy();
//                Game.ComputeThirdStrategy();

//                // Print Decision
//                foreach (Buster buster in Game.MyBusters)
//                {
//                    Console.WriteLine(buster.PrintDecision() + " " + buster.Weapon.ToString() + "|" + buster.Decision.Mission.ToString());
//                }
//            }
//        }

//        private static void PreComputeStrategy()
//        {
//            foreach (Buster buster in Game.MyBusters)
//            {
//                if (buster.State == 1 && buster.DistanceTo(ImportantCoordinates.MyBase) < GameConstants.BaseRadius)
//                {
//                    //He will release, warn supporters they are free to go
//                    List<Buster> supporters = Game.MyBusters.Where(b => b.Decision.Mission == Mission.SupportHome && b.Decision.Target.Id == buster.Id).ToList();
//                    supporters.ForEach(s => s.Decision.Mission = Mission.None);
//                }
//            }            
//        }

//        private static void ComputeFirstStrategy()
//        {
//            foreach (Buster buster in Game.MyBusters)
//            {
//                if (buster.State == 2)
//                {
//                    // Stunned :/
//                    // TODO : nothing to do ? // warn intercepted or something ?
//                    buster.Decision.Mission = Mission.None;
//                    List<Buster> mySupporters = Game.MyBusters.Where(b => b.Decision.Mission == Mission.SupportHome &&
//                                                                           b.Decision.Target.Id == buster.Id // Supporting me
//                                                                           ).ToList();
//                    List<Buster> ennemyAround = Game.EnnemyBusters.Where(b => b.DistanceTo(buster) < GameConstants.VisionRange + GameConstants.BustMax && b.LastUpdateTurn >= Game.Turn - 1).ToList();
//                    if(ennemyAround != null)
//                    {
//                        Buster ennemy = Buster.GetClosest(ennemyAround, buster);
//                        if(buster != null)
//                        {
//                            mySupporters.ForEach(sup =>
//                           {
//                               sup.Decision.Mission = Mission.Fight;
//                               sup.Decision.Target = ennemy;
//                           });
//                        }
//                    }
                    
//                }
//                else if (buster.State == 1)
//                {
//                    // Carrying ghost
//                    buster.Decision.Mission = Mission.BringGhostHome;
//                    buster.FirstStrategy = true;

//                    if (Game.MyScore == Game.GhostCount / 2)
//                    {
//                        // Warn all to secure victory
//                        List<Buster> myFriends = Game.MyBusters.Where(b => b.Id != buster.Id).ToList();
//                        myFriends.ForEach(f =>
//                        {
//                            f.Decision.Mission = Mission.SupportHome;
//                            f.Decision.Target = buster;
//                            f.FirstStrategy = true;
//                        }
//                        );
//                    }

//                    //If not in base and ennemy around, ask for support, once.
//                    if (buster.DistanceTo(ImportantCoordinates.MyBase) > GameConstants.BaseRadius)
//                    {
//                        List<Buster> ennemies = Game.EnnemyBusters.Where(b => b.LastUpdateTurn == Game.Turn &&
//                                                                        b.State != 2 && //Maybe do something better here
//                                                                        b.DistanceTo(buster) < GameConstants.VisionRange
//                                                                        ).ToList();

//                        List<Buster> mySupporters = Game.MyBusters.Where(b => b.Decision.Mission == Mission.SupportHome &&
//                                                                            b.Decision.Target.Id == buster.Id // Supporting me
//                                                                            ).ToList();

//                        if (ennemies.Count == 0 && Game.MyScore < Game.GhostCount/2) //TODO maybe -1 ?
//                        {
//                            //No one around anymore, let it go
//                            mySupporters.ForEach(s => s.Decision.Mission = Mission.None);
//                        }
//                        if (mySupporters.Count < ennemies.Count)
//                        {
//                            for (int i = 0; i < ennemies.Count - mySupporters.Count; ++i)
//                            {
//                                List<Buster> notOccupiedFriend = Game.MyBusters.Where(b => b.State != 1 && // not carrying
//                                        b.Decision.Mission != Mission.SupportHome &&// not covering someone
//                                        b.State != 2 // not stunned / TODO : beware
//                                        ).ToList();
//                                if (notOccupiedFriend.Count > 0)
//                                {
//                                    Buster chosenFriend = notOccupiedFriend.First();
//                                    chosenFriend.Decision.Mission = Mission.SupportHome;
//                                    chosenFriend.Decision.Target = buster;
//                                    chosenFriend.FirstStrategy = true;
//                                }
//                                else
//                                {
//                                    // Try with luck
//                                    break;
//                                }
//                            }
//                        }
//                    }
//                }
//                else
//                {
//                    // Do it in second strategy cause of warnings
//                }
//            }
//        }

//        private static void ComputeSecondStrategy()
//        {
//            List<Buster> secondList = Game.MyBusters.Where(b => b.FirstStrategy == false).ToList();
//            foreach (Buster buster in secondList)
//            {
//                // Doing nothing or busting ghost
//                Buster carryingEnnemy = Buster.GetClosest(Game.EnnemyBusters.Where(b => b.LastUpdateTurn == Game.Turn && b.State == 1 && b.DistanceTo(buster) < GameConstants.BustMax).ToList(), buster);
//                // If ennemy on range, get his ghost
//                if (carryingEnnemy != null)
//                {
//                    if (buster.Weapon == 0)
//                    {
//                        //Can Stun
//                        buster.Decision.Mission = Mission.Steal;
//                        buster.Decision.Target = carryingEnnemy;
//                        continue;
//                    }
//                    else
//                    {
//                        double ennemyDistanceToBase = carryingEnnemy.DistanceTo(ImportantCoordinates.EnnemyBase);
//                        if ((ennemyDistanceToBase - GameConstants.BaseRadius) / GameConstants.BusterMove < buster.Weapon)
//                        {
//                            //Chase
//                            buster.Decision.Mission = Mission.Steal;
//                            buster.Decision.Target = carryingEnnemy;
//                            continue;
//                        }
//                    }
//                }

//                // If get here, nothing to stun

//                if (buster.Decision.Mission == Mission.SupportHome)
//                {
//                    // Keep support mission or find one to do
//                    continue;
//                }

//                if (buster.Decision.Mission == Mission.Fight && Game.Turn - buster.Decision.Target.LastUpdateTurn < 5 && buster.Decision.Target.State != 2)
//                {
//                    // Keep Fighting mission or find one to do
//                    continue;
//                }

//                // If chasing and still have a ghost and i haven't been stunned, keep on doing so
//                if (buster.Decision.Mission == Mission.Steal && buster.Decision.Target.State == 1 && buster.State != 2)
//                {
//                    continue;
//                }

//                // If we were targetting a ghost, look if we know how took it : if ennemy, try stun strategy.
//                if (buster.Decision.Mission == Mission.Bust && buster.Decision.Target.LastUpdateTurn != Game.Turn)
//                {
//                    // can be used for generic interception ?
//                    carryingEnnemy = Buster.GetClosest(Game.EnnemyBusters.Where(b => b.LastUpdateTurn == Game.Turn && b.State == 1 && b.Value == buster.Decision.Target.Id).ToList(), buster);
//                    if (carryingEnnemy != null)
//                    {
//                        if (buster.Weapon == 0 && buster.DistanceTo(carryingEnnemy) < GameConstants.BustMax)
//                        {
//                            //Can Stun
//                            buster.Decision.Mission = Mission.Steal;
//                            buster.Decision.Target = carryingEnnemy;
//                            continue;
//                        }
//                        else
//                        {
//                            double ennemyDistanceToBase = carryingEnnemy.DistanceTo(ImportantCoordinates.EnnemyBase);
//                            int ennemyTurnsToRelease = (int)Math.Round((ennemyDistanceToBase - GameConstants.BaseRadius) / GameConstants.BusterMove);
//                            double myDistanceToBase = buster.DistanceTo(ImportantCoordinates.EnnemyBase);
//                            int myTurnsToReachBase = (int)Math.Round((myDistanceToBase - GameConstants.BaseRadius) / GameConstants.BusterMove);

//                            if (myTurnsToReachBase < ennemyTurnsToRelease && myDistanceToBase < buster.Weapon)
//                            {
//                                //Intercept
//                                buster.Decision.Mission = Mission.Steal;
//                                buster.Decision.Target = carryingEnnemy;
//                                continue;
//                            }
//                        }
//                    }
//                }

//                // Bust : check ennemyCount to know how to fight
//                List<Ghost> visibleGhosts = Game.Ghosts.Where(g => g.LastUpdateTurn == Game.Turn &&
//                                                                g.DistanceTo(buster) < GameConstants.VisionRange).ToList();
//                Ghost weakest = Ghost.GetWeakest(visibleGhosts);

//                List<Ghost> allVisibleGhosts = Game.Ghosts.Where(g => g.LastUpdateTurn == Game.Turn).ToList();
//                Ghost allWeakest = Ghost.GetWeakest(allVisibleGhosts);

//                if (allWeakest != null && allWeakest.DistanceTo(buster) < 5 * GameConstants.BusterMove)
//                {
//                    // Maybe look at time to get there
//                    buster.Decision.Mission = Mission.Bust;
//                    buster.Decision.Target = allWeakest;
//                    continue;
//                }

//                if (weakest != null)
//                {
//                    buster.Decision.Mission = Mission.Bust;
//                    buster.Decision.Target = weakest;
//                    continue;
//                }

//                //No ghost around, join friend

//                if (allWeakest != null)
//                {
//                    buster.Decision.Mission = Mission.Bust;
//                    buster.Decision.Target = allWeakest;
//                    continue;
//                }

//                //Nothing at all... Explore !
//                if (buster.Decision.Mission == Mission.Explore)
//                {
//                    Area targettedArea = Game.Areas.Where(a => buster.Decision.Place.X == a.Middle.X && buster.Decision.Place.Y == a.Middle.Y).FirstOrDefault();
//                    if (targettedArea != null)
//                    {
//                        if (targettedArea.IsInside(buster.Position))
//                        {
//                            buster.Decision.Place = buster.FindAreaToExplore();
//                        }
//                        else
//                        {
//                            // keep doing the same
//                            continue;
//                        }
//                    }
//                }

//                buster.Decision.Mission = Mission.Explore;
//                buster.Decision.Place = buster.FindAreaToExplore();
//            }
//        }

//        public static void ComputeThirdStrategy()
//        {
//            // Compute fights for a ghost
//            List<Buster> bustingBusters = Game.MyBusters.Where(b => b.Decision.Mission == Mission.Bust && b.State != 2).ToList();
//            List<Ghost> bustedGhosts = bustingBusters.Select(b => b.Decision.Target).Distinct().Cast<Ghost>().ToList();
//            bustedGhosts.ForEach(ghost =>
//           {
//               List<Buster> concerned = bustingBusters.Where(b => b.Decision.Target.Id == ghost.Id).ToList();
//               List<Buster> realBustingConcerned = concerned.Where(b => b.DistanceTo(ghost) < GameConstants.BustMax && b.DistanceTo(ghost) > GameConstants.BustMin).ToList();
//               int myBustingCount = realBustingConcerned.Count;

//               int myWaitedCount = concerned.Count - myBustingCount;
//               List<Buster> visibleBustingEnnemies = Game.EnnemyBusters.Where(b => b.State == 3 && b.Value == ghost.Id && b.LastUpdateTurn == Game.Turn).ToList();
//               List<Buster> visibleEnnemiesAround = Game.EnnemyBusters.Where(b => b.DistanceTo(ghost) < GameConstants.VisionRange && b.State != 2 && b.LastUpdateTurn == Game.Turn).ToList();

//               if (myBustingCount == ghost.EnnemyBustersCount)
//               {
//                   if (visibleBustingEnnemies.Count < ghost.EnnemyBustersCount)
//                   {
//                       if (ghost.State > ghost.Value && ghost.State < 6 * myBustingCount)
//                       {
//                           concerned.First().Decision.Mission = Mission.GiveVision;
//                           concerned.First().Decision.Target = ghost;
//                       }
//                       // Else, hope for someone to help
//                   }
//                   else
//                   {
//                       //Trigger 
//                       if (ghost.State < 6 * myBustingCount)
//                       {
//                           int i = 0;
//                           visibleBustingEnnemies.ForEach(v =>
//                          {
//                              if (i < myBustingCount)
//                              {
//                                  realBustingConcerned[i].Decision.Mission = Mission.Fight;
//                                  realBustingConcerned[i].Decision.Target = v;
//                                  ++i;
//                              }
//                          }
//                               );

//                       }
//                   }

//               }
//               else if (myBustingCount < ghost.EnnemyBustersCount )
//               {
//                   // will I have someone to help me ?
//                   if (myWaitedCount > 0)
//                   {
//                       //keep on anyway
//                   }
//                   else
//                   {
//                       // else try stun
//                       int i = 0;
//                       visibleBustingEnnemies.ForEach(v =>
//                      {
//                          if (i < myBustingCount)
//                          {
//                              realBustingConcerned[i].Decision.Mission = Mission.Fight;
//                              realBustingConcerned[i].Decision.Target = v;
//                          }
//                          ++i;
//                      });
//                   }
//               }
//               else if (concerned.Count <= visibleEnnemiesAround.Count)
//               {
//                   int i = 0;
//                   visibleEnnemiesAround.ForEach(v =>
//                   {
//                       if (i < myBustingCount)
//                       {
//                           realBustingConcerned[i].Decision.Mission = Mission.Fight;
//                           realBustingConcerned[i].Decision.Target = v;
//                       }
//                       ++i;
//                   });
//               }
//               // if >, nothing to do, we will get it !
//           }
//            );
//        }
//    }
//}