#ifndef PROTOBUF_H
#define PROTOBUF_H

#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include "pb_test.pb.h"
#include "../serialization.h"

using namespace std;

void decodeBytesToStructProtoBuf(string& byte_msg, person_struct& p);
void decodeBytesToStructsProtoBuf(string& byte_msg, vector<person_struct>& p);
void encodeStructToBytesProtoBuf(person_struct& p, string* byte_msg);
void encodeStructsToBytesProtoBuf(vector<person_struct>& p, size_t lower, size_t upper, string* byte_msg);

#endif
