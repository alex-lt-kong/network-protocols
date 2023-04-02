#include <zmqpp/zmqpp.hpp>
#include <string>
#include <iostream>
#include <stdio.h>

using namespace std;

int main(void) {
    const string endpoint = "tcp://127.0.0.1:4242";

    zmqpp::context context;
    zmqpp::socket_type type = zmqpp::socket_type::publish;
    zmqpp::socket socket (context, type);
  
    cout << "Binding to endpoint: " << endpoint << "...\n";
    socket.bind(endpoint);
    cout << "Endpoint bound" << endl;

    size_t count = 0;

    while (true) {
        zmqpp::message msg;
        char pl_data[128];
        sprintf(pl_data, "Hello, world! Data are: %lu", count);
        msg.add_nocopy(pl_data, strlen(pl_data) + 1);
        socket.send(msg);
        cout << "[" << count++ << "] data published\n";
        this_thread::sleep_for(std::chrono::milliseconds(20));
    }
    return 0;
}