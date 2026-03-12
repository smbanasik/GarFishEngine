#ifndef COMMON_LOGGER_HPP
#define COMMON_LOGGER_HPP

#include <array>
#include <stdint.h>
#include <string>
#include <string_view>

namespace com {
enum class LogLevel {
    DEBUG,
    INFO,
    WARN,
    ERROR
};

namespace log_util {
constexpr std::array<std::string_view, 4> LOG_LEVEL_STRINGS{"DEBUG", "INFO", "WARN", "ERROR"};

constexpr std::array<std::string_view, 4> LOG_LEVEL_COLORS{"\033[90m", "\033[37m", "\033[33m", "\033[31m"};
constexpr std::string_view COLOR_RESET = "\033[0m";

constexpr std::string_view log_level_to_string(LogLevel l) {
    return LOG_LEVEL_STRINGS[static_cast<size_t>(l)];
}

constexpr std::string_view log_level_to_color(LogLevel l) {
    return LOG_LEVEL_COLORS[static_cast<size_t>(l)];
}
} // namespace log_util
namespace LoggerFlags {
constexpr uint8_t WRITE_TO_FILE_SHIFT = 0u;
constexpr uint8_t FILE_LOG_LEVEL_SHIFT = 1u;
constexpr uint8_t OUTPUT_TO_CONSOLE_SHIFT = 3u;
constexpr uint8_t CONSOLE_LOG_LEVEL_SHIFT = 4u;
constexpr uint8_t TIMESTAMP_LOG_SHIFT = 6u;
constexpr uint8_t COLOR_SHIFT = 7u;

constexpr uint8_t WRITE_TO_FILE_MASK = 0b1u << WRITE_TO_FILE_SHIFT;
constexpr uint8_t FILE_LOG_LEVEL_MASK = 0b11u << FILE_LOG_LEVEL_SHIFT;
constexpr uint8_t OUTPUT_TO_CONSOLE_MASK = 0b1u << OUTPUT_TO_CONSOLE_SHIFT;
constexpr uint8_t CONSOLE_LOG_LEVEL_MASK = 0b11u << CONSOLE_LOG_LEVEL_SHIFT;
constexpr uint8_t TIMESTAMP_LOG_MASK = 0b1u << TIMESTAMP_LOG_SHIFT;
constexpr uint8_t COLOR_MASK = 0b1u << COLOR_SHIFT;
} // namespace LoggerFlags

struct LoggerConfig {
    // No file, console info+, color and timestamp
    static constexpr uint8_t FLAG_DEFAULTS = 0b11011110;

    uint8_t flags = FLAG_DEFAULTS;

    void set_write_to_file(uint8_t flag) {
        reset_bits(LoggerFlags::WRITE_TO_FILE_MASK);
        flags = (flags | ((flag & 0b1u) << LoggerFlags::WRITE_TO_FILE_SHIFT));
    };

    void set_output_to_console(uint8_t flag) {
        reset_bits(LoggerFlags::OUTPUT_TO_CONSOLE_MASK);
        flags = (flags | ((flag & 0b1u) << LoggerFlags::OUTPUT_TO_CONSOLE_SHIFT));
    }

    void set_output_timestamp(uint8_t flag) {
        reset_bits(LoggerFlags::TIMESTAMP_LOG_MASK);
        flags = (flags | ((flag & 0b1u) << LoggerFlags::TIMESTAMP_LOG_SHIFT));
    }

    void set_file_log_level(uint8_t level) {
        reset_bits(LoggerFlags::FILE_LOG_LEVEL_MASK);
        flags = (flags | ((level & 0b11u) << LoggerFlags::FILE_LOG_LEVEL_SHIFT));
    }

    void set_console_log_level(uint8_t level) {
        reset_bits(LoggerFlags::CONSOLE_LOG_LEVEL_MASK);
        flags = (flags | ((level & 0b11u) << LoggerFlags::CONSOLE_LOG_LEVEL_SHIFT));
    }

    void set_color_present(uint8_t flag) {
        reset_bits(LoggerFlags::COLOR_MASK);
        flags = (flags | ((flag & 0b1u) << LoggerFlags::COLOR_SHIFT));
    }

    uint8_t get_bits(uint8_t mask, uint8_t shift) const { return (flags & mask) >> shift; }

    void reset_bits(uint8_t mask) { flags = flags & ~mask; }
};

class Logger {
  public:
    Logger(const LoggerConfig& config);
    Logger();
    ~Logger() {};

    static Logger& get();

    void debug(std::string_view message);
    void info(std::string_view message);
    void warn(std::string_view message);
    void error(std::string_view message);

  private:
    static Logger* loaded_logger;
    Logger(const Logger& other) = delete;
    Logger& operator=(const Logger& other) = delete;

    LoggerConfig config;

    void log(LogLevel level, std::string_view message);
};

} // namespace com
#endif