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
#include "../rpc.h"
#include "person.capnp.h"

using namespace std;

string decodeMessageToStructCapnp(kj::ArrayPtr<char> encoded_arr) {
    string str_repr;
    str_repr.reserve(256);
    auto encoded_array_ptr = encoded_arr;
    auto encoded_char_array = encoded_array_ptr.begin();
    auto size = encoded_array_ptr.size();

    auto received_array = kj::ArrayPtr<capnp::word>(reinterpret_cast<capnp::word*>(encoded_char_array), size/sizeof(capnp::word));
    capnp::FlatArrayMessageReader message_receiver_builder(received_array);
    Person::Reader person = message_receiver_builder.getRoot<Person>();
    str_repr.append("Id: ");
    str_repr.append(to_string(person.getId()));
    str_repr.append("\nName: ");
    str_repr.append(person.getName().cStr());
    str_repr.append("\nEmail: ");
    str_repr.append(person.getEmail());
    for (Person::PhoneNumber::Reader phone: person.getPhones()) {
        const char* typeName = "UNKNOWN";
        switch (phone.getType()) {
        case Person::PhoneNumber::Type::MOBILE: typeName = "mobile"; break;
        case Person::PhoneNumber::Type::HOME: typeName = "home"; break;
        case Person::PhoneNumber::Type::WORK: typeName = "work"; break;
        }

        str_repr.append("\nPhone: ");
        str_repr.append(to_string(phone.getNumber()));
    }
    Person::Employment::Reader employment = person.getEmployment();
    str_repr.append("\nNationality: ");
    str_repr.append(person.getNationality().cStr());
    str_repr.append("\nAddress: ");
    str_repr.append(person.getAddress().cStr());
    str_repr.append("\nBirthday: ");
    str_repr.append(person.getBitrthday().cStr());
    str_repr.append("\nSelf introduction: ");
    str_repr.append(person.getSelfIntroduction().cStr());

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

kj::Array<capnp::word> encodeStructToBytesCapnp(person_struct p) {
    capnp::MallocMessageBuilder msg_builder;


    Person::Builder person = msg_builder.initRoot<Person>();
    person.setId(p.id);
    person.setName(p.name);
    person.setEmail(p.email);
    // Type shown for explanation purposes; normally you'd use auto.
    capnp::List<Person::PhoneNumber>::Builder alicePhones =
        person.initPhones(1);
    alicePhones[0].setNumber(p.phone_number);
    alicePhones[0].setType(Person::PhoneNumber::Type::MOBILE);
    person.getEmployment().setSchool(p.school);
    person.setNationality(p.nationality);
    person.setAddress(p.address);
    person.setBitrthday(p.birthday);
    person.setCreationDate(p.creation_date);
    person.setUpdateDate(p.update_date);
    person.setSelfIntroduction(p.self_introduction);
    // https://stackoverflow.com/questions/60964979/cap-n-proto-c-serialize-to-char-array-or-any-byte-array
    kj::Array<capnp::word> encoded_arr = messageToFlatArray(msg_builder);
    return encoded_arr;
}

