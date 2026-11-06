using System;

class Program
{
    static void Main()
    {
        Console.WriteLine("=========================================");
        Console.WriteLine("       C# Factorial Calculator 🚀        ");
        Console.WriteLine("=========================================");
        Console.Write("Enter a number (1-20): ");
        
        if (int.TryParse(Console.ReadLine(), out int number))
        {
            if (number < 1 || number > 20)
            {
                Console.WriteLine("❌ Error: Please enter a number between 1 and 20.");
                return;
            }

            long factorial = 1;
            for (int i = 1; i <= number; i++)
            {
                factorial *= i;
            }

            Console.WriteLine("\n─────────────────────────────────────────");
            Console.WriteLine($" Factorial of {number} is : {factorial}");
            Console.WriteLine("─────────────────────────────────────────");
        }
        else
        {
            Console.WriteLine("❌ Error: Invalid numeric input.");
        }
    }
}