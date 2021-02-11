using System;
using System.Diagnostics;

namespace CodersStrikesBack
{
    #region Game
    public class Point
    {
        public double X { get; set; }
        public double Y { get; set; }

        public double Distance(Point p)
        {
            return Math.Sqrt(this.Distance2(p));
        }
        public double Distance2(Point p)
        {
            return (this.X - p.X) * (this.X - p.X) + (this.Y - p.Y) * (this.Y - p.Y);
        }
        
        public Point Closest(Point a, Point b)
        {
            double da = b.Y - a.Y;
            double db = a.X - b.X;
            double c1 = da * a.X + db * a.Y;
            double c2 = -db * this.X + da * this.Y;
            double det = da * da + db * db;
            double cx = 0;
            double cy = 0;

            if (det != 0)
            {
                cx = (da * c1 - db * c2) / det;
                cy = (da * c2 + db * c1) / det;
            }
            else
            {
                // Le point est déjà sur la droite
                cx = this.X;
                cy = this.Y;
            }

            return new Point() { X = cx, Y = cy };
        }
    }

    public class Pod : Point
    {
        public double Angle { get; set; }
        public bool Shield { get; set; }
        public double VX { get; set; }
        public double VY { get; set; }
        public int TimeOut { get; set; }
        public int Player { get; set; }

        public CheckPoint CheckPoint { get; set; }

        private double GetAngle(Point p)
        {
            double d = this.Distance(p);
            double dx = (p.X - this.X) / d;
            double dy = (p.Y - this.Y) / d;

            // Trigonométrie simple. On multiplie par 180.0 / PI pour convertir en degré.
            double a = Math.Acos(dx) * 180.0 / Math.PI;

            // Si le point qu'on veut est en dessus de nous, il faut décaler l'angle pour qu'il soit correct.
            if (dy < 0)
            {
                a = 360.0 - a;
            }

            return a;
        }

        private double DiffAngle(Point p)
        {
            double a = this.GetAngle(p);

            // Pour connaitre le sens le plus proche, il suffit de regarder dans les 2 sens et on garde le plus petit
            // Les opérateurs ternaires sont la uniquement pour éviter l'utilisation d'un operateur % qui serait plus lent
            double right = this.Angle <= a ? a - this.Angle : 360.0 - this.Angle + a;
            double left = this.Angle >= a ? this.Angle - a : this.Angle + 360.0 - a;

            if (right < left)
            {
                return right;
            }
            else
            {
                // On donne un angle négatif s'il faut tourner à gauche
                return -left;
            }
        }

        private void Rotate(Point p)
        {
            double a = this.DiffAngle(p);

            // On ne peut pas tourner de plus de 18° en un seul tour
            if (a > 18.0)
            {
                a = 18.0;
            }
            else if (a < -18.0)
            {
                a = -18.0;
            }

            this.Angle += a;

            // L'opérateur % est lent. Si on peut l'éviter, c'est mieux.
            if (this.Angle >= 360.0)
            {
                this.Angle = this.Angle - 360.0;
            }
            else if (this.Angle < 0.0)
            {
                this.Angle += 360.0;
            }
        }

        private void Boost(int thrust)
        {
            // N'oubliez pas qu'un pod qui a activé un shield ne peut pas accélérer pendant 3 tours
            if (this.Shield)
            {
                return;
            }

            // Conversion de l'angle en radian
            double ra = this.Angle * Math.PI / 180.0;

            // Trigonométrie
            this.VX += Math.Cos(ra) * thrust;
            this.VY += Math.Sin(ra) * thrust;
        }

        private void Move(double t)
        {
            this.X += this.VX * t;
            this.Y += this.VY * t;
        }

        private void End()
        {
            this.X = Math.Round(this.X);
            this.Y = Math.Round(this.Y);
            this.VX = Math.Truncate(this.VX * 0.85);
            this.VY = Math.Truncate(this.VY * 0.85);

            // N'oubliez pas que le timeout descend de 1 chaque tour. Il revient à 100 quand on passe par un checkpoint
            this.TimeOut -= 1;
        }

        public void Play(Point p, int thrust)
        {
            this.Rotate(p);
            this.Boost(thrust);
            this.Move(1.0);
            this.End();
        }

        public Pod Copy()
        {
            return new Pod()
            {
                X = this.X,
                Y = this.Y,
                Angle = this.Angle,
                Shield = this.Shield,
                VX = this.VX,
                VY = this.VY,
                TimeOut = this.TimeOut,
                Player = this.Player
            };
        }
    }

    public class CheckPoint : Point
    {
    }

    public class Board
    {
        public int MyTeamId = 1;
        public const int PodsNumber = 1;
        public const int PlayersNumber = 2;
        public Pod[] Pods = new Pod[PodsNumber * PlayersNumber];
        public Pod[] MyPods = new Pod[PodsNumber];

        public Board Copy()
        {
            Board copy = new Board();
            int count = 0;
            for(int i = 0; i < PodsNumber * PlayersNumber; ++i)
            {
                copy.Pods[i] = this.Pods[i].Copy();
                if(copy.Pods[i].Player == this.MyTeamId)
                {
                    copy.MyPods[count++] = copy.Pods[i];
                }
            }
            return copy;
        }
    }

    #endregion Game

    #region Genetic
    public static class GeneticConstants
    {
        public const int GenomeSize = 6;
        public const int PopulationSize = 20;
        public const int GenerationNumber = 1000;
        public const double PercentageWinChance = 0.75;
    }

    public class Gene
    {
        public double Thrust1 { get; set; }
        public double Thrust2 { get; set; }
        public double Direction1 { get; set; }
        public double Direction2 { get; set; }
        public double Shield1 { get; set; }
        public double Shield2 { get; set; }

        public Point Target { get; set; }

        public void Randomize(Random random)
        {
            this.Thrust1 = random.NextDouble();
            this.Thrust2 = random.NextDouble();
            this.Direction1 = random.NextDouble();
            this.Direction2 = random.NextDouble();
            this.Shield1 = random.NextDouble();
            this.Shield2 = random.NextDouble();
        }

        public void Copy(Gene gene)
        {
            this.Thrust1 = gene.Thrust1;
            this.Thrust2 = gene.Thrust2;
            this.Direction1 = gene.Direction1;
            this.Direction2 = gene.Direction2;
            this.Shield1 = gene.Shield1;
            this.Shield2 = gene.Shield2;
        }

        public int ComputeThrust(double thrust)
        {

        }
    }

    public class Genome
    {
        public Gene[] Genes = new Gene[GeneticConstants.GenomeSize];
        public int Score;

        public Genome()
        {
            for(int i = 0; i < GeneticConstants.GenomeSize; ++i)
            {
                this.Genes[i] = new Gene();
            }
        }

        public void Randomize(Random random)
        {
            for(int i = 0; i < GeneticConstants.GenomeSize; ++i)
            {
                this.Genes[i].Randomize(random);
            }
        }

        public void Evaluate(Board board)
        {
            // Simulate on board
            Board simulationBoard = board.Copy();

            for(int i = 0; i < GeneticConstants.GenomeSize; ++i)
            {
                 board.MyPods[0].Play(board.MyPods[0].CheckPoint, (int)this.Genes[i].ComputeThrust(this.Genes[i].Thrust1));
            }
            
            // Score
            this.Score = 1;
        }

        public void Mutate()
        {
            this.Score = 0;
        }
    }
    #endregion Genetic

    class Program
    {
        static void Main(string[] args)
        {
            // Genetic init
            Random random = new Random();            

            Genome[] population = new Genome[GeneticConstants.PopulationSize];
            for (int i = 0; i < GeneticConstants.PopulationSize; ++i)
            {
                population[i] = new Genome();
            }

            int[] winners = new int[GeneticConstants.PopulationSize / 2];
            int[] losers = new int[GeneticConstants.PopulationSize / 2];
            int[] tournament = new int[GeneticConstants.PopulationSize];


            // Game init
            Board board;

            // Game loop
            string[] inputs;

            while (true)
            {
                board = new Board();
                inputs = Console.ReadLine().Split(' ');
                int x = int.Parse(inputs[0]);
                int y = int.Parse(inputs[1]);                
                int nextCheckpointX = int.Parse(inputs[2]); // x position of the next check point
                int nextCheckpointY = int.Parse(inputs[3]); // y position of the next check point
                int nextCheckpointDist = int.Parse(inputs[4]); // distance to the next checkpoint
                int nextCheckpointAngle = int.Parse(inputs[5]); // angle between your pod orientation and the direction of the next checkpoint
                inputs = Console.ReadLine().Split(' ');
                int opponentX = int.Parse(inputs[0]);
                int opponentY = int.Parse(inputs[1]);
                
                // Create Pods
                Pod myPod = new Pod() { Player = 1, X = x, Y = y, Angle = nextCheckpointAngle, CheckPoint = new CheckPoint() { X = nextCheckpointX, Y = nextCheckpointY } };
                Pod opponentPod = new Pod() { Player = 0, X = opponentX, Y = opponentY};

                // Feed board
                board.Pods[0] = myPod;
                board.Pods[1] = opponentPod;
                board.MyPods[0] = myPod;
            }

            #region Genetic Algo
            Stopwatch watch = new Stopwatch();
            watch.Start();

            // Init
            for(int i = 0; i < GeneticConstants.PopulationSize; ++i)
            {
                tournament[i] = i;
                population[i].Randomize(random);
            }

            for(int g = 0; g < GeneticConstants.GenerationNumber; ++g)
            {
                // Evaluation
                for(int i = 0; i < GeneticConstants.PopulationSize; ++i)
                {
                    if(population[i].Score == 0)
                    {
                        population[i].Evaluate(board);
                    }
                }

                // Selection par tournoi
                // Shuffle
                for(int t = 0; t < GeneticConstants.PopulationSize; ++t)
                {
                    int t1 = random.Next(GeneticConstants.PopulationSize);
                    int t2 = random.Next(GeneticConstants.PopulationSize);
                    int tmp = tournament[t1];
                    tournament[t1] = tournament[t2];
                    tournament[t2] = tmp;
                }
                // Play tournament
                for(int t = 0; t < GeneticConstants.PopulationSize; t+=2)
                {
                    // t against t+1
                    if( (population[tournament[t]].Score  >= population[tournament[t+1]].Score && random.NextDouble() <= GeneticConstants.PercentageWinChance)
                        || (population[tournament[t]].Score < population[tournament[t + 1]].Score && random.NextDouble() < GeneticConstants.PercentageWinChance) )
                    {
                        // t win
                        winners[t / 2] = tournament[t];
                        losers[t / 2] = tournament[t+1];
                    }
                    else
                    {
                        // t+1 win
                        winners[t / 2] = tournament[t + 1];
                        losers[t / 2] = tournament[t];
                    }
                }

                // Croisements
                // Mutations
                for (int t = 0; t < GeneticConstants.PopulationSize/2; t+=2)
                {
                    // winners t with t + 1
                    for(int i = 0; i < GeneticConstants.GenomeSize/2; ++i)
                    {
                        population[losers[t]].Genes[i] = population[winners[t]].Genes[i];
                        population[losers[t + 1]].Genes[i] = population[winners[t + 1]].Genes[i];
                    }
                    for (int i = GeneticConstants.GenomeSize/2; i < GeneticConstants.GenomeSize; ++i)
                    {
                        population[losers[t + 1]].Genes[i] = population[winners[t]].Genes[i];
                        population[losers[t]].Genes[i] = population[winners[t + 1]].Genes[i];
                    }
                    population[losers[t]].Mutate();
                    population[losers[t + 1]].Mutate();
                }
            }

            // Evaluation finale + tri
            for (int i = 0; i < GeneticConstants.PopulationSize; ++i)
            {
                if (population[i].Score == 0)
                {
                    population[i].Evaluate(board);
                }
            }

            #endregion

            Genome best = population[0];
            for(int i = 1; i < GeneticConstants.PopulationSize; ++i)
            {
                if (population[i].Score > best.Score)
                    best = population[i];
            }

            Gene gene = best.Genes[0];
            Console.WriteLine($"{gene.Target.X} {gene.Target.Y} {gene.Thrust1}");

            watch.Stop();
            Console.Error.WriteLine($"{watch.ElapsedMilliseconds} milliseconds");          
        }
    }
}
