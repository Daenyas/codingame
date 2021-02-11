//using System;
//using System.Collections.Generic;
//using System.Linq;

//namespace FantasticBits
//{

//    #region Physic classes
//    public class Position
//    {
//        #region Properties
//        public int X { get; set; }
//        public int Y { get; set; }
//        #endregion

//        public double DistanceTo(Position position)
//        {
//            return Math.Sqrt((this.X - position.X) * (this.X - position.X) + (this.Y - position.Y) * (this.Y - position.Y));
//        }
//    }

//    public class Velocity
//    {
//        #region Properties
//        public static readonly int MAX_VALUE = 1000000;
//        public int VX { get; set; }
//        public int VY { get; set; }
//        #endregion
//    }

//    public class Line
//    {
//        #region Properties
//        public double A { get; set; }
//        public double B { get; set; }
//        #endregion
//        public static Line Compute(Position position1, Position position2)
//        {
//            var line = new Line();
//            line.A = ((double)(position1.Y - position2.Y))/(position1.X-position2.X);
//            line.B = ((double) position1.X*position2.Y - position2.X*position1.Y)/(position1.X - position2.X);
//            Console.Error.WriteLine($"{line.A} x + {line.B}");
//            return line;
//        }

//        public static Line Compute(Entity entity1, Entity entity2)
//        {
//            return Line.Compute(entity1.Position, entity2.Position);
//        }

//        public double GetValueAt(double x)
//        {
//            return this.A * x + this.B;
//        }
//    }
//    #endregion

//    #region Game classes
//    public class Board
//    {
//        #region Properties
//        public static readonly int WIDTH = 16001;
//        public static readonly int HEIGHT = 7501;
//        // 0/0 = top left
//        public static readonly int GOALS_LENGTH = 4000;
//        public static readonly int GOALS_CENTER_HEIGHT = 3750;
//        public static readonly int PILAR_RADIUS = 300;
//        public static readonly int TEAM_0_X = 0;
//        public static readonly int TEAM_1_X = 16000;

//        public int WizardNumberPerPlayer { get; set; }
//        public int BludgerNumberTotal { get; set; }
//        public List<Entity> Entities { get; set; }

//        public int MyMana { get; set; }
//        public int EnnemyMana { get; set; }

//        public int MyScore { get; set; }
//        public int EnnemyScore { get; set; }
//        #endregion

//        public Board()
//        {
//            this.Entities = new List<Entity>();
//            this.WizardNumberPerPlayer = 2;
//            this.BludgerNumberTotal = 2;
//        }

//        #region Helpers
//        public List<T> GetAll<T>() where T : Entity
//        {
//            return this.Entities.Where(t => t is T).Cast<T>().ToList();
//        }
//        #endregion
//    }

//    public class Game
//    {
//        #region Properties
//        public static readonly int MAX_TURN = 200;

//        public int Turn { get; set; }
//        public int MyTeamId { get; set; } // 0 : Shoot on right
//        public Board Board { get; set; }
//        #endregion

//        public Game()
//        {
//            this.Board = new Board();
//        }
//    }

//    public abstract class Entity
//    {
//        #region Properties
//        public int Id { get; set; }
//        public Position Position { get; set; }
//        public Velocity Velocity { get; set; }
//        #endregion

//        public Entity()
//        {
//            this.Position = new Position();
//            this.Velocity = new Velocity();
//        }

//        #region Distance functions
//        public T GetClosest<T>(List<T> entities) where T : Entity
//        {
//            T closest = null;
//            double minDistance = double.MaxValue;
//            entities.ForEach(e =>
//            {
//                double distance = this.DistanceTo(e);
//                if(distance < minDistance)
//                {
//                    minDistance = distance;
//                    closest = e;
//                }
//            });
//            return closest;
//        }        

//        public double DistanceTo(Entity entity)
//        {
//            return this.Position.DistanceTo(entity.Position);
//        }

//        public double DistanceTo(Position position)
//        {
//            return this.Position.DistanceTo(position);
//        }
//        #endregion

//        #region Helpers
//        public virtual string GetDescription()
//        {
//            return $"Id: {this.Id} Position: {this.Position.X}/{this.Position.Y} Velocity: {this.Velocity.VX}/{this.Velocity.VY}";
//        }
//        #endregion
//    }

//    public class Snaffle : Entity
//    {
//        #region Properties
//        public static readonly int RADIUS = 200;
//        #endregion

//        public Snaffle() : base()
//        {
//        }
//    }

//    public class Bludger : Entity
//    {
//        #region Properties
//        public static readonly int RADIUS = 150;
//        #endregion

//        public Bludger() : base()
//        {
//        }
//    }

//    public class Wizard : Entity
//    {
//        #region Properties
//        public static readonly int MAX_THRUST = 150;
//        public static readonly int MAX_THROW = 500;

//        public int TeamId { get; set; }
//        public bool Carrying { get; set; }

//        public int LastAccioTurn { get; set; }
//        public int LastFlipendoTurn { get; set; }
//        #endregion

//        public Wizard() : base()
//        {
//            this.LastAccioTurn = 1000;
//            this.LastFlipendoTurn = 1000;
//        }

//        #region Helpers
//        public override string GetDescription()
//        {
//            return base.GetDescription() + $" TeamId: {this.TeamId} Carrying: {this.Carrying} LastAccioTurn: {this.LastAccioTurn}";
//        }
//        #endregion
//    }
//    #endregion

//    #region Strategy classes
//    public interface IStrategy
//    {
//        void Compute(Game game);
//    }
    
//    public class SimpleStrategy : IStrategy
//    {
//        // For each wizard, rush to the closest Snaffle
//        public void Compute(Game game)
//        {
//            List<Wizard> myWizards = game.Board.GetAll<Wizard>().Where(w => w.TeamId == game.MyTeamId).OrderBy(e => e.Id).ToList();
//            myWizards.ForEach(w =>
//            {
//                bool occupied = false;

//                if(w.Carrying)
//                {
//                    // Throw
//                    Console.WriteLine($"THROW {(game.MyTeamId == 0 ? Board.TEAM_1_X : Board.TEAM_0_X) - w.Velocity.VX} {Board.GOALS_CENTER_HEIGHT - w.Velocity.VY} {Wizard.MAX_THROW}");
//                    occupied = true;
//                }

//                if(!occupied && game.Board.MyMana >= 20 && w.LastFlipendoTurn > 3)
//                {
//                    // Flipendo if on direction to ennemy goals
//                    var orderedSnaffles = game.Board.GetAll<Snaffle>().OrderBy(s => s.DistanceTo(w)).ToList();
//                    foreach(var s in orderedSnaffles)
//                    {
//                        // Leave if not in appropriate side 
//                        if (game.MyTeamId == 0 && s.Position.X < w.Position.X)
//                            continue;
//                        if (game.MyTeamId == 1 && s.Position.X > w.Position.X)
//                            continue;

//                        // Compute line
//                        var line = Line.Compute(s, w);
//                        var y = line.GetValueAt(game.MyTeamId == 0 ? Board.TEAM_1_X : Board.TEAM_0_X);

//                        // Compute bounce
//                        if( y < 0 )
//                        {
//                            var line2 = new Line() { A = -line.A, B = -line.B };
//                            y = line2.GetValueAt(game.MyTeamId == 0 ? Board.TEAM_1_X : Board.TEAM_0_X);
//                        }
//                        else if (y > Board.HEIGHT)
//                        {
//                            var line2 = new Line() { A = -line.A, B = 2*Board.HEIGHT - line.B };
//                            y = line2.GetValueAt(game.MyTeamId == 0 ? Board.TEAM_1_X : Board.TEAM_0_X);
//                        }

//                        if ( y > Board.GOALS_CENTER_HEIGHT - (Board.GOALS_LENGTH + Board.PILAR_RADIUS)/2 
//                            && y < Board.GOALS_CENTER_HEIGHT + (Board.GOALS_LENGTH + Board.PILAR_RADIUS) / 2 )
//                        {
//                            Console.WriteLine($"FLIPENDO {s.Id}");
//                            w.LastFlipendoTurn = 0;
//                            game.Board.MyMana -= 20;
//                            occupied = true;
//                            break;
//                        }                      
//                    }                    
//                }

//                if(!occupied && game.Board.MyMana >= 20 && w.LastAccioTurn > 6 )
//                {
//                    // Accio snaffle closest to matching ennemy
//                    Snaffle target = game.Board.GetAll<Snaffle>().OrderBy(s => s.DistanceTo(game.Board.GetAll<Wizard>().First(ew => ew.Id == (w.Id + 2)%4))).FirstOrDefault();
//                    Console.WriteLine($"ACCIO {target.Id}");
//                    w.LastAccioTurn = 0;
//                    game.Board.MyMana -= 15;
//                    occupied = true;
//                }

//                if(!occupied && game.Board.MyMana >= 10)
//                {
//                    // Petrificus
//                    var orderedSnaffles = game.Board.GetAll<Snaffle>().OrderBy(s => s.Position.DistanceTo(new Position() { X = game.MyTeamId == 0 ? Board.TEAM_0_X : Board.TEAM_1_X, Y = Board.GOALS_CENTER_HEIGHT })).ToList();
//                    foreach(var s in orderedSnaffles)
//                    {
//                        double estimatedX = s.Position.X + s.Velocity.VX * ((1 - Math.Pow(0.75, 3) ) / (1 - 0.75));
//                        // Compute position in 3 turns
//                        if( ( game.MyTeamId == 0 && estimatedX < Board.TEAM_0_X ) 
//                            || (game.MyTeamId == 1 && estimatedX > Board.TEAM_1_X) )
//                        {
//                            Console.WriteLine($"PETRIFICUS {s.Id}");
//                            game.Board.MyMana -= 10;
//                            occupied = true;
//                            break;
//                        }                        
//                    }
//                }

//                if(!occupied)
//                {
//                    // Go to closest snaffle
//                    Snaffle target = game.Board.GetAll<Snaffle>().OrderBy(s => s.DistanceTo(w)).FirstOrDefault();
//                    Console.WriteLine($"MOVE {target.Position.X + target.Velocity.VX} {target.Position.Y + target.Velocity.VY} {Wizard.MAX_THRUST}");
//                    occupied = true;
//                }
//            });
//        }
//    }
//    #endregion

//    #region Program
//    public class Program
//    {
//        static void Main(string[] args)
//        {
//            int myTeamId = int.Parse(Console.ReadLine()); // if 0 you need to score on the right of the map, if 1 you need to score on the left
//            Game game = new Game() { Turn = 0, MyTeamId = myTeamId };

//            // game loop
//            while (true)
//            {
//                string[] inputs = Console.ReadLine().Split(' ');
//                int myScore = int.Parse(inputs[0]);
//                int myMagic = int.Parse(inputs[1]);
//                game.Board.MyScore = myScore;
//                game.Board.MyMana = myMagic;
//                inputs = Console.ReadLine().Split(' ');
//                int opponentScore = int.Parse(inputs[0]);
//                int opponentMagic = int.Parse(inputs[1]);
//                game.Board.EnnemyScore = opponentScore;
//                game.Board.EnnemyMana = opponentMagic;

//                // Change this later to keep trace
//                game.Board.Entities.Clear();

//                int entities = int.Parse(Console.ReadLine()); // number of entities still in game
//                for (int i = 0; i < entities; i++)
//                {
//                    inputs = Console.ReadLine().Split(' ');

//                    int entityId = int.Parse(inputs[0]); // entity identifier
//                    string entityType = inputs[1]; // "WIZARD", "OPPONENT_WIZARD" or "SNAFFLE" (or "BLUDGER" after first league)
//                    int x = int.Parse(inputs[2]); // position
//                    int y = int.Parse(inputs[3]); // position
//                    int vx = int.Parse(inputs[4]); // velocity
//                    int vy = int.Parse(inputs[5]); // velocity
//                    int state = int.Parse(inputs[6]); // 1 if the wizard is holding a Snaffle, 0 otherwise

//                    Entity entity = game.Board.Entities.FirstOrDefault(e => e.Id == entityId);
//                    // Add or update specific
//                    if(entity == null)
//                    {
//                        switch (entityType)
//                        {
//                            case "WIZARD":
//                                entity = new Wizard() { TeamId = myTeamId, Carrying = state == 1 };
//                                break;
//                            case "OPPONENT_WIZARD":
//                                entity = new Wizard() { TeamId = 1 - myTeamId, Carrying = state == 1 };
//                                break;
//                            case "SNAFFLE":
//                                entity = new Snaffle();
//                                break;
//                            case "BLUDGER":
//                                entity = new Bludger();
//                                break;
//                            default:
//                                throw new Exception("Unknown entity");
//                        }
//                        game.Board.Entities.Add(entity);
//                    }
//                    else
//                    {
//                        switch (entityType)
//                        {
//                            case "WIZARD":
//                            case "OPPONENT_WIZARD":
//                                ((Wizard)entity).Carrying = state == 1;
//                                ((Wizard)entity).LastAccioTurn++;
//                                ((Wizard)entity).LastFlipendoTurn++;
//                                break;
//                            default:
//                                break;
//                        }
//                    }

//                    // Update common
//                    entity.Id = entityId;
//                    entity.Position.X = x;
//                    entity.Position.Y = y;
//                    entity.Velocity.VX = vx;
//                    entity.Velocity.VY = vy;

//                    Console.Error.WriteLine(entity.GetDescription());
//                }

//                Console.Error.WriteLine("Launching SimpleStrategy");
//                var stragegy = new SimpleStrategy();
//                stragegy.Compute(game);

//                // Write an action using Console.WriteLine()
//                // To debug: Console.Error.WriteLine("Debug messages...");

//                // Edit this line to indicate the action for each wizard (0 ≤ thrust ≤ 150, 0 ≤ power ≤ 500)
//                // i.e.: "MOVE x y thrust" or "THROW x y power"
//                //Console.WriteLine("MOVE 8000 3750 100");    

//                game.Turn++;            
//            }
//        }
//    }
//    #endregion
//}
