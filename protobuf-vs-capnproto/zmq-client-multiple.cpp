/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * This file is part of zmqpp.
 * Copyright (c) 2011-2015 Contributors as noted in the AUTHORS file.
 */

#include <zmqpp/zmqpp.hpp>
#include <string>
#include <iostream>
#include "serialization.h"
#include "./capnp/capnp.h"


using namespace std;

int main(int argc, char *argv[]) {
    
    
    const string endpoint = "tcp://localhost:4242";

    // initialize the 0MQ context
    zmqpp::context context;

    // generate a push socket
    zmqpp::socket_type type = zmqpp::socket_type::push;
    zmqpp::socket socket (context, type);

    // open the connection
    cout << "Opening connection to " << endpoint << "..." << endl;
    socket.connect(endpoint);

    const size_t batch_size = 20;
    assert (TEST_SIZE % batch_size == 0);
    const size_t batch_count = TEST_SIZE / batch_size;
    clock_t start, diff;
    start = clock();
    cout << "Filling " << TEST_SIZE << " items with random data..." << endl;
    vector<person_struct> persons = generateRandomData();
    vector<kj::Array<capnp::word>> byte_msgs_capnp(TEST_SIZE);
    cout << "Serializing " << TEST_SIZE << " items for ZeroMQ..." << endl;
    for (size_t i = 0; i < batch_count; ++i) {      
        byte_msgs_capnp[i] = encodeStructsToBytesCapnp(
            persons, i * batch_size, (i+1) * batch_size);
    }
    cout << "Sending " << TEST_SIZE << " items (in " << batch_count
         <<  " batches) over ZeroMQ..." << endl;
    for (int i = 0; i < batch_count; ++i) {
        zmqpp::message msg;
        msg.add_nocopy((byte_msgs_capnp[i].asChars().begin()), byte_msgs_capnp[i].asChars().size());
        socket.send(msg);
    }
    diff = clock() - start;
    
    cout << "Sending " << TEST_SIZE << " items takes "
         << diff / 1000 << "ms (" << TEST_SIZE * 1000 * 1000 / diff
         << " per sec or " << setprecision(2) << 1.0 * diff / TEST_SIZE
         << "us per record)" << endl;
}