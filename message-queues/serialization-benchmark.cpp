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
#include "./capnp/cp_test.capnp.h"
#include "./capnp/capnp.h"
#include "./protobuf/protobuf.h"


using namespace std;

void benchmarkSingleMessageSerialization() {
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
        decodeBytesToStructCapnp(byte_msgs_capnp[i], deserialized_persons[i]);
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
        decodeBytesToStructProtoBuf(
            byte_msgs_protobuf[i], deserialized_persons[i]);
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
}

void benchmarkMultipleMessagesSerialization(size_t batch_size) {
    assert (TEST_SIZE % batch_size == 0);
    const size_t batch_count = TEST_SIZE / batch_size;
    cout << "Generating random data..." << endl;
    vector<kj::Array<capnp::word>> byte_msgs_capnp{batch_count};
    vector<vector<person_struct>> deserialized_persons{batch_count};
    vector<person_struct> persons = generateRandomData();
    cout << "Random data are prepared\n\n"
         << "===== Testing CapnProto (batch_size: " << batch_size
         << ", batch_count: " << batch_count
         << ")=====" << endl;
    clock_t start, diff;

    start = clock();
    for (size_t i = 0; i < batch_count; ++i) {
        byte_msgs_capnp[i] = encodeStructsToBytesCapnp(
            persons, i * batch_size, (i+1) * batch_size);
    }
    diff = clock() - start;
    
    cout << "Serializing   " << TEST_SIZE << " items takes "
         << diff / 1000 << "ms (" << TEST_SIZE * 1000 * 1000 / diff
         << " per sec or " << setprecision(2) << 1.0 * diff / TEST_SIZE
         << "us per record)" << endl;
    start = clock();
    for (int i = 0; i < batch_count; ++i) {
        deserialized_persons[i] = vector<person_struct>{batch_size};
        decodeBytesToStructsCapnp(byte_msgs_capnp[i], deserialized_persons[i]);
    }
    diff = clock() - start;
    cout << "Deserializing " << TEST_SIZE << " items takes "
         << diff / 1000 << "ms (" << TEST_SIZE * 1000 * 1000 / diff
         << " per sec or " << setprecision(2) << 1.0 * diff / TEST_SIZE
         << "us per record)" << endl;
    cout << "\n--- Sample item ---\n"
         << getStringRepresentationOfPerson(
                deserialized_persons[rand() % deserialized_persons.size()][
                    rand() % batch_size]
            )
         << "\n\n\n"
         << "===== Testing ProtoBuf (batch_size: " << batch_size
         << ", batch_count: " << batch_count
         << ")=====" << endl;

    vector<vector<uint8_t>> byte_msgs_protobuf{batch_count};
    start = clock();
    for (int i = 0; i < batch_count; ++i) { 
        byte_msgs_protobuf[i] = encodeStructsToBytesProtoBuf(
            persons, i * batch_size, (i+1) * batch_size);
    }
    diff = clock() - start;
    
    cout << "Serializing   " << TEST_SIZE << " items takes "
         << diff / 1000 << "ms (" << TEST_SIZE * 1000 * 1000 / diff
         << " per sec or " << setprecision(2) << 1.0 * diff / TEST_SIZE
         << "us per record)" << endl;
    start = clock();
    for (int i = 0; i < batch_count; ++i) {
        deserialized_persons[i] = vector<person_struct>{batch_size};
        decodeBytesToStructsProtoBuf(
            byte_msgs_protobuf[i], deserialized_persons[i]);
    }
    diff = clock() - start;
    cout << "Deserializing " << TEST_SIZE << " items takes "
         << diff / 1000 << "ms (" << TEST_SIZE * 1000 * 1000 / diff
         << " per sec or " << setprecision(2) << 1.0 * diff / TEST_SIZE
         << "us per record)" << endl;
    cout << "\n--- Sample item ---\n"
         << getStringRepresentationOfPerson(deserialized_persons[
            rand() % deserialized_persons.size()][rand() % batch_size])
         << "\n\n" << endl;
}

int main() {
    srand(time(NULL));
    benchmarkSingleMessageSerialization();
    benchmarkMultipleMessagesSerialization(1);
    benchmarkMultipleMessagesSerialization(2);
    benchmarkMultipleMessagesSerialization(4);
    benchmarkMultipleMessagesSerialization(8);
    benchmarkMultipleMessagesSerialization(16);
    benchmarkMultipleMessagesSerialization(200);
    return 0;
}