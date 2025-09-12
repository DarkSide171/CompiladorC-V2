#include "../include/parser_logger.hpp"
#include <iostream>
#include <cassert>
#include <thread>
#include <chrono>

namespace Parser {

void testParserLoggerBasics() {
    std::cout << "Testing ParserLogger basics..." << std::endl;
    
    auto& logger = ParserLogger::getInstance();
    
    // Test singleton
    auto& logger2 = ParserLogger::getInstance();
    assert(&logger == &logger2);
    
    // Test configuration
    logger.setLogLevel(LogLevel::DEBUG);
    assert(logger.getLogLevel() == LogLevel::DEBUG);
    
    logger.setEnabled(true);
    assert(logger.isEnabled() == true);
    
    std::cout << "✓ Basic functionality works" << std::endl;
}

void testParserLoggerOutputs() {
    std::cout << "Testing ParserLogger outputs..." << std::endl;
    
    auto& logger = ParserLogger::getInstance();
    
    // Clear existing outputs
    logger.clearOutputs();
    
    // Add memory output for testing
    logger.addMemoryOutput(100);
    
    // Test basic logging
    logger.info("Test info message");
    logger.warning("Test warning message");
    logger.error("Test error message");
    
    // Test statistics
    auto stats = logger.getStatistics();
    assert(stats.totalEntries >= 3);
    
    std::cout << "✓ Output management works" << std::endl;
}

void testParserSpecificLogging() {
    std::cout << "Testing parser-specific logging..." << std::endl;
    
    auto& logger = ParserLogger::getInstance();
    
    // Clear and add memory output
    logger.clearOutputs();
    logger.addMemoryOutput(100);
    
    // Test parser-specific methods
    logger.logParseStart("expression", "IDENTIFIER");
    logger.logTokenConsumption("IDENTIFIER", "variable");
    logger.logParseEnd("expression", true, "success");
    
    logger.logParseStart("statement");
    logger.logParseError("statement", "unexpected token", "line 10");
    logger.logBacktrack("failed statement", 5);
    logger.logRecovery("skip to semicolon", "statement recovery");
    
    // Verify logging occurred
    auto stats = logger.getStatistics();
    assert(stats.totalEntries > 0);
    
    std::cout << "✓ Parser-specific logging works" << std::endl;
}

void testScopedTimer() {
    std::cout << "Testing ScopedTimer..." << std::endl;
    
    auto& logger = ParserLogger::getInstance();
    logger.clearOutputs();
    logger.addMemoryOutput(100);
    
    {
        ParserLogger::ScopedTimer timer("test_operation", LogLevel::INFO);
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    } // Timer destructor should log completion
    
    auto stats = logger.getStatistics();
    assert(stats.totalEntries >= 2); // Start and end messages
    
    std::cout << "✓ ScopedTimer works" << std::endl;
}

void testLogLevelConversion() {
    std::cout << "Testing log level conversion..." << std::endl;
    
    auto& logger = ParserLogger::getInstance();
    
    // Test level to string
    assert(logger.levelToString(LogLevel::TRACE) == "TRACE");
    assert(logger.levelToString(LogLevel::DEBUG) == "DEBUG");
    assert(logger.levelToString(LogLevel::INFO) == "INFO");
    assert(logger.levelToString(LogLevel::WARN) == "WARN");
    assert(logger.levelToString(LogLevel::ERROR) == "ERROR");
    assert(logger.levelToString(LogLevel::FATAL) == "FATAL");
    
    // Test string to level
    assert(logger.stringToLevel("TRACE") == LogLevel::TRACE);
    assert(logger.stringToLevel("DEBUG") == LogLevel::DEBUG);
    assert(logger.stringToLevel("INFO") == LogLevel::INFO);
    assert(logger.stringToLevel("WARN") == LogLevel::WARN);
    assert(logger.stringToLevel("ERROR") == LogLevel::ERROR);
    assert(logger.stringToLevel("FATAL") == LogLevel::FATAL);
    
    std::cout << "✓ Log level conversion works" << std::endl;
}

void runAllTests() {
    std::cout << "=== ParserLogger Tests ===" << std::endl;
    
    testParserLoggerBasics();
    testParserLoggerOutputs();
    testParserSpecificLogging();
    testScopedTimer();
    testLogLevelConversion();
    
    std::cout << "\n✅ All ParserLogger tests passed!" << std::endl;
}

} // namespace Parser

int main() {
    try {
        Parser::runAllTests();
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Test failed with exception: " << e.what() << std::endl;
        return 1;
    }
}