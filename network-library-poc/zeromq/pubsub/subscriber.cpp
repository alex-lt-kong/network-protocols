#include <string>
#include <iostream>
#include <thread>

#include <zmq.hpp>

using namespace std;

int main(int argc, char *argv[]) {
    if (argc != 2) {
        cerr << "Usage ./subscriber.out <interval-between-recv()-in-ms>" << endl;
        cerr << "Set this to a large number to test the behavior of a slow subscriber" << endl;
        return EXIT_FAILURE;
    }
    const string endpoint = "tcp://localhost:4242";

    zmq::context_t context;
    zmq::socket_t socket(context, zmq::socket_type::sub);

    socket.setsockopt(ZMQ_SUBSCRIBE, "", 0);

    /* This ensures that if a subscriber is too slow, it skips all the
    messages in the queue and directly jump to the latest one.*/
    int just_one = 1;
    socket.setsockopt(ZMQ_CONFLATE, &just_one, sizeof(just_one));
    socket.setsockopt(ZMQ_RCVHWM, &just_one, sizeof(just_one));

    cout << "Connecting to endpoint: " << endpoint << "...\n";
    socket.connect(endpoint);
    cout << "Endpoint connected" << endl;


    while (true) {
        zmq::message_t message;
        if (socket.recv(&message)) {
            cout << message << endl;
        } else {
            cerr << "Error receiving data" << endl;
        }
        this_thread::sleep_for(std::chrono::milliseconds(atoi(argv[1])));
    }
    return 0;
}

