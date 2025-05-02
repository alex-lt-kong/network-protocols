#include <iostream>
#include <boost/asio.hpp>
#include <thread>
#include <vector>
#include <spdlog/spdlog.h>
#include <iomanip>

using namespace boost::asio;
using namespace std;

void log_received_data(int port, const char* data, size_t length) {
    stringstream hex_stream, str_stream;

    hex_stream << std::hex << std::setfill('0');

    for (size_t i = 0; i < length; ++i) {
        unsigned char c = static_cast<unsigned char>(data[i]);
        hex_stream << std::setw(2) << static_cast<int>(c) << " "; // Format as hex

        if (isprint(c)) {
            str_stream << c; // Print printable characters
        } else {
            str_stream << '.'; // Replace non-printable characters with '.'
        }
    }

    spdlog::info("Received on port {} (Hex   ): [{}]", port, hex_stream.str());
    spdlog::info("Received on port {} (String): [{}]", port, str_stream.str());
}

void handle_connection(ip::tcp::socket& socket, int port) {
    try {
        spdlog::info("Client connected on port {}", port);

        while (true) {
            char buffer[1024];
            boost::system::error_code error;
            size_t bytes_received = socket.read_some(boost::asio::buffer(buffer), error);

            if (error == error::eof) {
                spdlog::info("Connection closed on port {}", port);
                break;
            } else if (error) {
                spdlog::error("Error receiving data on port {}: {}", port, error.message());
                break;
            }
            log_received_data(port, buffer, bytes_received);
        }
    } catch (const std::exception& e) {
        spdlog::error("Connection error on port {}: {}", port, e.what());
    }
}

void listen_on_port(io_context& io_context, int port) {
    try {
        ip::tcp::acceptor acceptor(io_context, ip::tcp::endpoint(ip::tcp::v4(), port));
        spdlog::info("Listening on port {}...", port);

        while (true) {
            ip::tcp::socket socket(io_context);
            acceptor.accept(socket);
            std::thread([socket = std::move(socket), port]() mutable {
                handle_connection(socket, port);
            }).detach();
        }
    } catch (const std::exception& e) {
        spdlog::error("Error on port {}: {}", port, e.what());
    }
}

vector<int> parse_ports(const string& port_string) {
    vector<int> ports;
    stringstream ss(port_string);
    string port;

    while (getline(ss, port, ',')) {
        try {
            ports.push_back(stoi(port));  // Convert string to int
        } catch (const invalid_argument&) {
            spdlog::error("Invalid port number: {}", port);
        }
    }
    return ports;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        spdlog::error("Usage: {} <port_list>", argv[0]);
        return 1;
    }

    string port_string = argv[1];  // Read comma-separated port list
    vector<int> ports = parse_ports(port_string);
    if (ports.empty()) {
        spdlog::error("No valid ports provided.");
        return 1;
    }
    io_context io_context;
    vector<thread> threads;

    for (int port : ports) {
        threads.emplace_back(listen_on_port, ref(io_context), port);
    }

    for (auto& t : threads) {
        t.join();
    }

    return 0;
}