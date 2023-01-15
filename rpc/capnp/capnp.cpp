#include "capnp.h"

using namespace std;

person_struct decodeMessageToStructCapnp(kj::ArrayPtr<char> encoded_arr) {
    person_struct p;
    auto encoded_array_ptr = encoded_arr;
    auto encoded_char_array = encoded_array_ptr.begin();
    auto size = encoded_array_ptr.size();

    auto received_array = kj::ArrayPtr<capnp::word>(reinterpret_cast<capnp::word*>(encoded_char_array), size/sizeof(capnp::word));
    capnp::FlatArrayMessageReader message_receiver_builder(received_array);
    Person::Reader person = message_receiver_builder.getRoot<Person>();
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

