#include "serialization.h"
#include "./capnp/person.capnp.h"
#include "./capnp/capnp.h"
#include "./protobuf/protobuf.h"

using namespace std;


string getStringRepresentationOfPerson(person_struct p) {
    string str_repr;
    str_repr.reserve(1024);
    str_repr.append("Id: ");
    str_repr.append(to_string(p.id));
    str_repr.append("\nName: ");
    str_repr.append(p.name);
    str_repr.append("\nEmail: ");
    str_repr.append(p.email);
    str_repr.append("\nPhone: ");
    str_repr.append(to_string(p.phone_number));
    if (p.phone_type == 0) {
        str_repr.append("(mobile)");
    } else if (p.phone_type == 1) {
        str_repr.append("(home)");
    } else {
        str_repr.append("(work)");
    }
    
    str_repr.append("\nNationality: ");
    str_repr.append(p.nationality);
    str_repr.append("\nAddress: ");
    str_repr.append(p.address);
    str_repr.append("\nBirthday: ");
    str_repr.append(p.birthday);
    str_repr.append("\nSelf introduction: ");
    str_repr.append(p.self_introduction);
    str_repr.append("\nScores: [");
    for (int i = 0; i< sizeof(p.scores)/sizeof(p.scores[0]); ++i) {        
        str_repr.append(to_string(p.scores[i]));
        str_repr.append(",");
    }
    str_repr.append("]\n");
    return str_repr;
  
}

float getRandomFloat(float a, float b) {
    float random = ((float) rand()) / (float) RAND_MAX;
    float diff = b - a;
    float r = random * diff;
    return a + r;
}
