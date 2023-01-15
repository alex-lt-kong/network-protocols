#ifndef PROTOBUF_H
#define PROTOBUF_H

#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include "person.pb.h"
#include "../serialization.h"

using namespace std;


person_struct decodeMessageToStructProtoBuf(vector<uint8_t> byte_msg);
vector<uint8_t> encodeStructToBytesProtoBuf(person_struct p);

#endif
