#include "test_message.pb.h"

#include <boost/asio.hpp>
#include <cxxopts.hpp>
#include <spdlog/async.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

#include <chrono>
#include <iostream>
#include <print>

using boost::asio::ip::udp;

int main(const int argc, char *argv[]) {
    cxxopts::Options options(argv[0], "udp multicast sender");
    // clang-format off
    options.add_options()
      ("h,help", "print help message")
      ("i,interface", "Bind to this interface for sending multicast packets", cxxopts::value<std::string>())
      ("a,address", "Send multicast packets to this address", cxxopts::value<std::string>()->default_value("239.255.0.1"))
      ("p,port", "Port", cxxopts::value<int>());
    // clang-format on
    auto result = options.parse(argc, argv);
    if (result.count("help")) {
        std::println("{}", options.help());
        return 0;
    }
    std::string interface_ip;
    int port;
    std::string multicast_address;
    try {
        interface_ip = result["interface"].as<std::string>();
        port = result["port"].as<int>();
        multicast_address = result["address"].as<std::string>();
    } catch (const cxxopts::exceptions::exception &e) {
        std::println("Error: {}", e.what());
        std::println("{}", options.help());
        return 1;
    }
    spdlog::init_thread_pool(64, 1);
    auto stdout_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    auto lgr = std::make_shared<spdlog::async_logger>("", stdout_sink, spdlog::thread_pool(),
                                                      spdlog::async_overflow_policy::overrun_oldest);
    lgr->set_pattern("%Y-%m-%dT%T.%f%z|%5t|%8l| %v");

    try {

        boost::asio::io_context io_context;

        // udp::socket socket(io_context, udp::endpoint(udp::v4(), 0));
        const udp::endpoint multicast_endpoint(boost::asio::ip::make_address(multicast_address), port);
        udp::socket socket(io_context, udp::v4());
        socket.set_option(
                boost::asio::ip::multicast::outbound_interface(boost::asio::ip::make_address(interface_ip).to_v4()));

        uint32_t seq_num = 0;
        long prev_unix_time_us = 0;
        uint32_t prev_seq_num = 0;
        constexpr long intervals_us = 5'000'000;
        while (true) {

            boost_udp_multicast_poc::TestMessage msg;
            auto now = std::chrono::system_clock::now();
            const long unix_time_us =
                    std::chrono::duration_cast<std::chrono::microseconds>(now.time_since_epoch()).count();
            msg.set_unix_time_us(unix_time_us);
            msg.set_seq_num(seq_num++);
            msg.set_is_active(seq_num % 2 == 0);

            auto sent_size = socket.send_to(boost::asio::buffer(msg.SerializeAsString()), multicast_endpoint);

            if (unix_time_us - prev_unix_time_us > intervals_us) {
                lgr->info("{} msg/sec sent. Latest msg: sent_size={}, seq_num={}, timestamp_us={}",
                          (seq_num - prev_seq_num) / ((unix_time_us - prev_unix_time_us) / 1'000'000), sent_size,
                          msg.seq_num(), msg.unix_time_us());
                prev_seq_num = seq_num;
                prev_unix_time_us = unix_time_us;
            }
        }
    } catch (std::exception &e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }

    return 0;
}
