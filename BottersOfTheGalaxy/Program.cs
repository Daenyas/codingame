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
    #region Board

    public class Item
    {
        public string Name { get; set; }
        public int Cost { get; set; }
        public int Damage { get; set; }
        public int Health { get; set; }
        public int MaxHealth { get; set; }
        public int Mana { get; set; }
        public int MaxMana { get; set; }
        public int MoveSpeed { get; set; }
        public int ManaRegeneration { get; set; }
        public bool IsPotion { get; set; }   
    }
    

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
        public int BaseAttackDamage { get; set; } // la quantité de dégâts qu'une unité peut effectuer avec une commande ATTACK
        public int BaseMovementSpeed { get; set; } // la distance qu'une unité peut parcourir en un tour. Une entité s'arrête lorsqu'elle arrive et n'ira pas plus loin avant le prochain tour.
        public int MaxHealth { get; set; }
        public int MaxMana { get; set; }
        public abstract string UnitType { get; }

        public virtual double AttackSpeed => 0.2;
        public virtual int Gold => IsRange ? 50 : 30;

        public virtual int AttackDamage => this.BaseAttackDamage;
        public virtual int MovementSpeed => this.BaseMovementSpeed;

        public double GetRangeAttackSpeed(double dist) => this.AttackSpeed * dist / this.AttackRange;
        public bool IsRange => this.AttackRange > 150;
        public double GetDeplacementSpeed(double dist) => dist / this.MovementSpeed;
        public bool IsAttackProcessed(double attackTime) => attackTime <= 1;
        public bool CanAttack(Position position) => this.GetDistance(position) <= this.AttackRange;

        public bool CanMoveThenAttack(Position position)
        {
            double distanceToBeInRange = this.GetDistance(position) - this.AttackRange;
            double speedToBeInRange = this.GetDeplacementSpeed(distanceToBeInRange);
            double attackTime = speedToBeInRange + this.AttackSpeed;
            return this.IsAttackProcessed(attackTime);
        }
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
        public List<Item> Items = new List<Item>();

        public override string UnitType => "HERO";

        public override double AttackSpeed => 0.1;
        public override int Gold => 300;
        public override int AttackDamage => this.BaseAttackDamage + Items.Sum(i => i.Damage);
        public override int MovementSpeed => this.BaseMovementSpeed + Items.Sum(i => i.MoveSpeed);
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
        public int RoundType = 0;

        public List<Bush> Bushes = new List<Bush>();
        public List<Spawn> Spawns = new List<Spawn>();
        public List<Unit> Units = new List<Unit>();
        public List<Item> Items = new List<Item>();

        // Helpers
        public int MyTeamFactor => (2 * this.MyTeamId - 1); // -1 when 0 (left) +1 when 1 (right)

        public List<Unit> MyUnits => this.Units.Where(u => u.TeamId == this.MyTeamId).ToList();
        public List<Unit> EnnemyUnits => this.Units.Where(u => u.TeamId != this.MyTeamId).ToList();

        public List<Minion> MyMinions => this.Units.OfType<Minion>().Where(u => u.TeamId == this.MyTeamId).ToList();
        public List<Minion> EnnemyMinions => this.Units.OfType<Minion>().Where(u => u.TeamId != this.MyTeamId).ToList();

        public Tower MyTower => this.Units.OfType<Tower>().First(u => u.TeamId == this.MyTeamId);
        public Tower EnnemyTower => this.Units.OfType<Tower>().First(u => u.TeamId != this.MyTeamId);

        public Hero MyHero => this.Units.OfType<Hero>().First(u => u.TeamId == this.MyTeamId);
        public Hero EnnemyHero => this.Units.OfType<Hero>().First(u => u.TeamId != this.MyTeamId);

        public void Clear()
        {
            this.Bushes.Clear();
            this.Spawns.Clear();
            this.Units.Clear();
        }
    }

    #endregion 

    public class Game
    {
        public static void Main(string[] args)
        {
            Board board = InitBoard();           

            // Game loop
            while (true)
            {
                RefreshBoard(board);

                // Write an action using Console.WriteLine()
                // To debug: Console.Error.WriteLine("Debug messages...");

                // If roundType has a negative value then you need to output a Hero name, such as "DEADPOOL" or "VALKYRIE".
                // Else you need to output roundType number of any valid action, such as "WAIT" or "ATTACK unitId"
                if(board.RoundType < 0)
                {
                    Draft(board);                    
                }
                else
                {
                    // Play
                    Play(board);                    
                }                
            }
        }

        private static void RefreshBoard(Board board)
        {
            // Clear board
            board.Clear();

            string[] inputs;

            board.MyGold = int.Parse(Console.ReadLine());
            board.EnnemyGold = int.Parse(Console.ReadLine());
            board.RoundType = int.Parse(Console.ReadLine()); // a positive value will show the number of heroes that await a command

            // Units
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

                switch (unitType)
                {
                    case "UNIT":
                        u = new Minion();
                        break;
                    case "HERO":
                        switch (heroType)
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
                u.BaseAttackDamage = attackDamage;
                u.BaseMovementSpeed = movementSpeed;
                //u.StunDuration
                //u.Gold

                board.Units.Add(u);
            }
        }

        private static Board InitBoard()
        {
            Board board = new Board();

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

                board.Items.Add(new Item
                {
                    Name = itemName,
                    Cost = itemCost,
                    Damage = damage,
                    Health = health,
                    MaxHealth = maxHealth,
                    Mana = mana,
                    MaxMana = maxMana,
                    MoveSpeed = moveSpeed,
                    ManaRegeneration = manaRegeneration,
                    IsPotion = isPotion == 1
                });
            }

            return board;
        }

        private static void Draft(Board board)
        {
            // Draft
            Console.WriteLine("IRONMAN");
        }

        private static void Play(Board board)
        {
            // We want more damage to clean easier so lets take damage item whenever we can, but not inefficient ones
            Item targettedItem = board.Items.OrderByDescending(i => i.Damage).FirstOrDefault(i=> i.Cost <= board.MyGold && i.Damage > 20);
            if(targettedItem != null)
            {
                Console.WriteLine($"BUY {targettedItem.Name}");
                return;
            }

            // Where to go ?
            Position targettedPosition = new Position();
            
            // Target first one by distance
            Unit closestEnnemy = board.EnnemyMinions.OrderBy(eu => board.MyHero.GetDistance(eu)).FirstOrDefault() as Unit ?? board.EnnemyTower;

            // Move to max range
            targettedPosition.X = closestEnnemy.X + board.MyTeamFactor * (board.MyHero.AttackRange - 1);
            targettedPosition.Y = closestEnnemy.Y;

            // Don't go above my minions (when wave is cleared)
            while(board.MyMinions.Count > 0 && board.MyMinions.All(mm => mm.GetDistance(closestEnnemy) >= targettedPosition.GetDistance(closestEnnemy)))
            {
                Console.Error.WriteLine($"None of my minion is closer to closestEnnemy");
                targettedPosition.X += board.MyTeamFactor * 10;
            }
            
            // Don't get in ennemy Hero range if has less range than me
            while(board.EnnemyHero.CanAttack(targettedPosition) && board.EnnemyHero.AttackRange < board.MyHero.AttackRange)
            {
                Console.Error.WriteLine($"{board.EnnemyHero.Id} can attack my hero on {targettedPosition.X}");
                targettedPosition.X += board.MyTeamFactor * 10;
            }

            // Don't get in ennemy tower range if i have not at least 2 minions there
            while(board.EnnemyTower.CanAttack(targettedPosition) && board.MyMinions.Count(mm => board.EnnemyTower.CanAttack(mm)) < 2)
            {
                Console.Error.WriteLine($"{board.EnnemyTower.Id} can attack my hero on {targettedPosition.X}");
                targettedPosition.X += board.MyTeamFactor * 1;
            }

            // When done, first target ennemy we can kill
            Unit target = board.EnnemyUnits.FirstOrDefault(u => u.Health < board.MyHero.AttackDamage);

            // Else first try tower if in range
            if (target == null)
            {                
                target = board.EnnemyTower.GetDistance(targettedPosition) < board.MyHero.AttackRange ? board.EnnemyTower : null;
            }
            // Else get lowest hp in range
            if (target == null)
            {             
                target = board.EnnemyUnits.OrderBy(u => u.Health).FirstOrDefault(u => u.GetDistance(targettedPosition) < board.MyHero.AttackRange);
            }

            if (target != null)
            {
                // Move Attack
                Console.WriteLine($"MOVE_ATTACK {targettedPosition.X} {targettedPosition.Y} {target.Id}");
            }
            else
            {
                // Just Move
                Console.WriteLine($"MOVE {targettedPosition.X} {targettedPosition.Y}");
            }
        }
    }
}
