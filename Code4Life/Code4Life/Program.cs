using System;
using System.Collections.Generic;
using System.Linq;

enum Molecule
{
    A,
    B,
    C,
    D,
    E    
}

public class Player
{
    public const int SamplesCapacity = 3;
    public const int MoleculesCapacity = 10;

    public int Id { get; set; }
    public string Target { get; set; }
    public int Eta { get; set; }
    public int Score { get; set; }
    public int[] Storage { get; set;}
    public int[] Expertise { get; set; }

    public Player()
    {
        this.Storage = new int[5];
        this.Expertise = new int[5];
    }

    public List<Sample> Samples(Board board) => board.Samples.Where(s => s.CarriedBy == this.Id).ToList();

    public int[] WillFree(Board board)
    {
        // Suppose store betters first
        var samples = this.Samples(board).OrderByDescending(s => s.Health).ToList();
        int[] used = new int[5];
        int[] extraExpertise = new int[5];

        for (int s = 0; s < samples.Count; ++s)
        {
            Sample sample = samples[s];           

            if (sample.CanStore(this, used, extraExpertise))
            {
                for (int i = 0; i < 5; ++i)
                {
                    used[i] += sample.Cost[i] - this.Expertise[i];
                }
                int gain = (int)Enum.Parse(typeof(Molecule), sample.ExpertiseGain);
                extraExpertise[gain] += 1;
            }               
        }

        return used;
    }

    public void Debug(Board board)
    {
        Console.Error.WriteLine($"* Player *");
        Console.Error.WriteLine($"Id : {this.Id}");
        Console.Error.WriteLine($"Target : {this.Target}");
        Console.Error.WriteLine($"Eta : {this.Eta}");
        Console.Error.WriteLine($"Score : {this.Score}");
        string storage = string.Join(",", this.Storage);
        Console.Error.WriteLine($"Storage : {storage}");
        string expertise = string.Join(",", this.Expertise);
        Console.Error.WriteLine($"Expertise : {expertise}");
        Console.Error.WriteLine($"** Samples **");
        this.Samples(board).ForEach(s => s.Debug());
        Console.Error.WriteLine($"* End Player *");
        Console.Error.WriteLine($"");
    }
}

public class Sample
{
    public int Id { get; set; }
    public int CarriedBy { get; set; }
    public int Rank { get; set; }
    public string ExpertiseGain { get; set; }
    public int Health { get; set; }
    public int[] Cost { get; set; }

    public Sample()
    {
        this.Cost = new int[5];
    }

    public bool CanDo(Board board, int[] used, int[] extra, int[] freeSoon)
    {
        bool canDo = true;
        int[] add = new int[5];
        
        for (int i = 0; i < 5; ++i)
        {
            if (used != null)
            {
                add[i] = used[i];
            }
            if (extra != null)
            {
                add[i] -= extra[i];
            }

            int needed = add[i] + this.Cost[i] - board.Me.Expertise[i] - board.Me.Storage[i];
            if(freeSoon == null)
            {
                if (needed > 0 && needed > board.Available[i])
                {
                    canDo = false;
                    break;
                }
            }
            else
            {
                if (needed > 0 && needed > board.Available[i] + freeSoon[i])
                {
                    canDo = false;
                    break;
                }
            }            
        }
        return canDo;
    }
      
    public bool CanStore(Player player, int[] used, int[] extra)
    {
        bool canStore = true;
        int[] add = new int[5];

        for (int i = 0; i < 5; ++i)
        {
            if (used != null)
            {
                add[i] = used[i];
            }
            if(extra != null)
            {
                add[i] -= extra[i];
            }

            if (player.Storage[i] < add[i] + this.Cost[i] - player.Expertise[i])
                canStore = false;
        }
        return canStore;
    }

    public int CountNeededMolecules(Player player, int[] used, int[] extra)
    {
        int count = 0;
        int[] add = new int[5];

        for (int i = 0; i < 5; ++i)
        {
            if (used != null)
            {
                add[i] = used[i];
            }
            if (extra != null)
            {
                add[i] -= extra[i];
            }

            int needed = add[i] + this.Cost[i] - player.Expertise[i] - player.Storage[i];
            if(needed > 0)
            {
                count += needed;
            }
        }

        return count;
    }

    public int CountNeededMolecules(Player player, int molecule) => this.Cost[molecule] > 0 ? this.Cost[molecule] - player.Expertise[molecule] - player.Storage[molecule] : 0;

    public string FindNeededMolecule(Board board, int[] used, int[] extra)
    {
        string molecule = "";
        int[] add = new int[5];

        // mocule, score
        List<Tuple<int, int>> scoredMolecules = new List<Tuple<int, int>>();
        for (int i = 0; i < 5; ++i)
        {
            int score = 0;
            board.Me.Samples(board).ForEach(s => score += s.CountNeededMolecules(board.Me, i));
            board.Opponent.Samples(board).ForEach(s => score += s.CountNeededMolecules(board.Opponent, i));
            if(score > board.Available[i]) // means gonna fight for it
            {
                score += 10;
            }
            scoredMolecules.Add(new Tuple<int, int>(i, score));
        }

        // Sort by score
        scoredMolecules = scoredMolecules.OrderByDescending(t => t.Item2).ToList();

        for (int m = 0; m < 5; ++m)
        {
            int i = scoredMolecules[m].Item1;
            if (used != null)
            {
                add[i] = used[i];
            }
            if (extra != null)
            {
                add[i] -= extra[i];
            }

            int needed = add[i] + this.Cost[i] - board.Me.Expertise[i] - board.Me.Storage[i];

            if (needed > 0)
            {
                molecule = ((Molecule)i).ToString();
                break;
            }
        }
       
        return molecule;
    }

    public void Debug()
    {
        Console.Error.WriteLine($"* Sample *");
        Console.Error.WriteLine($"Id : {this.Id}");
        Console.Error.WriteLine($"CarriedBy : {this.CarriedBy}");
        Console.Error.WriteLine($"Rank : {this.Rank}");
        Console.Error.WriteLine($"ExpertiseGain : {this.ExpertiseGain}");
        Console.Error.WriteLine($"Health : {this.Health}");
        string cost = string.Join(",", this.Cost);
        Console.Error.WriteLine($"Cost : {cost}");
        Console.Error.WriteLine($"* End Sample *");
        Console.Error.WriteLine($"");
    }
}

public class Project
{
    public int Id { get; set; }
    public int Health => 50;
    public int[] Limit { get; set; }

    public Project()
    {
        this.Limit = new int[5];
    }

    public void Debug()
    {
        Console.Error.WriteLine($"* Project *");
        Console.Error.WriteLine($"Id : {this.Id}");
        Console.Error.WriteLine($"Health : {this.Health}");
        string limit = string.Join(",", this.Limit);
        Console.Error.WriteLine($"Limit : {limit}");
        Console.Error.WriteLine($"* End Project *");
        Console.Error.WriteLine($"");
    }
}

public class Board
{
    public int Turn { get; set; }
    public int[] Available { get; set; }
    public List<Sample> Samples { get; set; }
    public List<Project> Projects { get; set; }
    public Player[] Players { get; set; }
    public Player Me => this.Players[0];
    public Player Opponent => this.Players[1];

    public Board()
    {
        this.Available = new int[5];
        this.Samples = new List<Sample>();
        this.Projects = new List<Project>();
        this.Players = new Player[2];
    }

    public void Debug()
    {
        Console.Error.WriteLine($"*** Board Information ***");
        Console.Error.WriteLine($"Turn : {this.Turn}");
        string available = string.Join(",", this.Available);
        Console.Error.WriteLine($"Available : {available}");
        Console.Error.WriteLine($"** Projects **");
        this.Projects.ForEach(s => s.Debug());
        Console.Error.WriteLine($"** Samples **");
        this.Samples.Where(s => s.CarriedBy == -1).ToList().ForEach(s => s.Debug());
        Console.Error.WriteLine($"** Players **");
        for (int i = 0; i < 2; ++i)
        {
            this.Players[i].Debug(this);
        }
    }
}

public class Strategy
{
    public int Step { get; set; }
    public void Play(Board board)
    {
        switch(this.Step)
        {
            case 0:
                // START STATE

                // Go to samples
                Console.WriteLine("GOTO SAMPLES");
                this.Step = 1;
                break;
            case 1:
                // SAMPLES STATE

                if(board.Me.Eta != 0)
                {
                    // I'm not arrived
                    Console.WriteLine("GOTO SAMPLES");
                }
                else
                {
                    // Full me up
                    if (board.Me.Samples(board).Count < 3)
                    {
                        // Take rank 1 at start for now
                        int rank = 1;
                        int expertise = board.Me.Expertise.Sum();
                        if (expertise >= 3) //3
                        {
                            rank = 2;
                        }
                        if(expertise >= 6 && board.Me.Samples(board).Count(s => s.Rank == 3) < 2) //6
                        {
                            rank = 3;
                        }
                        Console.WriteLine($"CONNECT {rank}");
                    }
                    else
                    {
                        // Go to Diagnose                    
                        Console.WriteLine("GOTO DIAGNOSIS");
                        this.Step = 2;
                    }
                }                
                break;
            case 2:
                // DIAGNOSE STATE

                if (board.Me.Eta != 0)
                {
                    // I'm not arrived
                    Console.WriteLine("GOTO DIAGNOSIS");
                }
                else
                {
                    Sample myUndiagSample = board.Me.Samples(board).FirstOrDefault(s => s.Health == -1);
                    if (myUndiagSample != null)
                    {
                        Console.WriteLine($"CONNECT {myUndiagSample.Id}");
                    }
                    else
                    {
                        // Throw if i can't do any one
                        var mySamples = board.Me.Samples(board);
                        int[] freeSoon = board.Opponent.WillFree(board);
                        bool canDoOneSoon = mySamples.Any(s => s.CanDo(board, null, null, freeSoon));
                        
                        if (mySamples.Count == 0)
                        {
                            // Go to samples
                            Console.WriteLine("GOTO SAMPLES");
                            this.Step = 1;
                        }
                        else if (canDoOneSoon)
                        {
                            // Go to molecules
                            Console.WriteLine("GOTO MOLECULES");
                            this.Step = 3;
                        }
                        else
                        {
                            // throw them
                            Console.WriteLine($"CONNECT {mySamples.First().Id}");
                        }
                    }
                }                
                break;
            case 3:
                // MOLECULES STATE

                if (board.Me.Eta != 0)
                {
                    // I'm not arrived
                    Console.WriteLine("GOTO MOLECULES");
                }
                else
                {
                    // Take enough of each
                    bool taken = false;

                    var mySamples = board.Me.Samples(board).OrderByDescending(s => s.Health).ThenBy(s => s.CountNeededMolecules(board.Me, null, null)).ToList();

                    if (board.Me.Storage.Sum() < 10)
                    {
                        int[] used = new int[5];
                        int[] extraExpertise = new int[5];

                        for (int s = 0; s < mySamples.Count; ++s)
                        {
                            Sample checkedSample = mySamples[s];
                            if (checkedSample.CanStore(board.Me, used, extraExpertise))
                            {
                                for (int i = 0; i < 5; ++i)
                                {
                                    used[i] += checkedSample.Cost[i] - board.Me.Expertise[i];
                                }
                                int gain = (int)Enum.Parse(typeof(Molecule), checkedSample.ExpertiseGain);
                                extraExpertise[gain] += 1;
                            }
                            else if (checkedSample.CanDo(board, used, extraExpertise, null))
                            {
                                string needed = checkedSample.FindNeededMolecule(board, used, extraExpertise);               
                                if( !string.IsNullOrWhiteSpace(needed) )
                                  //  && checkedSample.CountNeededMolecules(board, used) <= 10 - board.Me.Storage.Sum()) // enough space to store it completely
                                {
                                    Console.WriteLine($"CONNECT {needed}");
                                    taken = true;
                                    break;
                                }
                            }
                        }
                    }

                    if (!taken)
                    {
                        if(mySamples.Count <= 1 && board.Turn < 190)
                        {
                            // Just have one, better go to take some more,
                            Console.WriteLine("GOTO SAMPLES");
                            this.Step = 1;
                        }
                        else
                        {
                            int[] freeSoon = board.Opponent.WillFree(board);
                            bool canDoOne = mySamples.Any(s => s.CanDo(board, null, null, null));
                            bool canDoOneSoon = mySamples.Any(s => s.CanDo(board, null, null, freeSoon));

                            if (canDoOne)
                            {
                                // Can store one, go to lab
                                Console.WriteLine("GOTO LABORATORY");
                                this.Step = 4;
                            }
                            else if(canDoOneSoon)
                            {
                                // WAIT !    // mb check if my opponent tricks me
                                Console.WriteLine("WAIT");
                            }
                            else if (mySamples.Count == 3)
                            {
                                // Go to throw
                                // Go to Diagnose                    
                                Console.WriteLine("GOTO DIAGNOSIS");
                                this.Step = 2;
                            }
                            else
                            {
                                // Go to samples
                                Console.WriteLine("GOTO SAMPLES");
                                this.Step = 1;
                            }
                        }                       
                    }
                }                
                break;
            case 4:
                // LABORATORY STATE

                if (board.Me.Eta != 0)
                {
                    // I'm not arrived
                    Console.WriteLine("GOTO LABORATORY");
                }
                else
                {
                    var mySamples = board.Me.Samples(board).OrderByDescending(s => s.Health).ToList();
                    Sample toStore = null;

                    for(int s = 0; s < mySamples.Count; ++s)
                    {
                        Sample checkSample = mySamples[s];
                        
                        if (checkSample.CanStore(board.Me, null, null))
                        {
                            toStore = checkSample;
                            break;
                        }
                    }        

                    if (toStore != null)
                    {
                        // Cloud it
                        Console.WriteLine($"CONNECT {toStore.Id}");
                    }
                    else
                    {                       
                        if(board.Turn > 180)
                        {
                            // Go to molecules
                            Console.WriteLine("GOTO MOLECULES");
                            this.Step = 3;                        
                        }
                        else
                        {
                            int[] freeSoon = board.Opponent.WillFree(board);
                            bool canDoOneSoon = mySamples.Any(s => s.CanDo(board, null, null, freeSoon));

                            if (!canDoOneSoon)
                            {
                                if (mySamples.Count == 3)
                                {
                                    // Go to throw
                                    // Go to Diagnose                    
                                    Console.WriteLine("GOTO DIAGNOSIS");
                                    this.Step = 2;
                                }
                                else
                                {
                                    // We need one new, go to samples
                                    Console.WriteLine("GOTO SAMPLES");
                                    this.Step = 1;
                                }
                            }
                            else
                            {
                                // Go to molecules
                                Console.WriteLine("GOTO MOLECULES");
                                this.Step = 3;
                            }
                        }                                               
                    }
                }                
                break;
        }
    }
}

public class Game
{
    static void Main(string[] args)
    {
        Board board = new Board() { Turn = 0 };
        Strategy strategy = new Strategy();

        string[] inputs;
        int projectCount = int.Parse(Console.ReadLine());
        for (int i = 0; i < projectCount; i++)
        {
            Project project = new Project { Id = i };
            board.Projects.Add(project);

            inputs = Console.ReadLine().Split(' ');
            for(int j = 0; j < 5; ++j)
            {
                project.Limit[j] = int.Parse(inputs[j]);
            }
        }

        // game loop
        while (true)
        {
            for (int i = 0; i < 2; i++)
            {
                Player player = new Player() { Id = i };
                board.Players[i] = player;

                inputs = Console.ReadLine().Split(' ');
                player.Target = inputs[0];
                player.Eta = int.Parse(inputs[1]);
                player.Score = int.Parse(inputs[2]);
                for (int j = 3; j < 8; ++j)
                {
                    player.Storage[j - 3] = int.Parse(inputs[j]);
                }
                for (int j = 8; j < 13; ++j)
                {
                    player.Expertise[j - 8] = int.Parse(inputs[j]);
                }                
            }

            inputs = Console.ReadLine().Split(' ');
            for(int j = 0; j < 5; ++j)
            {
                board.Available[j] = int.Parse(inputs[j]);
            }

            int sampleCount = int.Parse(Console.ReadLine());
            board.Samples.Clear();
            for (int i = 0; i < sampleCount; i++)
            {
                Sample sample = new Sample();
                board.Samples.Add(sample);

                inputs = Console.ReadLine().Split(' ');
                sample.Id = int.Parse(inputs[0]);
                sample.CarriedBy = int.Parse(inputs[1]);
                sample.Rank = int.Parse(inputs[2]);
                sample.ExpertiseGain = inputs[3];
                sample.Health = int.Parse(inputs[4]);
                for(int j = 5; j < 10; ++j)
                {
                    sample.Cost[j-5] = int.Parse(inputs[j]);
                }
            }

            // Write an action using Console.WriteLine()
            // To debug: Console.Error.WriteLine("Debug messages...");
            // board.Debug();
            
            // Strategy !
            strategy.Play(board);
            
            board.Turn++;
        }
    }
}