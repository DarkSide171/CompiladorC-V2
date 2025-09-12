#ifndef PARSER_LOGGER_HPP
#define PARSER_LOGGER_HPP

#include "parser_types.hpp"
#include <string>
#include <fstream>
#include <memory>
#include <vector>
#include <chrono>
#include <mutex>

namespace Parser {

// Log levels
enum class LogLevel {
    TRACE = 0,
    DEBUG = 1,
    INFO = 2,
    WARN = 3,
    ERROR = 4,
    FATAL = 5
};

// Log entry structure
struct LogEntry {
    std::chrono::system_clock::time_point timestamp;
    LogLevel level;
    std::string message;
    std::string function;
    std::string file;
    int line;
    
    LogEntry(LogLevel level, const std::string& message, 
             const std::string& file = "", int line = 0, const std::string& function = "");
    
    std::string toString() const;
};

// Abstract log output interface
class LogOutput {
public:
    virtual ~LogOutput() = default;
    virtual void write(const LogEntry& entry) = 0;
    virtual void flush() = 0;
};

// Console log output
class ConsoleLogOutput : public LogOutput {
public:
    ConsoleLogOutput() = default;
    
    void write(const LogEntry& entry) override;
    void flush() override;
};

// File log output
class FileLogOutput : public LogOutput {
public:
    FileLogOutput(const std::string& filename);
    ~FileLogOutput();
    
    void write(const LogEntry& entry) override;
    void flush() override;
    
private:
    std::string filename_;
    std::ofstream file_;
};

// Memory log output (for testing)
class MemoryLogOutput : public LogOutput {
public:
    MemoryLogOutput(size_t maxEntries = 1000) : maxEntries_(maxEntries) {}
    
    void write(const LogEntry& entry) override;
    void flush() override;
    
    std::vector<LogEntry> getEntries() const;
    std::vector<LogEntry> getEntries(LogLevel minLevel) const;
    void clear();
    
private:
    mutable std::mutex mutex_;
    std::vector<LogEntry> entries_;
    size_t maxEntries_;
};

// Main logger class
class ParserLogger {
public:
    ParserLogger();
    ~ParserLogger();
    
    // Singleton access
    static ParserLogger& getInstance();
    
    // Configuration
    void setLogLevel(LogLevel level) { currentLogLevel = level; }
    LogLevel getLogLevel() const { return currentLogLevel; }
    
    void setEnabled(bool enabled) { loggingEnabled = enabled; }
    bool isEnabled() const { return loggingEnabled; }
    
    // Output management
    void addOutput(std::unique_ptr<LogOutput> output);
    void clearOutputs();
    void removeConsoleOutput();
    void removeFileOutputs();
    
    // Convenience methods for adding outputs
    void addConsoleOutput(bool useColors = true);
    void addFileOutput(const std::string& filename, bool append = true);
    void addMemoryOutput(size_t maxEntries = 1000);
    
    // Logging methods
    void log(LogLevel level, const std::string& message, 
             const std::string& file = "", int line = 0, const std::string& function = "");
    
    void trace(const std::string& message, const std::string& function = "", 
               const std::string& file = "", int line = 0) {
        log(LogLevel::TRACE, message, file, line, function);
    }
    
    void debug(const std::string& message, const std::string& function = "", 
               const std::string& file = "", int line = 0) {
        log(LogLevel::DEBUG, message, file, line, function);
    }
    
    void info(const std::string& message, const std::string& function = "", 
              const std::string& file = "", int line = 0) {
        log(LogLevel::INFO, message, file, line, function);
    }
    
    void warning(const std::string& message, const std::string& function = "", 
                 const std::string& file = "", int line = 0) {
        log(LogLevel::WARN, message, file, line, function);
    }
    
    void error(const std::string& message, const std::string& function = "", 
               const std::string& file = "", int line = 0) {
        log(LogLevel::ERROR, message, file, line, function);
    }
    
    void fatal(const std::string& message, const std::string& function = "", 
               const std::string& file = "", int line = 0) {
        log(LogLevel::FATAL, message, file, line, function);
    }
    
    // Utility methods
    void flush();
    std::string levelToString(LogLevel level) const;
    LogLevel stringToLevel(const std::string& levelStr) const;
    
    // Parser-specific logging methods
    void logParseStart(const std::string& ruleName, const std::string& tokenInfo = "");
    void logParseEnd(const std::string& ruleName, bool success, const std::string& result = "");
    void logParseError(const std::string& ruleName, const std::string& error, const std::string& context = "");
    void logTokenConsumption(const std::string& tokenType, const std::string& tokenValue);
    void logBacktrack(const std::string& reason, int position);
    void logRecovery(const std::string& strategy, const std::string& context);
    
    // Performance logging
    class ScopedTimer {
    public:
        ScopedTimer(const std::string& operation, LogLevel level = LogLevel::DEBUG);
        ~ScopedTimer();
        
    private:
        std::string operationName;
        LogLevel logLevel;
        std::chrono::high_resolution_clock::time_point startTime;
    };
    
    // Statistics
    struct LogStats {
        size_t totalEntries = 0;
        size_t entriesByLevel[6] = {0}; // One for each LogLevel
        std::chrono::system_clock::time_point firstEntry;
        std::chrono::system_clock::time_point lastEntry;
    };
    
    LogStats getStatistics() const { return stats; }
    void resetStatistics();
    
private:
    LogLevel currentLogLevel;
    bool loggingEnabled;
    std::vector<std::unique_ptr<LogOutput>> outputs;
    LogStats stats;
    
    // Thread safety (if needed in the future)
    // std::mutex logMutex;
    
    void updateStatistics(const LogEntry& entry);
};

// Convenience macros for logging with automatic function/file/line info
#define PARSER_LOG_TRACE(msg) \
    Parser::ParserLogger::getInstance().trace(msg, __FUNCTION__, __FILE__, __LINE__)

#define PARSER_LOG_DEBUG(msg) \
    Parser::ParserLogger::getInstance().debug(msg, __FUNCTION__, __FILE__, __LINE__)

#define PARSER_LOG_INFO(msg) \
    Parser::ParserLogger::getInstance().info(msg, __FUNCTION__, __FILE__, __LINE__)

#define PARSER_LOG_WARNING(msg) \
    Parser::ParserLogger::getInstance().warning(msg, __FUNCTION__, __FILE__, __LINE__)

#define PARSER_LOG_ERROR(msg) \
    Parser::ParserLogger::getInstance().error(msg, __FUNCTION__, __FILE__, __LINE__)

#define PARSER_LOG_FATAL(msg) \
    Parser::ParserLogger::getInstance().fatal(msg, __FUNCTION__, __FILE__, __LINE__)

// Scoped timer macro
#define PARSER_TIMER(operation) \
    Parser::ParserLogger::ScopedTimer timer(operation)

} // namespace Parser

#endif // PARSER_LOGGER_HPP