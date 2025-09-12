// Unit tests for ParserState module
#include "../include/parser_state.hpp"
#include "../include/token_stream.hpp"
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
    auto tokenStream = std::make_unique<MockTokenStream>();
    stateManager.setTokenStream(tokenStream.get());
    
    // Test getCurrentToken
    const Token& current = stateManager.getCurrentToken();
    EXPECT_EQ(Lexer::TokenType::IDENTIFIER, current.getType());
    EXPECT_EQ("test", current.getLexeme());
    
    // Test peekToken
    const Token& next = stateManager.peekToken();
    EXPECT_EQ(Lexer::TokenType::ASSIGN, next.getType());
    
    // Test consumeToken
    bool consumed = stateManager.consumeToken();
    EXPECT_TRUE(consumed);
    
    // After consuming, current should be the next token
    const Token& afterConsume = stateManager.getCurrentToken();
    EXPECT_EQ(Lexer::TokenType::ASSIGN, afterConsume.getType());
    
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
    
    // Test scope operations
    stateManager.enterScope("test_scope");
    EXPECT_GT(stateManager.getCurrentScopeLevel(), 0);
    
    // Test symbol declaration
    Position pos{1, 1};
    stateManager.declareSymbol("test_var", ASTNodeType::VARIABLE_DECLARATION, pos);
    EXPECT_TRUE(stateManager.isSymbolDeclared("test_var"));
    
    stateManager.exitScope();
    EXPECT_EQ(0, stateManager.getCurrentScopeLevel());
    
    return true;
}

bool testPositionSaving() {
    ParserStateManager stateManager;
    auto tokenStream = std::make_unique<MockTokenStream>();
    stateManager.setTokenStream(tokenStream.get());
    
    // Save current position
    stateManager.savePosition("checkpoint1");
    
    // Advance and then restore
    stateManager.consumeToken();
    stateManager.consumeToken();
    
    EXPECT_TRUE(stateManager.restorePosition("checkpoint1"));
    
    return true;
}

bool testErrorTracking() {
    ParserStateManager stateManager;
    
    EXPECT_EQ(0, stateManager.getErrorCount());
    
    Position pos{1, 5};
    stateManager.addWarning("Test warning", pos);
    EXPECT_EQ(1, stateManager.getWarningCount());
    
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
    
    stateManager.startTimer("test_operation");
    
    // Simulate some work
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    
    stateManager.endTimer("test_operation");
    
    auto profile = stateManager.getTimingProfile();
    EXPECT_GT(profile.size(), 0);
    
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
    
    if (allPassed) {
        std::cout << "\nAll tests PASSED!" << std::endl;
        return 0;
    } else {
        std::cout << "\nSome tests FAILED!" << std::endl;
        return 1;
    }
}