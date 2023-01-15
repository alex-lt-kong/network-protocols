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
  

  vector<person_struct> persons = generateRandomData();
  for (int i = 0; i < TEST_SIZE; ++i) {
      auto byte_msg = encodeStructToBytesCapnp(persons[i]);
      cout << "byte_msg.size(): " << byte_msg.size() << endl;
      zmqpp::message msg;
      msg.add_raw((byte_msg.asChars().begin()), byte_msg.asChars().size());

      for (int i = 0; i < byte_msg.asChars().size(); ++i)
          printf("%x(%d) ", byte_msg.asBytes().begin()[i], byte_msg.asBytes().begin()[i]);
      printf("\n??");
      cout << endl;
      auto d = msg.get(0);
      auto t = reinterpret_cast<uint8_t*>((char*)(d.data()));

      for (int i = 0; i < msg.size(0) * sizeof(capnp::word); ++i)
          printf("%x(%d) ", t[i], t[i]);
      cout << "byte_msg.asChars().size(): " << byte_msg.asChars().size() << " msg.size(0) :" << msg.size(0) << endl;
      auto received_array = kj::ArrayPtr<capnp::word>(reinterpret_cast<capnp::word*>(t), msg.size(0) * sizeof(capnp::word));
    capnp::FlatArrayMessageReader msg_builder(received_array);
    Person::Reader person = msg_builder.getRoot<Person>();
    cout << "message received: person.getName().cStr() [" << person.getName().cStr() << "]" << endl;
    socket.send(msg);
  }
  cout << "Sent message." << endl;
  cout << "Finished." << endl;
}