#include <capnp/message.h>
#include <capnp/serialize-packed.h>
#include <iostream>
#include <iomanip>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <vector>
#include <string>
#include <stdlib.h>
#include <time.h>
#include "./capnp/person.capnp.h"
#include "./capnp/capnp.h"

using namespace std;

int main() {
    srand(time(NULL));
    
    vector<string> names{
        "Alice", "Bob", "Charlie", "Dave", "Ella", "Frankenstein"
    };
    vector<string> email_hosts{
        "@gmail.com", "@outlook.com", "@yahoo.com", "@qq.com",
        "@icloud.com", "@proton.me"
    };
    vector<string> schools{
        "MIT", "Stanford", "Harverd", "Cambridge", "Oxford", "HKUST"
    };
    vector<string> nationalities{
        "America", "Britain", "China", "Dominica", "El Salvador", "Finland"
    };
    vector<string> addresses{
        "4417 Jade Underpass Suite 432, Schowalterton, Alabama",
        "Studio 75x Stacey Underpass, Mollyfurt, Grantborough",
        "63 Nok Teu Ya East Road, New Territories, Kowloon",
        "Rácz üdülőpart 813., Budapest, Nógrád",
        "Av. Esteve, Nro 6, Parroquia Lucas, Carabobo",
        "Voortmanring 9-p, Beek, Zuid-Holland"
    };
    vector<string> dates{
        "1970-01-01", "1999-12-31", "2000-01-01", "1111-11-11", "2023-01-13"
        "0001-01-01", "9999-12-31", "1234-05-06", "4321-12-34", 
    };

    vector<string> self_introductions{
        "“My name is Randy Patterson, and I’m currently looking for a job in youth services. I have 10 years of experience working with youth agencies. I have a bachelor’s degree in outdoor education. I raise money, train leaders, and organize units. I have raised over $100,000 each of the last six years. I consider myself a good public speaker, and I have a good sense of humor. “Who do you know who works with youth?”",
        "“My name is Lucas Martin, and I enjoy meeting new people and finding ways to help them have an uplifting experience. I have had a variety of customer service opportunities, through which I was able to have fewer returned products and increased repeat customers, when compared with co-workers. I am dedicated, outgoing, and a team player. Who could I speak with in your customer service department about your organization’s customer service needs?”",
        "“People find me to be an upbeat, self-motivated team player with excellent communication skills. For the past several years I have worked in lead qualification, telemarketing, and customer service in the technology industry. My experience includes successfully calling people in director-level positions of technology departments and developing viable leads. I have a track record of maintaining a consistent call and activity volume and consistently achieving the top 10 percent in sales, and I can do the same thing for your company.”",
        "“I am a dedicated person with a family of four. I enjoy reading, and the knowledge and perspective that my reading gives me has strengthened my teaching skills and presentation abilities. I have been successful at raising a family, and I attribute this success to my ability to plan, schedule, and handle many different tasks at once. This flexibility will help me in the classroom, where there are many different personalities and learning styles.”",
        "I have moved to this city three months ago. I love the street food here. On weekends, I explore new eating joints. This way, I get to learn the routes of this city and prepare myself professionally. Being a sales professional, my profile requires a lot of traveling.",
        "I have done my schooling from Jaipur. For my graduation, I chose Xyz program at Abc University. It was a very enriching experience at the University as not only we were actively involved in practical projects, but also got opportunities to participate in a number of sports and other extra-curricular activities such as plays and skits."
    };
    
    constexpr size_t TEST_SIZE = 10 * 1000 * 1000;
    vector<person_struct> persons{TEST_SIZE};
    for (size_t i = 0; i < TEST_SIZE; ++i) {
        uint32_t idx = rand() % names.size();
        persons[i].id = rand();
        persons[i].name = names[idx];
        persons[i].email = persons[i].name + email_hosts[idx];
        persons[i].phone_number = rand();
        persons[i].school = schools[idx];
        persons[i].nationality = nationalities[idx];
        persons[i].address = addresses[idx];
        persons[i].birthday = dates[idx];
        persons[i].creation_date = dates[idx+1];
        persons[i].update_date = dates[idx+2];
        persons[i].self_introduction = self_introductions[idx];
    }
    
    vector<kj::Array<capnp::word>> byte_msgs{TEST_SIZE};
    vector<string> str_reprs{TEST_SIZE};
    printf("sample data are prepared\n");
    clock_t start = clock(), diff;
    for (int i = 0; i < TEST_SIZE; ++i) {
        
        byte_msgs[i] = encodeStructToBytes(
            persons[i].id, persons[i].name, persons[i].email,
            persons[i].phone_number, persons[i].school,
            persons[i].nationality, persons[i].address,
            persons[i].birthday, persons[i].creation_date,
            persons[i].update_date, persons[i].self_introduction
        );
        
    }
    diff = clock() - start;
    cout << "Serializing   " << TEST_SIZE << " items takes "
         << diff / 1000 << "ms (" << TEST_SIZE * 1000 * 1000 / diff
         << " per sec or " << setprecision(2) << 1.0 * diff / TEST_SIZE
         << "us per record)" << endl;
    start = clock();
    for (int i = 0; i < TEST_SIZE; ++i) {
        str_reprs[i] = decodeMessageToStruct(byte_msgs[i].asChars());
    }
    diff = clock() - start;
    cout << "Deserializing " << TEST_SIZE << " items takes "
         << diff / 1000 << "ms (" << TEST_SIZE * 1000 * 1000 / diff
         << " per sec or " << setprecision(2) << 1.0 * diff / TEST_SIZE
         << "us per record)" << endl;
    cout << "\n===== Sample item so that compiler can't just optimize the "
         << "calculation away=====\n"
         << str_reprs[rand() % str_reprs.size()] << endl;
    return 0;
}