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
#include "../serialization.h"
#include "person.capnp.h"

using namespace std;

template <typename T>
person_struct decodeMessageToStructCapnp(T &byte_msg) {
    person_struct p;

    capnp::FlatArrayMessageReader msg_builder(byte_msg);
    Person::Reader person = msg_builder.getRoot<Person>();
    p.id = person.getId();
    p.name = person.getName().cStr();
    p.email = person.getEmail().cStr();
    for (Person::PhoneNumber::Reader phone: person.getPhones()) {
        p.phone_number = phone.getNumber();
        
        if (phone.getType() == Person::PhoneNumber::Type::MOBILE) {
            p.phone_type = 0;
        } else if (phone.getType() == Person::PhoneNumber::Type::HOME) {
            p.phone_type = 1;
        } else {
            p.phone_type = 2;
        }
    }
    p.nationality = person.getNationality().cStr();
    p.address = person.getAddress().cStr();
    p.birthday = person.getBitrthday().cStr();
    p.update_date = person.getUpdateDate().cStr();
    p.creation_date = person.getCreationDate().cStr();
    p.self_introduction = person.getSelfIntroduction().cStr();
    double sum = 0;
    int i = 0;
    for (float score: person.getScores()) {        
        p.scores[i] = score;
        ++i;
    }
    return p;  
}

kj::Array<capnp::word> encodeStructToBytesCapnp(person_struct p);
