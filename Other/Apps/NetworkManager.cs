using System;
using System.Net;
using System.Net.NetworkInformation;
using System.Net.Sockets;
using System.Threading.Tasks;

class NetworkManager
{
    static async Task Main()
    {
        bool keepRunning = true;
        while (keepRunning)
        {
            Console.Clear();
            Console.WriteLine("=========================================");
            Console.WriteLine("    NetTERM Network Security Manager 🛡️   ");
            Console.WriteLine("=========================================");
            Console.WriteLine("1. Ping Remote Host");
            Console.WriteLine("2. Scan Local Port Range");
            Console.WriteLine("3. Show Network Interfaces (Linux)");
            Console.WriteLine("4. Exit");
            Console.WriteLine("=========================================");
            Console.Write("Select an option (1-4): ");

            string? choice = Console.ReadLine();
            Console.WriteLine("\n─────────────────────────────────────────");

            switch (choice)
            {
                case "1":
                    await RunPingTool();
                    break;
                case "2":
                    await RunPortScanner();
                    break;
                case "3":
                    ShowNetworkInterfaces();
                    break;
                case "4":
                    keepRunning = false;
                    Console.WriteLine("Shutting down Network Security Manager...");
                    break;
                default:
                    Console.WriteLine("❌ Invalid option. Press any key to retry.");
                    Console.ReadKey();
                    break;
            }

            if (keepRunning && choice != "4")
            {
                Console.WriteLine("\nPress any key to return to main menu...");
                Console.ReadKey();
            }
        }
    }

    static async Task RunPingTool()
    {
        Console.Write("Enter host or IP to ping (e.g., 8.8.8.8): ");
        string? host = Console.ReadLine();
        if (string.IsNullOrWhiteSpace(host)) return;

        try
        {
            using Ping pingSender = new Ping();
            Console.WriteLine($"Sending ICMP echo requests to {host}...");
            
            PingReply reply = await pingSender.SendPingAsync(host, 2000);
            if (reply.Status == IPStatus.Success)
            {
                Console.WriteLine($"✅ Response from {reply.Address}");
                Console.WriteLine($"   RoundTrip time: {reply.RoundtripTime} ms");
                Console.WriteLine($"   TTL: {reply.Options?.Ttl}");
            }
            else
            {
                Console.WriteLine($"❌ Ping failed: {reply.Status}");
            }
        }
        catch (Exception ex)
        {
            Console.WriteLine($"❌ Error: {ex.Message}");
        }
    }

    static async Task RunPortScanner()
    {
        string target = "127.0.0.1";
        int[] targetPorts = { 22, 80, 443, 8080, 3000 };
        
        Console.WriteLine($"Starting async port scan on target: {target}");
        Console.WriteLine("─────────────────────────────────────────");

        foreach (int port in targetPorts)
        {
            try
            {
                using TcpClient client = new TcpClient();
                var connectTask = client.ConnectAsync(target, port);
                var delayTask = Task.Delay(400);

                var completedTask = await Task.WhenAny(connectTask, delayTask);
                if (completedTask == connectTask && client.Connected)
                {
                    Console.WriteLine($"🔥 Port {port,-5} | STATE: OPEN   (Active Service)");
                }
                else
                {
                    Console.WriteLine($"❌ Port {port,-5} | STATE: CLOSED");
                }
            }
            catch
            {
                Console.WriteLine($"❌ Port {port,-5} | STATE: CLOSED");
            }
        }
    }

    static void ShowNetworkInterfaces()
    {
        Console.WriteLine("Active Linux Network Interfaces State:");
        Console.WriteLine("─────────────────────────────────────────");

        try
        {
            NetworkInterface[] interfaces = NetworkInterface.GetAllNetworkInterfaces();
            foreach (NetworkInterface ni in interfaces)
            {
                if (ni.OperationalStatus == OperationalStatus.Up)
                {
                    Console.WriteLine($"Interface : {ni.Name}");
                    Console.WriteLine($"   Type   : {ni.NetworkInterfaceType}");
                    Console.WriteLine($"   Speed  : {ni.Speed / 1000000} Mbps");
                    
                    var ipProps = ni.GetIPProperties();
                    foreach (var addr in ipProps.UnicastAddresses)
                    {
                        if (addr.Address.AddressFamily == AddressFamily.InterNetwork)
                        {
                            Console.WriteLine($"   IPv4   : {addr.Address}");
                        }
                    }
                    Console.WriteLine("─────────────────────────────────────────");
                }
            }
        }
        catch (Exception ex)
        {
            Console.WriteLine($"❌ Failed to retrieve interfaces: {ex.Message}");
        }
    }
}
