//using System;
//using System.Linq;
//using System.IO;
//using System.Text;
//using System.Collections;
//using System.Collections.Generic;

//namespace v2
//{
//    public static class Constants
//    {
//        /* Constants */
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

//    public static class ImportantCoordinates
//    {
//        // Consider all coordinates and their mirror like if we were bottom left (team 0)
//        public static Coordinate MyBase = new Coordinate(
//                                Game.MyTeamId == 0 ? 0 : Constants.XMax,
//                                Game.MyTeamId == 0 ? 0 : Constants.YMax);
//        public static Coordinate EnnemyBase = new Coordinate(
//                                Game.MyTeamId == 1 ? 0 : Constants.XMax,
//                                Game.MyTeamId == 1 ? 0 : Constants.YMax);

//        public static Coordinate AroundEnnemyBase1 = new Coordinate(
//                            (int)(Game.MyTeamId == 1 ? (Constants.BaseRadius + Constants.VisionRange) * Math.Cos(30.0 * Math.PI / 180) : Constants.XMax - (Constants.BaseRadius + Constants.VisionRange) * Math.Cos(30.0 * Math.PI / 180)),
//                            (int)(Game.MyTeamId == 1 ? (Constants.BaseRadius + Constants.VisionRange) * Math.Sin(30.0 * Math.PI / 180) : Constants.YMax - (Constants.BaseRadius + Constants.VisionRange) * Math.Sin(30.0 * Math.PI / 180)));
//        public static Coordinate AroundEnnemyBase2 = new Coordinate(
//                            (int)(Game.MyTeamId == 1 ? (Constants.BaseRadius + Constants.VisionRange) * Math.Cos(60.0 * Math.PI / 180) : Constants.XMax - (Constants.BaseRadius + Constants.VisionRange) * Math.Cos(60.0 * Math.PI / 180)),
//                            (int)(Game.MyTeamId == 1 ? (Constants.BaseRadius + Constants.VisionRange) * Math.Sin(60.0 * Math.PI / 180) : Constants.YMax - (Constants.BaseRadius + Constants.VisionRange) * Math.Sin(60.0 * Math.PI / 180)));

//        public static Coordinate TopMiddle = new Coordinate(
//                                Constants.XMax / 2,
//                                Game.MyTeamId == 1 ? Constants.VisionRange : Constants.YMax - Constants.VisionRange);
//        public static Coordinate RightMiddle = new Coordinate(
//                                Game.MyTeamId == 1 ? Constants.VisionRange : Constants.XMax - Constants.VisionRange,
//                                Constants.YMax / 2);
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

//        public double GetDistanceTo(Coordinate coord)
//        {
//            return Math.Sqrt((this.x - coord.x) * (this.x - coord.x) + (this.y - coord.y) * (this.y - coord.y));
//        }

//        public override string ToString()
//        {
//            return this.x + " " + this.y;
//        }
//    }

//    public abstract class Entity
//    {
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
//        public int Turn
//        {
//            get;
//            set;
//        }
//    }

//    public class Ghost : Entity
//    {
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
        
//        public Ghost(string[] prop)
//        {
//            this.Id = int.Parse(prop[0]);
//            this.Position = new Coordinate(int.Parse(prop[1]), int.Parse(prop[2]));
//            this.State = int.Parse(prop[4]);
//            this.Value = int.Parse(prop[5]);
//            this.Turn = Game.Turn;
//            this.MyBustersCount = 0;
//        }

//        public void Update(Ghost g)
//        {
//            this.Position = g.Position;
//            this.State = g.State;
//            this.Value = g.Value;
//            this.Turn = Game.Turn;
//        }

//    }

//    public class Buster : Entity
//    {
//        public int TeamId
//        {
//            get;
//            set;
//        }

//        public int Weapon
//        {
//            get;
//            set;
//        }

//        public string Decision
//        {
//            get;
//            set;
//        }

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

//        public Buster(string[] prop)
//        {
//            this.Id = int.Parse(prop[0]);
//            this.Position = new Coordinate(int.Parse(prop[1]), int.Parse(prop[2]));
//            this.TeamId = int.Parse(prop[3]);
//            this.State = int.Parse(prop[4]);
//            this.Value = int.Parse(prop[5]);
//            this.Turn = Game.Turn;
//            this.Weapon = 0;
//            this.Target = null;
//            this.HasWarnedAvenge = false;
//        }

//        public void Update(Buster b)
//        {
//            this.Position = b.Position;
//            this.State = b.State;
//            this.Value = b.Value;
//            this.Turn = Game.Turn;
//            if (this.Weapon > 0)
//            {
//                this.Weapon--;
//            }
//            if(this.State == 2 && b.State != 2 )
//            {
//                // I have been freed, no more warning
//                this.HasWarnedAvenge = false;
//            }
//        }

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
//                Coordinate basePosition = new Coordinate(
//                                        Game.MyTeamId == 0 ? 0 : Constants.XMax,
//                                        Game.MyTeamId == 0 ? 0 : Constants.YMax);
//                double baseDistance = basePosition.GetDistanceTo(this.Position);
//                if (baseDistance < Constants.BaseRadius)
//                {
//                    return "RELEASE";
//                }
//                else
//                {
//                    return "MOVE " + basePosition.ToString();
//                }
//            }
//            else
//            {      
//                Ghost weakest = this.FindWeakestGhost();
//                Buster carryingEnnemy = this.FindClosestCarryingEnnemy();
//                // If ennemy on range, get his ghost
//                if (carryingEnnemy != null)
//                {
//                    if (this.Weapon == 0)
//                    {
//                        this.Weapon = Constants.ReloadTurns;
//                        return "STUN " + carryingEnnemy.Id;
//                    }
//                }

//                // Have to find what to do
//                if (this.Target != null)
//                {
//                    // Go there !
//                    Console.Error.WriteLine("Go avenge !");
//                    return "MOVE " + this.Target.Position.ToString();
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
//                            this.Weapon = Constants.ReloadTurns;
//                            return "STUN " + carryingEnnemy.Id;
//                        }
//                        else
//                        {
//                            Coordinate ennemyBasePosition = new Coordinate(
//                                        Game.MyTeamId == 0 ? Constants.XMax : 0,
//                                        Game.MyTeamId == 0 ? Constants.YMax : 0);
//                            return "MOVE " + ennemyBasePosition.ToString();
//                        }
//                    }
//                }
//                else
//                {
//                    double distance = weakest.Position.GetDistanceTo(this.Position);
//                    if (distance > Constants.BustMax)
//                    {
//                        //Get Closer
//                        Console.Error.WriteLine("Get closer");
//                        return "MOVE " + weakest.Position.ToString();
//                    }
//                    if (distance < Constants.BustMin)
//                    {
//                        //Stay, the Ghost will fear
//                        Console.Error.WriteLine("He will fear me");
//                        return "MOVE " + new Coordinate(
//                                            weakest.Position.x + (Game.MyTeamId == 0 ? -Constants.BustMin : Constants.BustMin) / 2,
//                                            weakest.Position.y + (Game.MyTeamId == 0 ? -Constants.BustMin : Constants.BustMin) / 2
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
//                    result = ImportantCoordinates.TopMiddle;
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

//        public Ghost FindClosestGhost()
//        {
//            double minDistance = double.MaxValue;
//            Ghost final = null;
//            foreach (Ghost ghost in Game.Ghosts.Where(g => g.Turn == Game.Turn).ToList())
//            {
//                double distance = ghost.Position.GetDistanceTo(this.Position);
//                if (distance < Constants.VisionRange && distance < minDistance)
//                {
//                    final = ghost;
//                    minDistance = distance;
//                }
//            }
//            return final;
//        }

//        public Ghost FindWeakestGhost()
//        {
//            int minStamina = int.MaxValue;
//            Ghost final = null;
//            foreach (Ghost ghost in Game.Ghosts.Where(g => g.Turn == Game.Turn).ToList())
//            {                
//                if (ghost.State < minStamina)
//                {
//                    final = ghost;
//                    minStamina = ghost.State;
//                }
//            }
//            return final;
//        }

//        public Buster FindClosestCarryingEnnemy()
//        {
//            double minDistance = double.MaxValue;
//            Buster final = null;
//            foreach (Buster buster in Game.EnnemyBusters.Where(b => b.Turn == Game.Turn && b.State == 1).ToList())
//            {
//                double distance = buster.Position.GetDistanceTo(this.Position);
//                if (distance < Constants.BustMax && distance < minDistance)
//                {
//                    final = buster;
//                    minDistance = distance;
//                }
//            }
//            return final;
//        }

//        public void WarnClosestToAvenge()
//        {
//            // Check if I am stunned !
//            if (this.State == 2 && !this.HasWarnedAvenge)
//            {                
//                foreach (Buster ennemyBuster in Game.EnnemyBusters.Where(b => b.Turn == Game.Turn && //Updated
//                                                                    b.State == 1 && //Carrying
//                                                                    this.Position.GetDistanceTo(b.Position) < Constants.VisionRange //Around me
//                                                                    ).ToList())
//                {
//                    // Warn Him to our closest friend !
//                    double minDistance = double.MaxValue;
//                    Buster closestFriend = null;
//                    foreach (Buster myFriend in Game.MyBusters.Where(b => b.Id != this.Id && b.Target == null && b.Weapon < 8 && b.State != 2).ToList())
//                    {
//                        double distance = myFriend.Position.GetDistanceTo(ennemyBuster.Position);
//                        if (distance < Constants.BustMax && distance < minDistance)
//                        {
//                            Console.Error.WriteLine("My best friend is " + myFriend.Id);
//                            closestFriend = myFriend;
//                            minDistance = distance;
//                        }
//                    }
//                    if (closestFriend != null)
//                    {
//                        this.HasWarnedAvenge = true;
//                        closestFriend.Target = ennemyBuster;
//                    }
//                }
//            }
//        }
//    }

//    /**
//     * Send your busters out into the fog to trap ghosts and bring them home!
//     **/
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
//            Game.Turn = 0;
//            Game.BustersPerPlayer = int.Parse(Console.ReadLine()); // the amount of busters you control
//            Game.GhostCount = int.Parse(Console.ReadLine()); // the amount of ghosts on the map
//            Game.MyTeamId = int.Parse(Console.ReadLine()); // if this is 0, your base is on the top left of the map, if it is one, on the bottom right
//            Game.Ghosts = new List<Ghost>();
//            Game.MyBusters = new List<Buster>();
//            Game.EnnemyBusters = new List<Buster>();
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