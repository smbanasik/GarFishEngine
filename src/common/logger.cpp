#include <common/logger.hpp>

#include <cassert>
#include <chrono>
#include <format>
#include <fstream>
#include <iostream>
#include <string>

void output_to_file(std::string_view message) {
    return; // TODO: implement
};
void output_to_console(std::string_view message) {
    std::cout << message;
};

com::Logger* com::Logger::loaded_logger = nullptr;

com::Logger::Logger(const LoggerConfig& config) : config(config) {
    assert(loaded_logger == nullptr);
    loaded_logger = this;
};

com::Logger::Logger() : config(LoggerConfig{LoggerConfig::FLAG_DEFAULTS}) {
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

// TODO: consider moving color and timestamp logic out of this string
void com::Logger::log(LogLevel level, std::string_view message) {
    if (static_cast<uint8_t>(level)
            < config.get_bits(LoggerFlags::CONSOLE_LOG_LEVEL_MASK, LoggerFlags::CONSOLE_LOG_LEVEL_SHIFT)
        && static_cast<uint8_t>(level)
               < config.get_bits(LoggerFlags::FILE_LOG_LEVEL_MASK, LoggerFlags::FILE_LOG_LEVEL_SHIFT))
        return;

    std::string_view color = "", reset = "";
    if (config.get_bits(LoggerFlags::COLOR_MASK, LoggerFlags::COLOR_SHIFT)) {
        color = com::log_util::log_level_to_color(level);
        reset = com::log_util::COLOR_RESET;
    }

    std::string formatted_msg = "";
    if (config.get_bits(LoggerFlags::TIMESTAMP_LOG_MASK, LoggerFlags::TIMESTAMP_LOG_SHIFT)) {
        auto time_now = std::chrono::system_clock::now();
        auto formatted_msg
            = std::format("[{:%Y-%m-%d %H:%M:%S}] {}[{}]{} {}\n", time_now, com::log_util::log_level_to_color(level),
                          com::log_util::log_level_to_string(level), com::log_util::COLOR_RESET, message);
    } else {
        auto formatted_msg
            = std::format("{}[{}]{} {}\n", com::log_util::log_level_to_color(level),
                          com::log_util::log_level_to_string(level), com::log_util::COLOR_RESET, message);
    }

    if (config.get_bits(LoggerFlags::OUTPUT_TO_CONSOLE_MASK, LoggerFlags::OUTPUT_TO_CONSOLE_SHIFT)) {
        output_to_console(formatted_msg);
    }
    if (config.get_bits(LoggerFlags::WRITE_TO_FILE_MASK, LoggerFlags::WRITE_TO_FILE_SHIFT)) {
        output_to_file(formatted_msg);
    }
};