#include <common/logger.hpp>

#include <cassert>
#include <chrono>
#include <string>
#include <iostream>
#include <fstream>
#include <format>

void output_to_file(std::string_view message) {
    return; // TODO: implement
};
void output_to_console(std::string_view message) {
    std::cout << message;
};


com::Logger* com::Logger::loaded_logger = nullptr;

com::Logger::Logger(const LoggerConfig& config) : config(config.flags) {
    assert(loaded_logger == nullptr);
    loaded_logger = this;
};

com::Logger::Logger() : config(LoggerConfig::FLAG_DEFAULTS) {
    assert(loaded_logger == nullptr);
    loaded_logger = this;
};

com::Logger& com::Logger::get() {
    return *loaded_logger;
};

void com::Logger::debug(std::string_view message) {
    log(LogLevel::DEBUG, message);
};

void com::Logger::info(std::string_view message) {
    log(LogLevel::INFO, message);
};

void com::Logger::warn(std::string_view message) {
    log(LogLevel::WARN, message);
};

void com::Logger::error(std::string_view message) {
    log(LogLevel::ERROR, message);
};

void com::Logger::log(LogLevel level, std::string_view message) {
    // If LogLevel < config, return

    // TODO: remove timestamp according to config
    // TODO: allow for no color according to config
    auto time_now = std::chrono::system_clock::now();
    auto formatted_msg = std::format("[{:%Y-%m-%d %H:%M:%S}] {}[{}]{} {}\n", time_now,
        com::log_level_to_color(level),
        com::log_level_to_string(level),
        com::COLOR_RESET,
        message);

    // If output to console
    output_to_console(formatted_msg);
    // If write to file
    // output_to_file(log_message);
};