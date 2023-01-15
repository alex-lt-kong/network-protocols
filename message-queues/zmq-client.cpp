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

  // send a message
  cout << "Sending text and a number..." << endl;
  
  
  
  constexpr size_t TEST_SIZE = 1;
  vector<person_struct> persons = generateRandomData(TEST_SIZE);
  string test = "jello";
  for (int i = 0; i < TEST_SIZE; ++i) {
      auto byte_msg = encodeStructToBytesCapnp(persons[i]);
      cout << byte_msg.size() << endl;
      zmqpp::message_t msg;
      msg.add_raw(byte_msg.asChars().begin(), byte_msg.size());
      cout << msg.size(0) << endl;
      socket.send(msg);
  }
  cout << "Sent message." << endl;
  cout << "Finished." << endl;
}