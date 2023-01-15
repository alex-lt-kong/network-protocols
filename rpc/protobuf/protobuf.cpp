#include "protobuf.h"

using namespace std;


person_struct decodeMessageToStructProtoBuf(vector<uint8_t> byte_msg) {
    person_struct p;
    pb_test::Person person;
    

    if (!person.ParseFromArray(byte_msg.data(), byte_msg.size())) {
      cerr << "Failed to parse person_." << endl;
      return p;
    }
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
    return p;
}

vector<uint8_t> encodeStructToBytesProtoBuf(person_struct p) {
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
        person.add_scores(p.scores[0]);
    }

    person.set_bitrthday(p.birthday);
    person.set_creationdate(p.creation_date);
    person.set_updatedate(p.update_date);
    person.set_selfintroduction(p.self_introduction);

    vector<uint8_t> byte_msg(person.ByteSizeLong()); 
    person.SerializeToArray(byte_msg.data(), person.ByteSizeLong());
    return byte_msg;
    // According to: https://stackoverflow.com/questions/4986673/c11-rvalues-and-move-semantics-confusion-return-statement/4986802#4986802
    // No special treatment is needed--compiler will apply move constructor
    // or other optimization techniques automatically.
}
