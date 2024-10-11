#include <iostream>
#include <stdio.h>
#include <string>
#include <thread>
#include <zmq.hpp>

using namespace std;

int main(void) {
  const string endpoint = "tcp://127.0.0.1:4242";

  zmq::context_t context;
  zmq::socket_t socket(context, zmq::socket_type::pub);

  cout << "Binding to endpoint: " << endpoint << "...\n";
  socket.bind(endpoint);
  cout << "Endpoint bound" << endl;

  size_t count = 0;
  char pl_data[128];
  while (true) {
    sprintf(pl_data, "Hello, world! Data are: %lu", count);
    zmq::message_t msg(pl_data, strlen(pl_data) + 1);
    if (socket.send(msg, 0)) {
      cout << "[" << count++ << "] data published\n";
    } else {
      cerr << "Error publishing data" << endl;
    }
    this_thread::sleep_for(std::chrono::milliseconds(1));
  }
  return 0;
}