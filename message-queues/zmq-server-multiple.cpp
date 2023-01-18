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

  const size_t batch_size = 20;
  assert (TEST_SIZE % batch_size == 0);
  const size_t batch_count = TEST_SIZE / batch_size;
  
  zmqpp::message_t message;
  vector<vector<person_struct>> persons{batch_count};
  vector<string> messages{batch_count};
  for (size_t i = 0; i < batch_count; ++i) {    
    messages[i].reserve(256);
  }
  cout << "Binding to " << endpoint << "..." << endl;
  socket.bind(endpoint);
  for (size_t i = 0; i < batch_count; ++i) {
    socket.receive(message);
    messages[i] = message.get(0);
  }

  for (size_t i = 0; i < batch_count; ++i) {
    auto received_array = kj::ArrayPtr<capnp::word>(
      reinterpret_cast<capnp::word*>((char*)(messages[i].data())),
      messages[i].size() * sizeof(capnp::word)
    );
    persons[i] = vector<person_struct>{batch_size};
    decodeBytesToStructsCapnp(received_array, persons[i]);
  }


  cout << getStringRepresentationOfPerson(
    persons[rand() % persons.size()][rand() % batch_size]) << endl;
}

