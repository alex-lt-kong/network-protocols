#include <capnp/message.h>
#include <capnp/serialize-packed.h>
#include <iostream>
#include <iomanip>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <vector>
#include <string>
#include <stdlib.h>
#include <time.h>
#include "./capnp/person.capnp.h"
#include "./capnp/capnp.h"
#include "./protobuf/protobuf.h"


using namespace std;

int main() {
    srand(time(NULL));
    cout << "Generating random data..." << endl;
    vector<kj::Array<capnp::word>> byte_msgs_capnp{TEST_SIZE};
    vector<person_struct> deserialized_persons{TEST_SIZE};
    vector<person_struct> persons = generateRandomData();
    cout << "Random data are prepared\n\n"
         << "===== Testing CapnProto =====" << endl;
    clock_t start, diff;

    start = clock();
    for (int i = 0; i < TEST_SIZE; ++i) {        
        byte_msgs_capnp[i] = encodeStructToBytesCapnp(persons[i]);
    }
    diff = clock() - start;
    
    cout << "Serializing   " << TEST_SIZE << " items takes "
         << diff / 1000 << "ms (" << TEST_SIZE * 1000 * 1000 / diff
         << " per sec or " << setprecision(2) << 1.0 * diff / TEST_SIZE
         << "us per record)" << endl;
    start = clock();
    for (int i = 0; i < TEST_SIZE; ++i) {
        deserialized_persons[i] = decodeMessageToStructCapnp(byte_msgs_capnp[i]);
    }
    diff = clock() - start;
    cout << "Deserializing " << TEST_SIZE << " items takes "
         << diff / 1000 << "ms (" << TEST_SIZE * 1000 * 1000 / diff
         << " per sec or " << setprecision(2) << 1.0 * diff / TEST_SIZE
         << "us per record)" << endl;
    cout << "\n--- Sample item ---\n"
         << getStringRepresentationOfPerson(
             deserialized_persons[rand() % deserialized_persons.size()])
         << "\n\n\n"
         << "===== Testing ProtoBuf =====" << endl;
    vector<vector<uint8_t>> byte_msgs_protobuf{TEST_SIZE};
    start = clock();
    for (int i = 0; i < TEST_SIZE; ++i) {        
        byte_msgs_protobuf[i] = encodeStructToBytesProtoBuf(persons[i]);
    }
    diff = clock() - start;
    
    cout << "Serializing   " << TEST_SIZE << " items takes "
         << diff / 1000 << "ms (" << TEST_SIZE * 1000 * 1000 / diff
         << " per sec or " << setprecision(2) << 1.0 * diff / TEST_SIZE
         << "us per record)" << endl;
    start = clock();
    for (int i = 0; i < TEST_SIZE; ++i) {
        deserialized_persons[i] = decodeMessageToStructProtoBuf(
            byte_msgs_protobuf[i]);
    }
    diff = clock() - start;
    cout << "Deserializing " << TEST_SIZE << " items takes "
         << diff / 1000 << "ms (" << TEST_SIZE * 1000 * 1000 / diff
         << " per sec or " << setprecision(2) << 1.0 * diff / TEST_SIZE
         << "us per record)" << endl;
    cout << "\n--- Sample item ---\n"
         << getStringRepresentationOfPerson(
             deserialized_persons[rand() % deserialized_persons.size()])
         << "\n\n" << endl;


    return 0;
}