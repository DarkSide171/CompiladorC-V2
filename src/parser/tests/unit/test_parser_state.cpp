// Unit tests for ParserState module
#include "../../include/parser_state.hpp"
#include "../../include/token_stream.hpp"
#include "../../include/error_recovery.hpp"
#include <memory>
#include <vector>
#include <iostream>
#include <cassert>
#include <stdexcept>
#include <chrono>
#include <thread>

using namespace Parser;

// Simple test framework macros
#define EXPECT_TRUE(condition) \
    do { \
        if (!(condition)) { \
            std::cerr << "FAIL: " << #condition << " at line " << __LINE__ << std::endl; \
            return false; \
        } \
    } while(0)

#define EXPECT_FALSE(condition) \
    do { \
        if (condition) { \
            std::cerr << "FAIL: !" << #condition << " at line " << __LINE__ << std::endl; \
            return false; \
        } \
    } while(0)

#define EXPECT_EQ(expected, actual) \
    do { \
        if ((expected) != (actual)) { \
            std::cerr << "FAIL: " << #expected << " != " << #actual << " at line " << __LINE__ << std::endl; \
            return false; \
        } \
    } while(0)

#define EXPECT_NE(expected, actual) \
    do { \
        if ((expected) == (actual)) { \
            std::cerr << "FAIL: " << #expected << " == " << #actual << " at line " << __LINE__ << std::endl; \
            return false; \
        } \
    } while(0)

#define EXPECT_GT(val1, val2) \
    do { \
        if (!((val1) > (val2))) { \
            std::cerr << "FAIL: " << #val1 << " <= " << #val2 << " at line " << __LINE__ << std::endl; \
            return false; \
        } \
    } while(0)

// Mock TokenStream for testing
class MockTokenStream : public TokenStream {
private:
    std::vector<Token> tokens;
    size_t position;
    
public:
    MockTokenStream() : position(0) {
        // Create some mock tokens for testing
        Lexer::Position pos1{1, 1, 0};
        Lexer::Position pos2{1, 6, 5};
        Lexer::Position pos3{1, 8, 7};
        Lexer::Position pos4{1, 10, 9};
        Lexer::Position pos5{1, 11, 10};
        
        tokens.emplace_back(Lexer::TokenType::IDENTIFIER, "test", pos1);
        tokens.emplace_back(Lexer::TokenType::ASSIGN, "=", pos2);
        tokens.emplace_back(Lexer::TokenType::INTEGER_LITERAL, "42", pos3);
        tokens.emplace_back(Lexer::TokenType::SEMICOLON, ";", pos4);
        tokens.emplace_back(Lexer::TokenType::END_OF_FILE, "", pos5);
    }
    
    const Token& current() const override {
        if (position >= tokens.size()) {
            return tokens.back(); // Return EOF token
        }
        return tokens[position];
    }
    
    const Token& peek(size_t offset = 1) const override {
        size_t peekPos = position + offset;
        if (peekPos >= tokens.size()) {
            return tokens.back(); // Return EOF token
        }
        return tokens[peekPos];
    }
    
    bool advance() override {
        if (position < tokens.size() - 1) {
            position++;
            return true;
        }
        return false;
    }
    
    bool isAtEnd() const override {
        return position >= tokens.size() - 1 || 
               tokens[position].getType() == Lexer::TokenType::END_OF_FILE;
    }
    
    size_t getPosition() const override {
        return position;
    }
    
    void setPosition(size_t pos) override {
        position = std::min(pos, tokens.size() - 1);
    }
    
    size_t size() const override {
        return tokens.size();
    }
    
    const Token& previous(size_t offset = 1) const override {
        if (position >= offset) {
            return tokens[position - offset];
        }
        return tokens[0];
    }
    
    std::vector<Token> getRange(size_t start, size_t end) const override {
        std::vector<Token> result;
        for (size_t i = start; i < std::min(end, tokens.size()); ++i) {
            result.push_back(tokens[i]);
        }
        return result;
    }
};

// Test functions
bool testConstruction() {
    ParserStateManager stateManager;
    
    EXPECT_EQ(0, stateManager.getErrorCount());
    EXPECT_EQ(0, stateManager.getWarningCount());
    
    return true;
}

bool testTokenControl() {
    ParserStateManager stateManager;
    
    // Test basic error and warning counts
    EXPECT_EQ(0, stateManager.getErrorCount());
    EXPECT_EQ(0, stateManager.getWarningCount());
    
    return true;
}

bool testContextManagement() {
    ParserStateManager stateManager;
    
    // Test pushing and popping contexts
    stateManager.pushContext("function", 0);
    const ParseContext* context = stateManager.getCurrentContext();
    EXPECT_NE(nullptr, context);
    EXPECT_EQ("function", context->contextName);
    
    stateManager.pushContext("block", 1);
    context = stateManager.getCurrentContext();
    EXPECT_NE(nullptr, context);
    EXPECT_EQ("block", context->contextName);
    
    stateManager.popContext();
    context = stateManager.getCurrentContext();
    EXPECT_NE(nullptr, context);
    EXPECT_EQ("function", context->contextName);
    
    stateManager.popContext();
    context = stateManager.getCurrentContext();
    EXPECT_EQ(nullptr, context);
    
    return true;
}

bool testScopeManagement() {
    ParserStateManager stateManager;
    
    // Test initial scope level
    size_t initialLevel = stateManager.getCurrentScopeLevel();
    
    // Test scope operations
    stateManager.enterScope("test_scope");
    EXPECT_GT(stateManager.getCurrentScopeLevel(), initialLevel);
    
    // Test symbol declaration
    Position pos{1, 1};
    stateManager.declareSymbol("test_var", ASTNodeType::VARIABLE_DECLARATION, pos);
    EXPECT_TRUE(stateManager.isSymbolDeclared("test_var"));
    
    stateManager.exitScope();
    EXPECT_EQ(initialLevel, stateManager.getCurrentScopeLevel());
    
    return true;
}

bool testPositionSaving() {
    ParserStateManager stateManager;
    
    // Test basic position tracking
    EXPECT_EQ(0, stateManager.getCurrentTokenPosition());
    
    stateManager.setCurrentTokenPosition(5);
    EXPECT_EQ(5, stateManager.getCurrentTokenPosition());
    
    return true;
}

bool testErrorTracking() {
    ParserStateManager stateManager;
    
    // Test basic error counting
    EXPECT_EQ(0, stateManager.getErrorCount());
    EXPECT_EQ(0, stateManager.getWarningCount());
    EXPECT_FALSE(stateManager.hasErrors());
    
    return true;
}

bool testStatistics() {
    ParserStateManager stateManager;
    
    stateManager.startParsing();
    stateManager.endParsing();
    
    const auto& stats = stateManager.getStatistics();
    EXPECT_EQ(0, stats.totalErrors);
    EXPECT_EQ(0, stats.totalWarnings);
    
    return true;
}

bool testTiming() {
    ParserStateManager stateManager;
    
    // Test basic timing functionality
    stateManager.startParsing();
    
    // Simulate some work
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    
    stateManager.endParsing();
    
    const auto& stats = stateManager.getStatistics();
    EXPECT_EQ(0, stats.totalErrors);
    
    return true;
}

bool testEnhancedStatistics() {
    ParserStateManager stateManager;
    
    // Test token processing tracking
    stateManager.incrementTokensProcessed();
    stateManager.incrementTokensProcessed();
    stateManager.incrementTokensProcessed();
    
    // Test warning functionality
    Position pos{1, 10};
    stateManager.addWarning("Test warning message", pos);
    stateManager.addWarning("Another warning", pos);
    
    // Test profiling
    stateManager.startTimer("parsing_expression");
    stateManager.endTimer("parsing_expression");
    
    const auto& stats = stateManager.getStatistics();
    EXPECT_EQ(3, stats.totalTokensProcessed);
    EXPECT_EQ(2, stats.totalWarnings);
    EXPECT_EQ(2, stateManager.getWarningCount());
    
    auto timingProfile = stateManager.getTimingProfile();
    EXPECT_NE(timingProfile.end(), timingProfile.find("parsing_expression"));
    
    return true;
}

bool testStatisticsReset() {
    ParserStateManager stateManager;
    
    // Add some data first
    stateManager.incrementTokensProcessed();
    Position pos{1, 10};
    stateManager.addWarning("Test warning", pos);
    stateManager.startTimer("test_timer");
    stateManager.endTimer("test_timer");
    
    // Reset statistics
    stateManager.resetStatistics();
    
    const auto& stats = stateManager.getStatistics();
    EXPECT_EQ(0, stats.totalTokensProcessed);
    EXPECT_EQ(0, stats.totalWarnings);
    EXPECT_EQ(0, stats.totalErrors);
    
    auto timingProfile = stateManager.getTimingProfile();
    EXPECT_TRUE(timingProfile.empty());
    
    return true;
}

bool testDebugAndProfiling() {
    ParserStateManager stateManager;
    
    // Test debug mode control
    EXPECT_FALSE(stateManager.isDebugModeEnabled());
    stateManager.enableDebugMode(true);
    EXPECT_TRUE(stateManager.isDebugModeEnabled());
    
    // Test debug level control
    EXPECT_EQ(ParserStateManager::DebugLevel::NONE, stateManager.getDebugLevel());
    stateManager.setDebugLevel(ParserStateManager::DebugLevel::DETAILED);
    EXPECT_EQ(ParserStateManager::DebugLevel::DETAILED, stateManager.getDebugLevel());
    
    // Test debug info generation
    std::string debugInfo = stateManager.getDebugInfo();
    EXPECT_GT(debugInfo.length(), 0);
    EXPECT_NE(std::string::npos, debugInfo.find("Parser Debug Information"));
    
    // Test detailed state info
    std::string detailedInfo = stateManager.getDetailedStateInfo();
    EXPECT_GT(detailedInfo.length(), 0);
    EXPECT_NE(std::string::npos, detailedInfo.find("Detailed Parser State"));
    
    // Test memory tracking
    size_t initialMemory = stateManager.getMemoryUsage();
    EXPECT_GT(initialMemory, 0);
    
    stateManager.trackMemoryUsage("test_operation");
    auto memoryProfile = stateManager.getMemoryProfile();
    EXPECT_NE(memoryProfile.end(), memoryProfile.find("test_operation"));
    
    // Test performance report
    stateManager.startParsing();
    stateManager.incrementTokensProcessed();
    stateManager.incrementTokensProcessed();
    stateManager.endParsing();
    
    std::string perfReport = stateManager.getPerformanceReport();
    EXPECT_GT(perfReport.length(), 0);
    EXPECT_NE(std::string::npos, perfReport.find("Performance Report"));
    EXPECT_NE(std::string::npos, perfReport.find("Tokens Processed"));
    
    // Test debug logging (should not crash)
    stateManager.logDebugInfo("Test debug message");
    
    return true;
}

bool testAdvancedProfiling() {
    ParserStateManager stateManager;
    stateManager.enableDebugMode(true);
    stateManager.setDebugLevel(ParserStateManager::DebugLevel::VERBOSE);
    
    // Test multiple memory tracking points
    stateManager.trackMemoryUsage("start");
    
    // Add some data to increase memory usage
    stateManager.enterScope("test_scope");
    Position pos{1, 1};
    stateManager.declareSymbol("var1", ASTNodeType::VARIABLE_DECLARATION, pos);
    stateManager.declareSymbol("var2", ASTNodeType::VARIABLE_DECLARATION, pos);
    
    stateManager.trackMemoryUsage("after_symbols");
    
    // Test timing multiple operations
    stateManager.startTimer("operation1");
    stateManager.endTimer("operation1");
    
    stateManager.startTimer("operation2");
    stateManager.endTimer("operation2");
    
    // Verify profiles contain expected data
    auto memoryProfile = stateManager.getMemoryProfile();
    EXPECT_NE(memoryProfile.end(), memoryProfile.find("start"));
    EXPECT_NE(memoryProfile.end(), memoryProfile.find("after_symbols"));
    
    auto timingProfile = stateManager.getTimingProfile();
    EXPECT_NE(timingProfile.end(), timingProfile.find("operation1"));
    EXPECT_NE(timingProfile.end(), timingProfile.find("operation2"));
    
    // Test that memory usage increased after adding symbols
    EXPECT_GT(memoryProfile["after_symbols"], memoryProfile["start"]);
    
    stateManager.exitScope();
    
    return true;
}

int main() {
    std::cout << "Running ParserState unit tests..." << std::endl;
    
    bool allPassed = true;
    
    std::cout << "Testing construction...";
    if (testConstruction()) {
        std::cout << " PASS" << std::endl;
    } else {
        std::cout << " FAIL" << std::endl;
        allPassed = false;
    }
    
    std::cout << "Testing token control...";
    if (testTokenControl()) {
        std::cout << " PASS" << std::endl;
    } else {
        std::cout << " FAIL" << std::endl;
        allPassed = false;
    }
    
    std::cout << "Testing context management...";
    if (testContextManagement()) {
        std::cout << " PASS" << std::endl;
    } else {
        std::cout << " FAIL" << std::endl;
        allPassed = false;
    }
    
    std::cout << "Testing scope management...";
    if (testScopeManagement()) {
        std::cout << " PASS" << std::endl;
    } else {
        std::cout << " FAIL" << std::endl;
        allPassed = false;
    }
    
    std::cout << "Testing position saving...";
    if (testPositionSaving()) {
        std::cout << " PASS" << std::endl;
    } else {
        std::cout << " FAIL" << std::endl;
        allPassed = false;
    }
    
    std::cout << "Testing error tracking...";
    if (testErrorTracking()) {
        std::cout << " PASS" << std::endl;
    } else {
        std::cout << " FAIL" << std::endl;
        allPassed = false;
    }
    
    std::cout << "Testing statistics...";
    if (testStatistics()) {
        std::cout << " PASS" << std::endl;
    } else {
        std::cout << " FAIL" << std::endl;
        allPassed = false;
    }
    
    std::cout << "Testing timing...";
    if (testTiming()) {
        std::cout << " PASS" << std::endl;
    } else {
        std::cout << " FAIL" << std::endl;
        allPassed = false;
    }
    
    std::cout << "Testing enhanced statistics...";
    if (testEnhancedStatistics()) {
        std::cout << " PASS" << std::endl;
    } else {
        std::cout << " FAIL" << std::endl;
        allPassed = false;
    }
    
    std::cout << "Testing statistics reset...";
    if (testStatisticsReset()) {
        std::cout << " PASS" << std::endl;
    } else {
        std::cout << " FAIL" << std::endl;
        allPassed = false;
    }
    
    std::cout << "Testing debug and profiling...";
    if (testDebugAndProfiling()) {
        std::cout << " PASS" << std::endl;
    } else {
        std::cout << " FAIL" << std::endl;
        allPassed = false;
    }
    
    std::cout << "Testing advanced profiling...";
    if (testAdvancedProfiling()) {
        std::cout << " PASS" << std::endl;
    } else {
        std::cout << " FAIL" << std::endl;
        allPassed = false;
    }
    
    if (allPassed) {
        std::cout << "\nAll tests PASSED!" << std::endl;
        return 0;
    } else {
        std::cout << "\nSome tests FAILED!" << std::endl;
        return 1;
    }
}