using System;
using System.Linq;
using System.IO;
using System.Text;
using System.Collections;
using System.Collections.Generic;

/**
 * Made with love by AntiSquid, Illedan and Wildum.
 * You can help children learn to code while you participate by donating to CoderDojo.
 **/

namespace BottersOfTheGalaxy
{
    public class Position
    {
        public int X { get; set; }
        public int Y { get; set; }

        public double GetDistance(Position other) => Math.Sqrt((this.X - other.X) * (this.X - other.X) + (this.Y - other.Y) * (this.Y - other.Y));
    }
    
    public class Bush : Position
    {
        public int Radius { get; set; }
    }

    public class Spawn : Position
    {
        public int Radius { get; set; }
    }

    public abstract class Unit : Position
    {
        public int Id { get; set; }
        public int TeamId { get; set; } // l'équipe à laquelle elle appartient
        public int AttackRange { get; set; } // la distance depuis laquelle une unité peut attaquer
        public int Health { get; set; } // la quantité de dégâts qu'une unité peut subir avant de mourir
        public int Mana { get; set; } // la quantité de mana qu'une unité a à sa disposition pour pouvoir effectuer ses sorts
        public int AttackDamage { get; set; } // la quantité de dégâts qu'une unité peut effectuer avec une commande ATTACK
        public int MovementSpeed { get; set; } // la distance qu'une unité peut parcourir en un tour. Une entité s'arrête lorsqu'elle arrive et n'ira pas plus loin avant le prochain tour.
        public int MaxHealth { get; set; }
        public int MaxMana { get; set; }
        public abstract string UnitType { get; }

        public virtual double AttackSpeed => 0.2;
        public virtual int Gold => IsRange ? 50 : 30;

        public double GetRangeAttackSpeed(double dist) => this.AttackSpeed * dist / this.AttackRange;
        public bool IsRange => this.AttackRange > 150;
        public double GetDeplacementSpeed(double dist) => dist / this.MovementSpeed;
        public bool IsAttackProcessed(double attackTime) => attackTime <= 1;
        // TODO : fonctions pour marcher jusqu'à range et tirer etc...
        public bool CanAttack(Unit unit) => this.GetDistance(unit) < this.AttackRange;
    }

    // Can't move
    public class Tower : Unit
    {
        public override string UnitType => "TOWER";
    }

    public class Minion : Unit
    {
        public override string UnitType => "UNIT";

        public bool Aggro(Hero attacker) => this.GetDistance(attacker) < 300;
    }

    public class NeutralMonster : Unit
    {
        public override string UnitType => "GROOT";

    }

    public abstract class Hero : Unit
    {
        public override string UnitType => "HERO";

        public override double AttackSpeed => 0.1;
        public override int Gold => 300;

        // Run and hit towards our base
        public Position RunAndHit(Unit targetUnit)
        {
            Position targetPosition = new Position();

            double distToEnnemy = this.GetDistance(targetUnit);

            if(distToEnnemy > this.AttackRange)
            {
                // Cannot attack, so has to move towards
            }
            else
            {
                // Run back to limit and attack

            }

            return targetPosition;
        }
    }

    public class Deadpool : Hero
    {
    }

    public class DoctorStrange : Hero
    {
    }

    public class Hulk : Hero
    {
    }

    public class Ironman : Hero
    {
    }

    public class Valkyrie : Hero
    {
    }

    public class Board
    {
        public const int Height = 750;
        public const int Width = 1920;
        public const int RespawnTimer = 15;

        public int MyTeamId = -1;
        public int MyGold = 0;
        public int EnnemyGold = 0;

        public List<Bush> Bushes = new List<Bush>();
        public List<Spawn> Spawns = new List<Spawn>();
        public List<Unit> Units = new List<Unit>();

        public List<Unit> MyUnits => this.Units.Where(u => u.TeamId == this.MyTeamId).ToList();
        public List<Unit> EnnemyUnits => this.Units.Where(u => u.TeamId != this.MyTeamId).ToList();

        public void Clear()
        {
            this.Bushes.Clear();
            this.Spawns.Clear();
            this.Units.Clear();
        }
    }

    public class Game
    {
        public static void Main(string[] args)
        {
            Board board = new Board();

            // Init board
            string[] inputs;
            board.MyTeamId = int.Parse(Console.ReadLine());
            int bushAndSpawnPointCount = int.Parse(Console.ReadLine()); // usefrul from wood1, represents the number of bushes and the number of places where neutral units can spawn
            for (int i = 0; i < bushAndSpawnPointCount; i++)
            {
                inputs = Console.ReadLine().Split(' ');
                string entityType = inputs[0]; // BUSH, from wood1 it can also be SPAWN
                int x = int.Parse(inputs[1]);
                int y = int.Parse(inputs[2]);
                int radius = int.Parse(inputs[3]);
                if (entityType == "BUSH")
                {
                    board.Bushes.Add(new Bush() { X = x, Y = y, Radius = radius });
                }
                else
                {
                    board.Spawns.Add(new Spawn() { X = x, Y = y, Radius = radius });
                }                
            }

            int itemCount = int.Parse(Console.ReadLine()); // useful from wood2
            for (int i = 0; i < itemCount; i++)
            {
                inputs = Console.ReadLine().Split(' ');
                string itemName = inputs[0]; // contains keywords such as BRONZE, SILVER and BLADE, BOOTS connected by "_" to help you sort easier
                int itemCost = int.Parse(inputs[1]); // BRONZE items have lowest cost, the most expensive items are LEGENDARY
                int damage = int.Parse(inputs[2]); // keyword BLADE is present if the most important item stat is damage
                int health = int.Parse(inputs[3]);
                int maxHealth = int.Parse(inputs[4]);
                int mana = int.Parse(inputs[5]);
                int maxMana = int.Parse(inputs[6]);
                int moveSpeed = int.Parse(inputs[7]); // keyword BOOTS is present if the most important item stat is moveSpeed
                int manaRegeneration = int.Parse(inputs[8]);
                int isPotion = int.Parse(inputs[9]); // 0 if it's not instantly consumed
            }

            // Game loop
            while (true)
            {
                // Refresh board
                board.Clear();

                board.MyGold = int.Parse(Console.ReadLine());
                board.EnnemyGold = int.Parse(Console.ReadLine());
                int roundType = int.Parse(Console.ReadLine()); // a positive value will show the number of heroes that await a command
                int entityCount = int.Parse(Console.ReadLine());
                for (int i = 0; i < entityCount; i++)
                {
                    inputs = Console.ReadLine().Split(' ');
                    int unitId = int.Parse(inputs[0]);
                    int team = int.Parse(inputs[1]);
                    string unitType = inputs[2]; // UNIT, HERO, TOWER, can also be GROOT from wood1
                    int x = int.Parse(inputs[3]);
                    int y = int.Parse(inputs[4]);
                    int attackRange = int.Parse(inputs[5]);
                    int health = int.Parse(inputs[6]);
                    int maxHealth = int.Parse(inputs[7]);
                    int shield = int.Parse(inputs[8]); // useful in bronze
                    int attackDamage = int.Parse(inputs[9]);
                    int movementSpeed = int.Parse(inputs[10]);
                    int stunDuration = int.Parse(inputs[11]); // useful in bronze
                    int goldValue = int.Parse(inputs[12]);
                    int countDown1 = int.Parse(inputs[13]); // all countDown and mana variables are useful starting in bronze
                    int countDown2 = int.Parse(inputs[14]);
                    int countDown3 = int.Parse(inputs[15]);
                    int mana = int.Parse(inputs[16]);
                    int maxMana = int.Parse(inputs[17]);
                    int manaRegeneration = int.Parse(inputs[18]);
                    string heroType = inputs[19]; // DEADPOOL, VALKYRIE, DOCTOR_STRANGE, HULK, IRONMAN
                    int isVisible = int.Parse(inputs[20]); // 0 if it isn't
                    int itemsOwned = int.Parse(inputs[21]); // useful from wood1

                    Unit u;

                    switch(unitType)
                    {
                        case "UNIT":
                            u = new Minion();
                            break;
                        case "HERO":
                            switch(heroType)
                            {
                                case "DEADPOOL":
                                    u = new Deadpool();
                                    break;
                                case "VALKYRIE":
                                    u = new Valkyrie();
                                    break;
                                case "DOCTOR_STRANGE":
                                    u = new DoctorStrange();
                                    break;
                                case "HULK":
                                    u = new Hulk();
                                    break;
                                case "IRONMAN":
                                default:
                                    u = new Ironman();
                                    break;
                            }
                            //u.CountDown123
                            u.Mana = mana;
                            u.MaxMana = maxMana;
                            //u.ManaRegen
                            //u.IsVisible
                            //u.Items
                            break;
                        case "TOWER":
                            u = new Tower();
                            break;
                        case "GROOT":
                        default:
                            u = new NeutralMonster();
                            break;
                    }

                    u.Id = unitId;
                    u.TeamId = team;
                    u.X = x;
                    u.Y = y;
                    u.AttackRange = attackRange;
                    u.Health = health;
                    u.MaxHealth = maxHealth;
                    //u.Shield = 
                    u.AttackDamage = attackDamage;
                    u.MovementSpeed = movementSpeed;
                    //u.StunDuration
                    //u.Gold

                    board.Units.Add(u);
                }

                // Write an action using Console.WriteLine()
                // To debug: Console.Error.WriteLine("Debug messages...");


                // If roundType has a negative value then you need to output a Hero name, such as "DEADPOOL" or "VALKYRIE".
                // Else you need to output roundType number of any valid action, such as "WAIT" or "ATTACK unitId"
                if(roundType < 0)
                {
                    // Draft
                    Console.WriteLine("IRONMAN");
                }
                else
                {
                    // Play
                    Hero myHero = board.MyUnits.OfType<Hero>().FirstOrDefault();

                    // Get closest ennemy
                    double minDistance = double.MaxValue;
                    Unit target = null;
                    foreach(Unit ennemyUnit in board.EnnemyUnits.Where(u => !(u is Tower)))
                    {
                        double distance = myHero.GetDistance(ennemyUnit);
                        if(distance < minDistance)
                        {
                            minDistance = distance;
                            target = ennemyUnit;
                        }
                    }

                    // Don't get in range
                    bool inEnnemyRange = false;
                    foreach (Unit ennemyUnit in board.EnnemyUnits)
                    {
                        if(ennemyUnit.CanAttack(myHero))
                        {
                            inEnnemyRange = true;
                            break;
                        }
                    }

                    // Attack
                    if(inEnnemyRange || target == null)
                    {
                        Console.WriteLine($"MOVE {(board.MyTeamId == 0 ? 200 : 1720)} 590");
                    }     
                    else
                    {
                        Console.WriteLine($"ATTACK {target.Id}");
                    }
                }                
            }
        }
    }
}
