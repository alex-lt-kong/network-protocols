#ifndef PROTOBUF_H
#define PROTOBUF_H

#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include "pb_test.pb.h"
#include "../serialization.h"

using namespace std;

void decodeBytesToStructProtoBuf(vector<uint8_t>& byte_msg, person_struct& p);
void decodeBytesToStructsProtoBuf(vector<uint8_t>& byte_msg, vector<person_struct>& p);
vector<uint8_t> encodeStructToBytesProtoBuf(person_struct& p);
vector<uint8_t> encodeStructsToBytesProtoBuf(vector<person_struct>& p, size_t lower, size_t upper);;

#endif
