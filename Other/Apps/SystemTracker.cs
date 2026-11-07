using System;
using System.Diagnostics;
using System.IO;

class SystemTracker
{
    static void Main()
    {
        Console.WriteLine("=========================================");
        Console.WriteLine("     C# System Resource Tracker 📊       ");
        Console.WriteLine("=========================================");

        try
        {
            if (File.Exists("/proc/meminfo"))
            {
                string[] lines = File.ReadAllLines("/proc/meminfo");
                long totalMem = 0;
                long freeMem = 0;

                foreach (string line in lines)
                {
                    if (line.StartsWith("MemTotal:"))
                        totalMem = ParseMemValue(line);
                    if (line.StartsWith("MemAvailable:") || line.StartsWith("MemFree:"))
                        freeMem = ParseMemValue(line);
                }

                long usedMem = totalMem - freeMem;
                Console.WriteLine($"Total Memory : {totalMem / 1024} MB");
                Console.WriteLine($"Used Memory  : {usedMem / 1024} MB");
                Console.WriteLine($"Free Memory  : {freeMem / 1024} MB");
            }
            else
            {
                Console.WriteLine("Memory Info  : Only available on Linux OS.");
            }
        }
        catch
        {
            Console.WriteLine("❌ Error reading memory stats.");
        }

        Console.WriteLine("\n─────────────────────────────────────────");
        Console.WriteLine(" Active System Processes (Top 5):");
        Console.WriteLine("  PID\t| Name");
        Console.WriteLine("─────────────────────────────────────────");

        try
        {
            Process[] processList = Process.GetProcesses();
            int count = 0;
            
            foreach (Process proc in processList)
            {
                if (count >= 5) break;
                Console.WriteLine($"  {proc.Id}\t| {proc.ProcessName}");
                count++;
            }
        }
        catch
        {
            Console.WriteLine("❌ Error retrieving active process list.");
        }

        Console.WriteLine("=========================================");
    }

    static long ParseMemValue(string line)
    {
        string[] parts = line.Split(new[] { ' ' }, StringSplitOptions.RemoveEmptyEntries);
        if (parts.Length >= 2 && long.TryParse(parts[1], out long value))
        {
            return value;
        }
        return 0;
    }
}
