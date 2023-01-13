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

void decodeMessageToStruct(kj::ArrayPtr<char> encoded_arr) {
    auto encoded_array_ptr = encoded_arr;
    auto encoded_char_array = encoded_array_ptr.begin();
    auto size = encoded_array_ptr.size();

    auto received_array = kj::ArrayPtr<capnp::word>(reinterpret_cast<capnp::word*>(encoded_char_array), size/sizeof(capnp::word));
    capnp::FlatArrayMessageReader message_receiver_builder(received_array);
    Person::Reader person_ = message_receiver_builder.getRoot<Person>();
   // std::cout << person_.getName().cStr() << ": "
     //           << person_.getEmail().cStr() << std::endl;
    for (Person::PhoneNumber::Reader phone: person_.getPhones()) {
        const char* typeName = "UNKNOWN";
        switch (phone.getType()) {
        case Person::PhoneNumber::Type::MOBILE: typeName = "mobile"; break;
        case Person::PhoneNumber::Type::HOME: typeName = "home"; break;
        case Person::PhoneNumber::Type::WORK: typeName = "work"; break;
        }
      //  std::cout << "  " << typeName << " phone: "
     //           << phone.getNumber() << std::endl;
    }
    Person::Employment::Reader employment = person_.getEmployment();
  //  cout << "  nationality: " << person_.getNationality().cStr() << endl;
   // cout << "  address: " << person_.getAddress().cStr() << endl;
   // cout << "  birthday: " << person_.getBitrthday().cStr() << endl;
    switch (employment.which()) {
        case Person::Employment::UNEMPLOYED:
        //std::cout << "  unemployed" << std::endl;
        break;
        case Person::Employment::EMPLOYER:
       // std::cout << "  employer: "
       //             << employment.getEmployer().cStr() << std::endl;
        break;
        case Person::Employment::SCHOOL:
        //std::cout << "  student at: "
       //             << employment.getSchool().cStr() << std::endl;
        break;
        case Person::Employment::SELF_EMPLOYED:
        //std::cout << "  self-employed" << std::endl;
        break;
    }
  
}

kj::Array<capnp::word> encodeStructToBytes(
    uint32_t id, string name, string email, uint32_t phone_number, string school,
    string nationality,
    string birthday, string creation_date, string update_date
) {
    capnp::MallocMessageBuilder msg_builder;


    Person::Builder person = msg_builder.initRoot<Person>();
    person.setId(id);
    person.setName(name);
    person.setEmail(email);
    // Type shown for explanation purposes; normally you'd use auto.
    capnp::List<Person::PhoneNumber>::Builder alicePhones =
        person.initPhones(1);
    alicePhones[0].setNumber(phone_number);
    alicePhones[0].setType(Person::PhoneNumber::Type::MOBILE);
    person.getEmployment().setSchool(school);
    person.setNationality(nationality);
    person.setAddress(nationality);
    person.setBitrthday(birthday);
    person.setCreationDate(creation_date);
    person.setUpdateDate(update_date);
    // https://stackoverflow.com/questions/60964979/cap-n-proto-c-serialize-to-char-array-or-any-byte-array
    kj::Array<capnp::word> encoded_arr = messageToFlatArray(msg_builder);
    return encoded_arr;
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


    clock_t start = clock(), diff;
    for (int i = 0; i < TEST_SIZE; ++i) {
        
        kj::Array<capnp::word> byte_msg = encodeStructToBytes(
            persons[i].id, persons[i].name, persons[i].email,
            persons[i].phone_number, persons[i].school, persons[i].nationality,
            persons[i].birthday, persons[i].creation_date,
            persons[i].update_date
        );
        decodeMessageToStruct(byte_msg.asChars());
    }
    diff = clock() - start;

    cout << diff / 1000 << "ms, i.e., " << TEST_SIZE * 1000 * 1000 / diff
         << " per sec or " << diff / TEST_SIZE << "ns per record" << endl;
    return 0;
}