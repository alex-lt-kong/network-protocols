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
  
  zmqpp::message_t message;
  vector<person_struct> persons{TEST_SIZE};
  vector<string> messages{TEST_SIZE};
  for (size_t i = 0; i < TEST_SIZE; ++i) {    
    messages[i].reserve(256);
  }
  cout << "Binding to " << endpoint << "..." << endl;
  socket.bind(endpoint);
  for (size_t i = 0; i < TEST_SIZE; ++i) {
    socket.receive(message);
    messages[i] = message.get(0);
  }

  for (size_t i = 0; i < TEST_SIZE; ++i) {
    auto received_array = kj::ArrayPtr<capnp::word>(
      reinterpret_cast<capnp::word*>((char*)(messages[i].data())),
      messages[i].size() * sizeof(capnp::word)
    );
    decodeBytesToStructCapnp(received_array, persons[i]);
  }


  cout << getStringRepresentationOfPerson(persons[rand() % persons.size()]) << endl;
}

