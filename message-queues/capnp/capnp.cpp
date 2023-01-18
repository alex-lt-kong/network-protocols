#include "capnp.h"

using namespace std;

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
    return messageToFlatArray(msg_builder);
}

