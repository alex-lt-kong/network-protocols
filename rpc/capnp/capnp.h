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
    str_repr.reserve(1024);
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
        str_repr.append("\nPhone: ");
        str_repr.append(to_string(phone.getNumber()));
        
        if (phone.getType() == Person::PhoneNumber::Type::MOBILE) {
            str_repr.append("(mobile)");
        } else if (phone.getType() == Person::PhoneNumber::Type::HOME) {
            str_repr.append("(home)");
        } else {
            str_repr.append("(work)");
        }
    }
    str_repr.append("\nNationality: ");
    str_repr.append(person.getNationality().cStr());
    str_repr.append("\nAddress: ");
    str_repr.append(person.getAddress().cStr());
    str_repr.append("\nBirthday: ");
    str_repr.append(person.getBitrthday().cStr());
    str_repr.append("\nSelf introduction: ");
    str_repr.append(person.getSelfIntroduction().cStr());
    double sum = 0;
    str_repr.append("\nsum(Scores): ");
    for (float score: person.getScores()) {        
        sum += score;
    }
    str_repr.append(to_string(sum));
    return str_repr;
  
}

kj::Array<capnp::word> encodeStructToBytesCapnp(person_struct p) {
    capnp::MallocMessageBuilder msg_builder;


    Person::Builder person = msg_builder.initRoot<Person>();
    person.setId(p.id);
    person.setName(p.name);
    person.setEmail(p.email);
    // Type shown for explanation purposes; normally you'd use auto.
    capnp::List<Person::PhoneNumber>::Builder phones = person.initPhones(1);
    phones[0].setNumber(p.phone_number);
    if (p.phone_type == 0) {
        phones[0].setType(Person::PhoneNumber::Type::MOBILE);
    } else if (p.phone_type == 1) {
        phones[0].setType(Person::PhoneNumber::Type::HOME);
    } else {
        phones[0].setType(Person::PhoneNumber::Type::WORK);
    }
    constexpr size_t score_count = 16;
    capnp::List<float, capnp::Kind::PRIMITIVE>::Builder scores = person.initScores(score_count);
    for (int i = 0; i < score_count; ++i) {
        scores.set(i, p.scores[i]);
    }
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

