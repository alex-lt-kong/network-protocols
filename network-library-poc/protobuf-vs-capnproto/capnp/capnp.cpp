#include "capnp.h"

using namespace std;

kj::Array<capnp::word> encodeStructToBytesCapnp(person_struct& p) {
    capnp::MallocMessageBuilder msg_builder;
    // Unlike ProtoBuf (https://developers.google.com/protocol-buffers/docs/cpptutorial#optimization-tips),
    // MessageBuilder should NOT be re-used in CapnProto
    // https://stackoverflow.com/questions/61347404/protobuf-vs-flatbuffers-vs-capn-proto-which-is-faster

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


kj::Array<capnp::word> encodeStructsToBytesCapnp(vector<person_struct>& p, size_t lower, size_t upper) {
    capnp::MallocMessageBuilder msg_builder;
    // Unlike ProtoBuf (https://developers.google.com/protocol-buffers/docs/cpptutorial#optimization-tips),
    // MessageBuilder should NOT be re-used in CapnProto
    // https://stackoverflow.com/questions/61347404/protobuf-vs-flatbuffers-vs-capn-proto-which-is-faster
    
    University::Builder uni = msg_builder.initRoot<University>();
    capnp::List<Person>::Builder students = uni.initStudents(upper - lower);
    for (size_t i = lower; i < upper; ++i) {
        Person::Builder p_builder = students[i-lower];
        p_builder.setId(p[i].id);
        p_builder.setName(p[i].name);
        p_builder.setEmail(p[i].email);
        // Type shown for explanation purposes; normally you'd use auto.
        capnp::List<Person::PhoneNumber>::Builder phones = p_builder.initPhones(1);
        phones[0].setNumber(p[i].phone_number);
        if (p[i].phone_type == 0) {
            phones[0].setType(Person::PhoneNumber::Type::MOBILE);
        } else if (p[i].phone_type == 1) {
            phones[0].setType(Person::PhoneNumber::Type::HOME);
        } else {
            phones[0].setType(Person::PhoneNumber::Type::WORK);
        }
        constexpr size_t score_count = 16;
        capnp::List<float, capnp::Kind::PRIMITIVE>::Builder scores = p_builder.initScores(score_count);
        for (int j = 0; j < score_count; ++j) {
            scores.set(j, p[i].scores[j]);
        }
        p_builder.setNationality(p[i].nationality);
        p_builder.setAddress(p[i].address);
        p_builder.setBitrthday(p[i].birthday);
        p_builder.setCreationDate(p[i].creation_date);
        p_builder.setUpdateDate(p[i].update_date);
        p_builder.setSelfIntroduction(p[i].self_introduction);
    }
    // https://stackoverflow.com/questions/60964979/cap-n-proto-c-serialize-to-char-array-or-any-byte-array
    return messageToFlatArray(msg_builder);
}

