#include "person.capnp.h"
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

using namespace std;

string decodeMessageToStruct(kj::ArrayPtr<char> encoded_arr) {
    string str_repr;
    str_repr.reserve(256);
    auto encoded_array_ptr = encoded_arr;
    auto encoded_char_array = encoded_array_ptr.begin();
    auto size = encoded_array_ptr.size();

    auto received_array = kj::ArrayPtr<capnp::word>(reinterpret_cast<capnp::word*>(encoded_char_array), size/sizeof(capnp::word));
    capnp::FlatArrayMessageReader message_receiver_builder(received_array);
    Person::Reader person_ = message_receiver_builder.getRoot<Person>();
    str_repr.append("Id: ");
    str_repr.append(to_string(person_.getId()));
    str_repr.append("\nName: ");
    str_repr.append(person_.getName().cStr());
    str_repr.append("\nEmail: ");
    str_repr.append(person_.getEmail());
    for (Person::PhoneNumber::Reader phone: person_.getPhones()) {
        const char* typeName = "UNKNOWN";
        switch (phone.getType()) {
        case Person::PhoneNumber::Type::MOBILE: typeName = "mobile"; break;
        case Person::PhoneNumber::Type::HOME: typeName = "home"; break;
        case Person::PhoneNumber::Type::WORK: typeName = "work"; break;
        }

        str_repr.append("\nPhone: ");
        str_repr.append(to_string(phone.getNumber()));
    }
    Person::Employment::Reader employment = person_.getEmployment();
    str_repr.append("\nNationality: ");
    str_repr.append(person_.getNationality().cStr());
    str_repr.append("\nAddress: ");
    str_repr.append(person_.getAddress().cStr());
    str_repr.append("\nBirthday: ");
    str_repr.append(person_.getBitrthday().cStr());

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
    return str_repr;
  
}

kj::Array<capnp::word> encodeStructToBytes(
    uint32_t id, string name, string email, uint32_t phone_number, string school,
    string nationality, string address,
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
    person.setAddress(address);
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
    string address;
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
            persons[i].update_date
        );
        
    }
    diff = clock() - start;
    cout << "Serializing   " << TEST_SIZE << " items takes "
         << diff / 1000 << "ms (" << TEST_SIZE * 1000 * 1000 / diff
         << " per sec or " << setprecision(2) << 1.0 * diff / TEST_SIZE
         << "ns per record)" << endl;
    start = clock();
    for (int i = 0; i < TEST_SIZE; ++i) {
        str_reprs[i] = decodeMessageToStruct(byte_msgs[i].asChars());
    }
    diff = clock() - start;
    cout << "Deserializing " << TEST_SIZE << " items takes "
         << diff / 1000 << "ms (" << TEST_SIZE * 1000 * 1000 / diff
         << " per sec or " << setprecision(2) << 1.0 * diff / TEST_SIZE
         << "ns per record)" << endl;
    cout << "\n===== Sample item so that compiler can't just optimize the "
         << "calculation away=====\n"
         << str_reprs[rand() % str_reprs.size()] << endl;
    return 0;
}