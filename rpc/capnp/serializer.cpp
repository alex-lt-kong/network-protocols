#include "person.capnp.h"
#include <capnp/message.h>
#include <capnp/serialize-packed.h>
#include <iostream>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <vector>
#include <string>
#include <stdlib.h>
#include <time.h>

using namespace std;

void writeAddressBook(int fd,
    uint32_t id, string name, string email, uint32_t phone_number, string school,
    string nationality,
    string birthday, string creation_date, string update_date
) {
    capnp::MallocMessageBuilder message;


    Person::Builder person = message.initRoot<Person>();
    person.setId(id);
    person.setName(name);
    person.setEmail(email);
    // Type shown for explanation purposes; normally you'd use auto.
    ::capnp::List<Person::PhoneNumber>::Builder alicePhones =
        person.initPhones(1);
    alicePhones[0].setNumber(phone_number);
    alicePhones[0].setType(Person::PhoneNumber::Type::MOBILE);
    person.getEmployment().setSchool(school);
    person.setNationality(nationality);
    person.setAddress(nationality);
    person.setBitrthday(birthday);
    person.setCreationDate(creation_date);
    person.setUpdateDate(update_date);
    writePackedMessageToFd(fd, message);
}


struct person_struct {
    uint32_t id;
    string name;
    string email;
    uint32_t phone_number;
    string school;
    string nationality;
    string birthday;
    string creation_date;
    string update_date;

};  

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
    vector<string> dates{
        "1970-01-01", "1999-12-31", "2000-01-01", "1111-11-11", "2023-01-13"
        "0001-01-01", "9999-12-31", "1234-05-06", "4321-12-34", 
    };
    
    constexpr size_t TEST_SIZE = 1 * 1000 * 1000;
    vector<person_struct> persons{TEST_SIZE};
    for (size_t i = 0; i < TEST_SIZE; ++i) {
        uint32_t idx = rand() % names.size();
        persons[i].id = rand();
        persons[i].name = names[idx];
        persons[i].email = persons[i].name + email_hosts[idx];
        persons[i].phone_number = rand();
        persons[i].school = schools[idx];
        persons[i].nationality = nationalities[idx];
        persons[i].birthday = dates[idx];
        persons[i].creation_date = dates[idx+1];
        persons[i].update_date = dates[idx+2];
    }
    printf("sample data are prepared\n");

    int fd = open("/tmp/ramdisk/capnp.data", O_CREAT | O_WRONLY, S_IRWXU | S_IRWXG);
    clock_t start = clock(), diff;
    for (int i = 0; i < TEST_SIZE; ++i) {
        
        if (fd == -1)  {
            fprintf(stderr, "1st open() error: %s\n", strerror(errno));
            return -1;
        }
        writeAddressBook(
            fd, persons[i].id, persons[i].name, persons[i].email,
            persons[i].phone_number, persons[i].school, persons[i].nationality,
            persons[i].birthday, persons[i].creation_date,
            persons[i].update_date
        );
    }
    diff = clock() - start;
    close(fd);
    cout << diff / 1000 << "ms, i.e., " << TEST_SIZE * 1000 * 1000 / diff
         << " per sec or " << diff / TEST_SIZE << "ns per record" << endl;
    return 0;
}