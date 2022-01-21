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
            BootstrapServers = "debian:9092,debian:9093",
            /* according to this page:
             * https://stackoverflow.com/questions/68002959/kafka-producer-is-not-able-to-update-broker-after-one-of-the-broker-is-up,
             * as long as there is one server available in the list, bootstrap can work.
             */
            MessageSendMaxRetries = 5,
            SaslMechanism = SaslMechanism.Plain,
            SecurityProtocol = SecurityProtocol.SaslPlaintext,
            SaslUsername = "admin",
            SaslPassword = "admin-sec2ret",
        };

        var producer = new ProducerBuilder<Null, string>(config).Build();
        try
        {
            while (Console.ReadKey().Key != ConsoleKey.Escape)
            {
                Task<DeliveryResult<Null, string>> t = producer.ProduceAsync(
                    "als-alert-topic",
                    new Message<Null, string> { Value = $"Message from C# producer, timestamp: {DateTime.Now.ToString("yyyyMMdd-HHmmss")}" }
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