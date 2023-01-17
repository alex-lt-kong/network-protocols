using System;
using System.IO;
using PbTest;
using Google.Protobuf;
using System.Runtime;

namespace MyAddressBook
{
    class Program
    {
        const UInt32 TEST_SIZE = 2 * 1000 * 1000;
        static Person[] generateRandomData()
        {
            string[] names = {
                "Alice", "Bob", "Charlie", "Dave", "Ella", "Frankenstein"
            };
            string[] email_hosts = {
                "@gmail.com", "@outlook.com", "@yahoo.com", "@qq.com", "@icloud.com", "@proton.me"
            };
            string[] schools = {
                "MIT", "Stanford", "Harverd", "Cambridge", "Oxford", "HKUST"
            };
            string[] nationalities = {
                "America", "Britain", "China", "Dominica", "El Salvador", "Finland"
            };
            string[] addresses = {
                "4417 Jade Underpass Suite 432, Schowalterton, Alabama",
                "Studio 75x Stacey Underpass, Mollyfurt, Grantborough",
                "63 Nok Teu Ya East Road, New Territories, Kowloon",
                "Rácz üdülőpart 813., Budapest, Nógrád",
                "Av. Esteve, Nro 6, Parroquia Lucas, Carabobo",
                "Voortmanring 9-p, Beek, Zuid-Holland"
            };
            string[] dates = {
                "1970-01-01", "1999-12-31", "2000-01-01", "1111-11-11", "2023-01-13",
                "0001-01-01", "9999-12-31", "1234-05-06", "4321-12-34",
            };

            string[] self_introductions = {
                "“My name is Randy Patterson, and I’m currently looking for a job in youth services. I have 10 years of experience working with youth agencies. I have a bachelor’s degree in outdoor education. I raise money, train leaders, and organize units. I have raised over $100,000 each of the last six years. I consider myself a good public speaker, and I have a good sense of humor. “Who do you know who works with youth?”",
                "“My name is Lucas Martin, and I enjoy meeting new people and finding ways to help them have an uplifting experience. I have had a variety of customer service opportunities, through which I was able to have fewer returned products and increased repeat customers, when compared with co-workers. I am dedicated, outgoing, and a team player. Who could I speak with in your customer service department about your organization’s customer service needs?”",
                "“People find me to be an upbeat, self-motivated team player with excellent communication skills. For the past several years I have worked in lead qualification, telemarketing, and customer service in the technology industry. My experience includes successfully calling people in director-level positions of technology departments and developing viable leads. I have a track record of maintaining a consistent call and activity volume and consistently achieving the top 10 percent in sales, and I can do the same thing for your company.”",
                "“I am a dedicated person with a family of four. I enjoy reading, and the knowledge and perspective that my reading gives me has strengthened my teaching skills and presentation abilities. I have been successful at raising a family, and I attribute this success to my ability to plan, schedule, and handle many different tasks at once. This flexibility will help me in the classroom, where there are many different personalities and learning styles.”",
                "I have moved to this city three months ago. I love the street food here. On weekends, I explore new eating joints. This way, I get to learn the routes of this city and prepare myself professionally. Being a sales professional, my profile requires a lot of traveling.",
                "I have done my schooling from Jaipur. For my graduation, I chose Xyz program at Abc University. It was a very enriching experience at the University as not only we were actively involved in practical projects, but also got opportunities to participate in a number of sports and other extra-curricular activities such as plays and skits."
            };
            Person[] persons = new Person[TEST_SIZE];
            Random rnd = new Random(Guid.NewGuid().GetHashCode());
            for (int i = 0; i < TEST_SIZE; ++i)
            {
                int idx = rnd.Next(0, names.Length);
                persons[i] = new Person()
                {
                    Id = (uint)rnd.Next(0, 2147483647),
                    Name = names[idx],
                    Email = names[idx] + email_hosts[idx],
                    Phones = { new Person.Types.PhoneNumber { Number = 5551212 } },
                    Nationality = nationalities[idx],
                    Address = addresses[idx],
                    Bitrthday = dates[idx],
                    CreationDate = dates[idx],
                    UpdateDate = dates[idx],
                    SelfIntroduction = self_introductions[idx],
                    Scores = {
                        (float)(rnd.Next(0, 1000) / 10.0), (float)(rnd.Next(0, 1000) / 10.0),(float)(rnd.Next(0, 1000) / 10.0),(float)(rnd.Next(0, 1000) / 10.0),
                        (float)(rnd.Next(0, 1000) / 10.0), (float)(rnd.Next(0, 1000) / 10.0),(float)(rnd.Next(0, 1000) / 10.0),(float)(rnd.Next(0, 1000) / 10.0),
                        (float)(rnd.Next(0, 1000) / 10.0), (float)(rnd.Next(0, 1000) / 10.0),(float)(rnd.Next(0, 1000) / 10.0),(float)(rnd.Next(0, 1000) / 10.0),
                        (float)(rnd.Next(0, 1000) / 10.0), (float)(rnd.Next(0, 1000) / 10.0),(float)(rnd.Next(0, 1000) / 10.0),(float)(rnd.Next(0, 1000) / 10.0)
                    }

                };
            }
            return persons;
        }

        static void Main(string[] args)
        {
            Random rnd = new Random(Guid.NewGuid().GetHashCode());
            Person[] persons = new Person[TEST_SIZE];
            Person[] persons_decoded = new Person[TEST_SIZE];
            MemoryStream[] mems = new MemoryStream[TEST_SIZE];
            Console.WriteLine("===== Testing ProtoBuf =====");
            persons = generateRandomData();
            Console.WriteLine("sample data are prepared");
            
            GCSettings.LatencyMode = GCLatencyMode.LowLatency;

            long now = DateTimeOffset.Now.ToUnixTimeMilliseconds();
            for (int i = 0; i < TEST_SIZE; ++i)
            {
                mems[i] = new MemoryStream();
                persons[i].WriteTo(mems[i]);
                mems[i].Seek(0, SeekOrigin.Begin);
            }
            long diff = DateTimeOffset.Now.ToUnixTimeMilliseconds() - now;
            Console.WriteLine($"Serializing  {TEST_SIZE} items takes {diff} ms ({TEST_SIZE * 1000.0 / diff:.##} records per sec or {1000.0 * diff / TEST_SIZE:.##} us per record)");

            now = DateTimeOffset.Now.ToUnixTimeMilliseconds();
            for (int i = 0; i < TEST_SIZE; ++i)
            {
                persons_decoded[i] = Person.Parser.ParseFrom(mems[i]);
            }
            diff = DateTimeOffset.Now.ToUnixTimeMilliseconds() - now;

            Console.WriteLine($"Deserializing {TEST_SIZE} items takes {diff} ms ({TEST_SIZE * 1000.0 / diff:.##} records per sec or {1000.0 * diff / TEST_SIZE:.##} us per record)\n");
            Console.WriteLine("--- Sample item ---");
            // Let's print one sample, so that there is no way for compilers to optimize the entire process away.
            Person rnd_person_decoded = persons_decoded[rnd.Next(0, (int)TEST_SIZE - 1)];
            Console.WriteLine($"Id: {rnd_person_decoded.Id}");
            Console.WriteLine($"Name: {rnd_person_decoded.Name}");
            Console.WriteLine($"E-mail: {rnd_person_decoded.Email}");
            Console.WriteLine($"Self-intro: {rnd_person_decoded.SelfIntroduction}");
            if (rnd_person_decoded.Phones?.Count > 0)
            {
                foreach (var phone in rnd_person_decoded.Phones)
                {
                    Console.WriteLine($"{phone.Type}: {phone.Number}");
                }
            }

            Console.Write("Scores: [");
            for (int i = 0; i < 16; ++i)
            {
                Console.Write(rnd_person_decoded.Scores[i]);
                Console.Write(", ");
            }
            Console.WriteLine("]");

            Console.ReadLine();
        }
    }
}