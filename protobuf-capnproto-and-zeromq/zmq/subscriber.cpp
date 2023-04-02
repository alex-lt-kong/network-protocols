#include <zmqpp/zmqpp.hpp>
#include <string>
#include <iostream>

using namespace std;

int main(void) {
    const string endpoint = "tcp://localhost:4242";

    zmqpp::context context;
    zmqpp::socket_type type = zmqpp::socket_type::subscribe;
    zmqpp::socket socket (context, type);

    socket.subscribe(""); // subscribe the default channel
    
    zmqpp::message_t message;
    cout << "Connecting to endpoint: " << endpoint << "...\n";
    socket.connect(endpoint);
    cout << "Endpoint connected" << endl;
    while (true) {
        socket.receive(message);
        cout << message.get(0)
             << " (size: " << message.size(0) << " bytes)" << endl;
    }
    return 0;
}

