#include "test_message.pb.h"

#include <boost/asio.hpp>
#include <cxxopts.hpp>
#include <spdlog/async.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

#include <print>

using boost::asio::ip::udp;

int main(const int argc, char *argv[]) {
    try {
        cxxopts::Options options(argv[0], "udp multicast sender");
        // clang-format off
        options.add_options()
          ("h,help", "print help message")
          ("m,method", "multicast or unicast", cxxopts::value<std::string>()->default_value("multicast"))
          ("i,interface", "Bind to this interface for receiving packets", cxxopts::value<std::string>())
          ("a,address", "Receive multicast packets from this address", cxxopts::value<std::string>()->default_value("239.255.0.1"))
          ("p,port", "Port", cxxopts::value<int>());
        // clang-format on
        auto result = options.parse(argc, argv);
        if (result.count("help")) {
            std::println("{}", options.help());
            return 0;
        }
        std::string interface_ip;
        int port;
        bool is_multicast = true;
        std::string address;
        try {
            auto method_lower = result["method"].as<std::string>();
            std::ranges::transform(method_lower, method_lower.begin(),
                                   [](const unsigned char c) { return std::tolower(c); });
            is_multicast = method_lower == "multicast";
            if (is_multicast)
                interface_ip = result["interface"].as<std::string>();
            else
                interface_ip = "0.0.0.0";
            port = result["port"].as<int>();
            address = result["address"].as<std::string>();
        } catch (const cxxopts::exceptions::exception &e) {
            std::println("{}", options.help());
            std::println("Error: {}", e.what());
            return 1;
        }


        spdlog::init_thread_pool(64, 1);
        auto stdout_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        auto lgr = std::make_shared<spdlog::async_logger>("", stdout_sink, spdlog::thread_pool(),
                                                          spdlog::async_overflow_policy::overrun_oldest);
        lgr->set_pattern("%Y-%m-%dT%T.%f%z|%5t|%8l| %v");
        lgr->info("Starting receiver with method={}, interface={}, address={}, port={}",
                  is_multicast ? "multicast" : "unicast", interface_ip, address, port);

        boost::asio::io_context io_context;

        boost_udp_multicast_poc::TestMessage msg;
        std::string buffer;
        buffer.resize(1024);

        udp::endpoint listen_endpoint(udp::v4(), port);
        udp::socket socket(io_context);
        socket.open(listen_endpoint.protocol());
        if (is_multicast)
            socket.set_option(
                    boost::asio::ip::multicast::join_group(boost::asio::ip::make_address(address).to_v4(),
                                                           boost::asio::ip::make_address(interface_ip).to_v4()));

        socket.bind(listen_endpoint);
        uint32_t prev_seq_num = 0;
        uint32_t msg_received_count = 0;
        long prev_unix_time_us;
        constexpr long intervals_us = 5'000'000;
        const std::array<std::string, 3> latency_units = {"us", "ms", "s"};
        int latency_unit_idx = 0;
        while (true) {
            size_t length = socket.receive_from(boost::asio::buffer(buffer), listen_endpoint);
            msg.ParseFromString(buffer);
            auto now = std::chrono::system_clock::now();
            auto unix_time_us = std::chrono::duration_cast<std::chrono::microseconds>(now.time_since_epoch()).count();
            ++msg_received_count;
            /* Could be misleading, as the seq_num is not guaranteed to be sequential
            if (msg.seq_num() != prev_seq_num + 1) {
                lgr->arn("Message lost: expected seq_num={}, received seq_num={}", prev_seq_num + 1, msg.seq_num());
            }*/

            prev_seq_num = msg.seq_num();
            if (unix_time_us - prev_unix_time_us > intervals_us) {
                auto latency_val = unix_time_us - msg.unix_time_us();
                if (latency_val > 1'000'000) {
                    latency_val /= 1'000'000;
                    latency_unit_idx = 2;
                } else if (latency_val > 1'000) {
                    latency_val /= 1'000;
                    latency_unit_idx = 1;
                } else {
                    latency_unit_idx = 0;
                }
                lgr->info("{} msg/sec received. Latest msg: length={}, seq_num={}, timestamp_us={}, latency({})={}",
                          msg_received_count / ((unix_time_us - prev_unix_time_us) / 1'000'000), length, msg.seq_num(),
                          msg.unix_time_us(), latency_units[latency_unit_idx], latency_val);
                prev_unix_time_us = unix_time_us;
                msg_received_count = 0;
            }
        }
    } catch (std::exception &e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }

    return 0;
}
