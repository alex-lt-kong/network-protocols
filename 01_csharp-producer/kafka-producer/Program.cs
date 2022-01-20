using Confluent.Kafka;
using System.Net;

class TestClass
{

    static void continueWithThis(Task<DeliveryResult<Null, string>> task) {
        if (task.IsFaulted)
        {
        }
        else
        {
            Console.WriteLine($"Wrote to offset: {task.Result.Offset}");
        }
    }

    static void Main(string[] args)
    {
        Console.WriteLine("Hi! Press any key to send a message to Kafka or press Esc to exit!");

        ProducerConfig config = new()
        {
            BootstrapServers = "debian:9092",
            MessageSendMaxRetries = 5,
        };

        var producer = new ProducerBuilder<Null, string>(config).Build();
        try
        {
            while (Console.ReadKey().Key != ConsoleKey.Escape)
            {
                Task<DeliveryResult<Null, string>> t = producer.ProduceAsync(
                    "quickstart-events",
                    new Message<Null, string> { Value = DateTime.Now.ToString("yyyyMMdd-HHmmss") }
                );
                t.ContinueWith(result => continueWithThis(result));
                // => is a Lambda operator
                // In Confluent's official example: https://docs.confluent.io/clients-confluent-kafka-dotnet/current/overview.html#producer
                // t.ContinueWith() takes an anonymous function as its parameter.
            }            
        }
        catch (Exception ex)
        { 
            Console.WriteLine(ex.ToString());
        }
        finally {
            Console.WriteLine("\n\nExiting producer...\n\n");
            //deal with exceptional circumstances in a catch block, and release the resources in the finally block.
            producer.Flush();
            producer.Dispose();
        }

    }
}