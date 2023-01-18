#include "serialization.h"
#include "./capnp/cp_test.capnp.h"
#include "./capnp/capnp.h"
#include "./protobuf/protobuf.h"

using namespace std;

vector<person_struct> generateRandomData() {
    
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
        "1970-01-01", "1999-12-31", "2000-01-01", "1111-11-11", "2023-01-13",
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
    
    vector<person_struct> persons{TEST_SIZE};
    for (size_t i = 0; i < TEST_SIZE; ++i) {
        uint32_t idx = rand() % names.size();
        persons[i].id = rand();
        persons[i].name = names[idx];
        persons[i].email = persons[i].name + email_hosts[idx];
        persons[i].phone_number = rand();
        persons[i].phone_type = persons[i].phone_number % 3;
        persons[i].school = schools[idx];
        persons[i].nationality = nationalities[idx];
        persons[i].address = addresses[idx];
        persons[i].birthday = dates[idx];
        persons[i].creation_date = dates[idx+1];
        persons[i].update_date = dates[idx+2];
        persons[i].self_introduction = self_introductions[idx];
        for (
            int j = 0;
            j < sizeof(persons[i].scores)/sizeof(persons[i].scores[0]);
            ++j
        ) {
            persons[i].scores[j] = getRandomFloat(0, 100.0);
        }
    }
    return persons;
}

string getStringRepresentationOfPerson(person_struct p) {
    string str_repr;
    str_repr.reserve(1024);
    str_repr.append("Id: ");
    str_repr.append(to_string(p.id));
    str_repr.append("\nName: ");
    str_repr.append(p.name);
    str_repr.append("\nEmail: ");
    str_repr.append(p.email);
    str_repr.append("\nPhone: ");
    str_repr.append(to_string(p.phone_number));
    if (p.phone_type == 0) {
        str_repr.append("(mobile)");
    } else if (p.phone_type == 1) {
        str_repr.append("(home)");
    } else {
        str_repr.append("(work)");
    }
    
    str_repr.append("\nNationality: ");
    str_repr.append(p.nationality);
    str_repr.append("\nAddress: ");
    str_repr.append(p.address);
    str_repr.append("\nBirthday: ");
    str_repr.append(p.birthday);
    str_repr.append("\nSelf introduction: ");
    str_repr.append(p.self_introduction);
    str_repr.append("\nScores: [");
    for (int i = 0; i< sizeof(p.scores)/sizeof(p.scores[0]); ++i) {        
        str_repr.append(to_string(p.scores[i]));
        str_repr.append(", ");
    }
    str_repr.append("]\n");
    return str_repr;
  
}

float getRandomFloat(float a, float b) {
    float random = ((float) rand()) / (float) RAND_MAX;
    float diff = b - a;
    float r = random * diff;
    return a + r;
}
