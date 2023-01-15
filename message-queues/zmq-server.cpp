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
#include "./capnp/capnp.h"
#include "serialization.h"

using namespace std;

int main(int argc, char *argv[]) {
  const string endpoint = "tcp://*:4242";

  // initialize the 0MQ context
  zmqpp::context context;

  // generate a pull socket
  zmqpp::socket_type type = zmqpp::socket_type::pull;
  zmqpp::socket socket (context, type);

  // bind to the socket
  cout << "Binding to " << endpoint << "..." << endl;
  socket.bind(endpoint);
  zmqpp::message_t message;
  vector<person_struct> persons{TEST_SIZE};
  for (size_t i = 0; i < TEST_SIZE; ++i) {    
    // decompose the message 
    socket.receive(message);
    cout << "message.size(): " << message.size(0) << endl;
    auto d = message.get(0);
    auto t = reinterpret_cast<uint8_t*>((char*)(d.data()));
    for (int i = 0; i < message.size(0); ++i)
        printf("%x(%d) ", t[i], t[i]);
    printf("\n");
    auto received_array = kj::ArrayPtr<capnp::word>(reinterpret_cast<capnp::word*>(t), message.size(0) * sizeof(capnp::word));
    capnp::FlatArrayMessageReader msg_builder(received_array);
    Person::Reader person = msg_builder.getRoot<Person>();
    cout << "message received: person.getName().cStr() [" << person.getEmail().cStr() << "]" << endl;
  }

  //cout << "Received text:\"" << text << "\" and a number: " << number << endl;
  cout << "Finished." << endl;
}

