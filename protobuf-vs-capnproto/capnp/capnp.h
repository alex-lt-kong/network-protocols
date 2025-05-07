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
#include "cp_test.capnp.h"

using namespace std;

template <typename T>
void decodeBytesToStructCapnp(T &byte_msg, person_struct& p) {

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
    int i = 0;
    for (float score: person.getScores()) {        
        p.scores[i] = score;
        ++i;
    }
}

template <typename T>
void decodeBytesToStructsCapnp(T &byte_msg, vector<person_struct>& persons) {

    capnp::FlatArrayMessageReader msg_builder(byte_msg);
    University::Reader uni = msg_builder.getRoot<University>();
    for (size_t i = 0; i < uni.getStudents().size(); ++i) {
        Person::Reader student = uni.getStudents()[i];
        persons[i].id = student.getId();
        persons[i].name = student.getName().cStr();
        persons[i].email = student.getEmail().cStr();
        for (Person::PhoneNumber::Reader phone: student.getPhones()) {
            persons[i].phone_number = phone.getNumber();
            
            if (phone.getType() == Person::PhoneNumber::Type::MOBILE) {
                persons[i].phone_type = 0;
            } else if (phone.getType() == Person::PhoneNumber::Type::HOME) {
                persons[i].phone_type = 1;
            } else {
                persons[i].phone_type = 2;
            }
        }
        persons[i].nationality = student.getNationality().cStr();
        persons[i].address = student.getAddress().cStr();
        persons[i].birthday = student.getBitrthday().cStr();
        persons[i].update_date = student.getUpdateDate().cStr();
        persons[i].creation_date = student.getCreationDate().cStr();
        persons[i].self_introduction = student.getSelfIntroduction().cStr();
        for (size_t j = 0; j < student.getScores().size(); ++j) {
            persons[i].scores[j] = student.getScores()[j];
        }
    }
}

kj::Array<capnp::word> encodeStructToBytesCapnp(person_struct& p);
kj::Array<capnp::word> encodeStructsToBytesCapnp(vector<person_struct>& p, size_t lower, size_t upper);
