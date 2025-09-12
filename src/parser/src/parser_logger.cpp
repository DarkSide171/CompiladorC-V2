#include "../include/parser_logger.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <chrono>
#include <ctime>
#include <algorithm>

namespace Parser {

// LogEntry implementation
LogEntry::LogEntry(LogLevel level, const std::string& message, 
                   const std::string& file, int line, const std::string& function)
    : level(level), message(message), function(function), file(file), line(line) {
    timestamp = std::chrono::system_clock::now();
}

std::string LogEntry::toString() const {
    std::ostringstream oss;
    
    // Format timestamp
    auto time_t = std::chrono::system_clock::to_time_t(timestamp);
    auto tm = *std::localtime(&time_t);
    
    oss << "[" << std::put_time(&tm, "%Y-%m-%d %H:%M:%S") << "] ";
    
    // Add log level
    switch (level) {
        case LogLevel::TRACE: oss << "[TRACE] "; break;
        case LogLevel::DEBUG: oss << "[DEBUG] "; break;
        case LogLevel::INFO:  oss << "[INFO]  "; break;
        case LogLevel::WARN:  oss << "[WARN]  "; break;
        case LogLevel::ERROR: oss << "[ERROR] "; break;
        case LogLevel::FATAL: oss << "[FATAL] "; break;
    }
    
    // Add message
    oss << message;
    
    // Add location info for debug builds
    if (!file.empty() && line > 0) {
        oss << " (" << file << ":" << line;
        if (!function.empty()) {
            oss << " in " << function;
        }
        oss << ")";
    }
    
    return oss.str();
}

// ConsoleLogOutput implementation
void ConsoleLogOutput::write(const LogEntry& entry) {
    if (entry.level >= LogLevel::ERROR) {
        std::cerr << entry.toString() << std::endl;
    } else {
        std::cout << entry.toString() << std::endl;
    }
}

void ConsoleLogOutput::flush() {
    std::cout.flush();
    std::cerr.flush();
}

// FileLogOutput implementation
FileLogOutput::FileLogOutput(const std::string& filename) : filename_(filename) {
    file_.open(filename_, std::ios::app);
    if (!file_.is_open()) {
        throw std::runtime_error("Failed to open log file: " + filename_);
    }
}

FileLogOutput::~FileLogOutput() {
    if (file_.is_open()) {
        file_.close();
    }
}

void FileLogOutput::write(const LogEntry& entry) {
    if (file_.is_open()) {
        file_ << entry.toString() << std::endl;
    }
}

void FileLogOutput::flush() {
    if (file_.is_open()) {
        file_.flush();
    }
}

// MemoryLogOutput implementation
void MemoryLogOutput::write(const LogEntry& entry) {
    std::lock_guard<std::mutex> lock(mutex_);
    entries_.push_back(entry);
    
    // Limit memory usage
    if (entries_.size() > maxEntries_) {
        entries_.erase(entries_.begin(), entries_.begin() + (entries_.size() - maxEntries_));
    }
}

void MemoryLogOutput::flush() {
    // No-op for memory output
}

std::vector<LogEntry> MemoryLogOutput::getEntries() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return entries_;
}

std::vector<LogEntry> MemoryLogOutput::getEntries(LogLevel minLevel) const {
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<LogEntry> filtered;
    
    for (const auto& entry : entries_) {
        if (entry.level >= minLevel) {
            filtered.push_back(entry);
        }
    }
    
    return filtered;
}

void MemoryLogOutput::clear() {
    std::lock_guard<std::mutex> lock(mutex_);
    entries_.clear();
}

// ParserLogger implementation
ParserLogger& ParserLogger::getInstance() {
    static ParserLogger instance;
    return instance;
}

ParserLogger::ParserLogger() : currentLogLevel(LogLevel::INFO), loggingEnabled(true) {
    // Initialize with console output by default
    addOutput(std::make_unique<ConsoleLogOutput>());
    resetStatistics();
}

ParserLogger::~ParserLogger() {
    flush();
}

void ParserLogger::log(LogLevel level, const std::string& message,
                      const std::string& file, int line, const std::string& function) {
    if (!loggingEnabled || level < currentLogLevel) {
        return;
    }
    
    LogEntry entry(level, message, file, line, function);
    updateStatistics(entry);
    
    for (auto& output : outputs) {
        output->write(entry);
    }
}



void ParserLogger::addOutput(std::unique_ptr<LogOutput> output) {
    outputs.push_back(std::move(output));
}

void ParserLogger::clearOutputs() {
    outputs.clear();
}

void ParserLogger::removeConsoleOutput() {
    outputs.erase(
        std::remove_if(outputs.begin(), outputs.end(),
            [](const std::unique_ptr<LogOutput>& output) {
                return dynamic_cast<ConsoleLogOutput*>(output.get()) != nullptr;
            }),
        outputs.end());
}

void ParserLogger::removeFileOutputs() {
    outputs.erase(
        std::remove_if(outputs.begin(), outputs.end(),
            [](const std::unique_ptr<LogOutput>& output) {
                return dynamic_cast<FileLogOutput*>(output.get()) != nullptr;
            }),
        outputs.end());
}

void ParserLogger::addConsoleOutput(bool useColors) {
    auto consoleOutput = std::make_unique<ConsoleLogOutput>();
    // Note: Color support could be implemented in ConsoleLogOutput class
    // For now, we store the preference but don't use it
    (void)useColors; // Acknowledge parameter until color support is implemented
    addOutput(std::move(consoleOutput));
}

void ParserLogger::addFileOutput(const std::string& filename, bool append) {
    // Create file output with append mode consideration
    auto fileOutput = std::make_unique<FileLogOutput>(filename);
    // Note: Append mode could be implemented in FileLogOutput constructor
    // For now, we acknowledge the parameter
    (void)append; // Acknowledge parameter until append mode is implemented
    addOutput(std::move(fileOutput));
}

void ParserLogger::addMemoryOutput(size_t maxEntries) {
    addOutput(std::make_unique<MemoryLogOutput>(maxEntries));
}

void ParserLogger::flush() {
    for (auto& output : outputs) {
        output->flush();
    }
}

std::string ParserLogger::levelToString(LogLevel level) const {
    switch (level) {
        case LogLevel::TRACE: return "TRACE";
        case LogLevel::DEBUG: return "DEBUG";
        case LogLevel::INFO: return "INFO";
        case LogLevel::WARN: return "WARN";
        case LogLevel::ERROR: return "ERROR";
        case LogLevel::FATAL: return "FATAL";
        default: return "UNKNOWN";
    }
}

LogLevel ParserLogger::stringToLevel(const std::string& levelStr) const {
    if (levelStr == "TRACE") return LogLevel::TRACE;
    if (levelStr == "DEBUG") return LogLevel::DEBUG;
    if (levelStr == "INFO") return LogLevel::INFO;
    if (levelStr == "WARN") return LogLevel::WARN;
    if (levelStr == "ERROR") return LogLevel::ERROR;
    if (levelStr == "FATAL") return LogLevel::FATAL;
    return LogLevel::INFO; // default
}

// Parser-specific logging methods
void ParserLogger::logParseStart(const std::string& ruleName, const std::string& tokenInfo) {
    std::ostringstream oss;
    oss << "[PARSE START] " << ruleName;
    if (!tokenInfo.empty()) {
        oss << " (token: " << tokenInfo << ")";
    }
    debug(oss.str());
}

void ParserLogger::logParseEnd(const std::string& ruleName, bool success, const std::string& result) {
    std::ostringstream oss;
    oss << "[PARSE END] " << ruleName << " - " << (success ? "SUCCESS" : "FAILED");
    if (!result.empty()) {
        oss << " (" << result << ")";
    }
    if (success) {
        debug(oss.str());
    } else {
        warning(oss.str());
    }
}

void ParserLogger::logParseError(const std::string& ruleName, const std::string& error, const std::string& context) {
    std::ostringstream oss;
    oss << "[PARSE ERROR] " << ruleName << ": " << error;
    if (!context.empty()) {
        oss << " (context: " << context << ")";
    }
    this->error(oss.str());
}

void ParserLogger::logTokenConsumption(const std::string& tokenType, const std::string& tokenValue) {
    std::ostringstream oss;
    oss << "[TOKEN] Consumed " << tokenType;
    if (!tokenValue.empty()) {
        oss << " '" << tokenValue << "'";
    }
    trace(oss.str());
}

void ParserLogger::logBacktrack(const std::string& reason, int position) {
    std::ostringstream oss;
    oss << "[BACKTRACK] " << reason << " (position: " << position << ")";
    debug(oss.str());
}

void ParserLogger::logRecovery(const std::string& strategy, const std::string& context) {
    std::ostringstream oss;
    oss << "[RECOVERY] Strategy: " << strategy;
    if (!context.empty()) {
        oss << " (" << context << ")";
    }
    info(oss.str());
}



void ParserLogger::resetStatistics() {
    stats = LogStats{};
}

void ParserLogger::updateStatistics(const LogEntry& entry) {
    stats.totalEntries++;
    stats.entriesByLevel[static_cast<int>(entry.level)]++;
    
    if (stats.totalEntries == 1) {
        stats.firstEntry = entry.timestamp;
    }
    stats.lastEntry = entry.timestamp;
}



// Utility functions
std::string logLevelToString(LogLevel level) {
    switch (level) {
        case LogLevel::TRACE: return "TRACE";
        case LogLevel::DEBUG: return "DEBUG";
        case LogLevel::INFO:  return "INFO";
        case LogLevel::WARN:  return "WARN";
        case LogLevel::ERROR: return "ERROR";
        case LogLevel::FATAL: return "FATAL";
        default: return "UNKNOWN";
    }
}

LogLevel stringToLogLevel(const std::string& levelStr) {
    std::string upper = levelStr;
    std::transform(upper.begin(), upper.end(), upper.begin(), ::toupper);
    
    if (upper == "TRACE") return LogLevel::TRACE;
    if (upper == "DEBUG") return LogLevel::DEBUG;
    if (upper == "INFO")  return LogLevel::INFO;
    if (upper == "WARN")  return LogLevel::WARN;
    if (upper == "ERROR") return LogLevel::ERROR;
    if (upper == "FATAL") return LogLevel::FATAL;
    
    return LogLevel::INFO; // Default
}

// ScopedTimer implementation
ParserLogger::ScopedTimer::ScopedTimer(const std::string& operation, LogLevel level)
    : operationName(operation), logLevel(level) {
    startTime = std::chrono::high_resolution_clock::now();
    ParserLogger::getInstance().log(logLevel, "Starting: " + operationName);
}

ParserLogger::ScopedTimer::~ScopedTimer() {
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    
    std::ostringstream oss;
    oss << "Completed: " << operationName << " (" << duration.count() << "ms)";
    ParserLogger::getInstance().log(logLevel, oss.str());
}

} // namespace Parser