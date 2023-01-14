#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include "person.pb.h"
#include "../rpc.h"

using namespace std;


string decodeMessageToStructProtoBuf(vector<uint8_t> byte_msg) {
    string str_repr;
    str_repr.reserve(256);
    pb_test::Person person;
    

    if (!person.ParseFromArray(byte_msg.data(), byte_msg.size())) {
      cerr << "Failed to parse person_." << endl;
      return "";
    }
    str_repr.append("Id: ");
    str_repr.append(to_string(person.id()));
    str_repr.append("\nName: ");
    str_repr.append(person.name());
    str_repr.append("\nnEmail: ");
    str_repr.append(person.email());
    str_repr.append("\nNationality: ");
    str_repr.append(person.nationality());
    str_repr.append("\nAddress: ");
    str_repr.append(person.address());
    str_repr.append("\nBirthday: ");
    str_repr.append(person.bitrthday());
    str_repr.append("\nSelf introduction: ");
    str_repr.append(person.selfintroduction());

    return str_repr;

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
    phone_number->set_type(pb_test::Person::MOBILE);

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
