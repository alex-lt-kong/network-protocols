#include "protobuf.h"

using namespace std;


void decodeBytesToStructsProtoBuf(string& byte_msg, vector<person_struct>& p) {
    pb_test::University uni;
    uni.ParseFromString(byte_msg);

    for (int i = 0; i < uni.students_size(); ++i) {
        const pb_test::Person& person = uni.students(i);

        p[i].id = person.id();    
        p[i].name = person.name();    
        p[i].email = person.email();
        const pb_test::Person::PhoneNumber& phone_number = person.phones(0);
        p[i].phone_number = phone_number.number();
        if (phone_number.type() ==  pb_test::Person::MOBILE) {
            p[i].phone_type = 0;
        } else if (phone_number.type() ==  pb_test::Person::HOME) {
            p[i].phone_type = 1;
        } else {
            p[i].phone_type = 2;
        }    
        p[i].nationality = person.nationality();
        p[i].address = person.address();
        p[i].birthday = person.bitrthday();
        p[i].update_date = person.updatedate();
        p[i].creation_date = person.creationdate();
        p[i].self_introduction = person.selfintroduction();
        for (size_t j = 0; j < person.scores_size(); ++j) {
            p[i].scores[j] = person.scores(j);
        }
    }
}

void decodeBytesToStructProtoBuf(string& byte_msg, person_struct& p) {
    pb_test::Person person;
    person.ParseFromString(byte_msg);
    p.id = person.id();    
    p.name = person.name();    
    p.email = person.email();
    const pb_test::Person::PhoneNumber& phone_number = person.phones(0);
    p.phone_number = phone_number.number();
    if (phone_number.type() ==  pb_test::Person::MOBILE) {
        p.phone_type = 0;
    } else if (phone_number.type() ==  pb_test::Person::HOME) {
        p.phone_type = 1;
    } else {
        p.phone_type = 2;
    }    
    p.nationality = person.nationality();
    p.address = person.address();
    p.birthday = person.bitrthday();
    p.update_date = person.updatedate();
    p.creation_date = person.creationdate();
    p.self_introduction = person.selfintroduction();
    for (size_t i = 0; i < person.scores_size(); ++i) {
        p.scores[i] = person.scores(i);
    }
}

void encodeStructToBytesProtoBuf(person_struct& p, string* byte_msg) {
    pb_test::Person person;
    person.set_id(p.id);
    person.set_name(p.name);
    person.set_email(p.email);
    person.set_address(p.address);
    person.set_nationality(p.nationality);


    pb_test::Person::PhoneNumber* phone_number = person.add_phones();
    phone_number->set_number(p.phone_number);
    if (p.phone_type == 0) {
        phone_number->set_type(pb_test::Person::MOBILE);
    } else if (p.phone_type == 1) {
        phone_number->set_type(pb_test::Person::HOME);
    } else {
        phone_number->set_type(pb_test::Person::WORK);
    }
    
    for (int i = 0; i < sizeof(p.scores)/sizeof(p.scores[0]); ++i) {
        person.add_scores(p.scores[i]);
    }

    person.set_bitrthday(p.birthday);
    person.set_creationdate(p.creation_date);
    person.set_updatedate(p.update_date);
    person.set_selfintroduction(p.self_introduction);

    //person.SerializeToArray(byte_msg.data(), person.ByteSizeLong());
    person.SerializeToString(byte_msg);
    // According to: https://stackoverflow.com/questions/4986673/c11-rvalues-and-move-semantics-confusion-return-statement/4986802#4986802
    // No special treatment is needed--compiler will apply move constructor
    // or other optimization techniques automatically.
}

void encodeStructsToBytesProtoBuf(vector<person_struct>& p, size_t lower, size_t upper, string* byte_msg) {
    pb_test::University uni;
    for (size_t i = lower; i < upper; ++i) {
        pb_test::Person* student = uni.add_students();
        student->set_id(p[i].id);
        student->set_name(p[i].name);
        student->set_email(p[i].email);
        student->set_address(p[i].address);
        student->set_nationality(p[i].nationality);


        pb_test::Person::PhoneNumber* phone_number = student->add_phones();
        phone_number->set_number(p[i].phone_number);
        if (p[i].phone_type == 0) {
            phone_number->set_type(pb_test::Person::MOBILE);
        } else if (p[i].phone_type == 1) {
            phone_number->set_type(pb_test::Person::HOME);
        } else {
            phone_number->set_type(pb_test::Person::WORK);
        }
        
        for (int j = 0; j < sizeof(p[i].scores)/sizeof(p[i].scores[0]); ++j) {
            student->add_scores(p[i].scores[j]);
        }

        student->set_bitrthday(p[i].birthday);
        student->set_creationdate(p[i].creation_date);
        student->set_updatedate(p[i].update_date);
        student->set_selfintroduction(p[i].self_introduction);

    }
    
    uni.SerializeToString(byte_msg);
    // According to: https://stackoverflow.com/questions/4986673/c11-rvalues-and-move-semantics-confusion-return-statement/4986802#4986802
    // No special treatment is needed--compiler will apply move constructor
    // or other optimization techniques automatically.
}
