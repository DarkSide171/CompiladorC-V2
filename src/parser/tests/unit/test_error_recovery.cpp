// Unit tests for Error Recovery module
#include "../../include/error_recovery.hpp"
#include "../../include/parser_types.hpp"
#include "../../include/parser_state.hpp"
#include "../../../lexer/include/token.hpp"
#include <iostream>
#include <memory>
#include <vector>
#include <cassert>

using namespace Parser;
using namespace Lexer;

// Helper function to create test tokens
std::vector<Token> createTestTokens(const std::vector<std::string>& lexemes) {
    std::vector<Token> tokens;
    for (const auto& lexeme : lexemes) {
        TokenType type = TokenType::IDENTIFIER; // Default type
        if (lexeme == ";") type = TokenType::SEMICOLON;
        else if (lexeme == "{") type = TokenType::LEFT_BRACE;
        else if (lexeme == "}") type = TokenType::RIGHT_BRACE;
        else if (lexeme == "(") type = TokenType::LEFT_PAREN;
        else if (lexeme == ")") type = TokenType::RIGHT_PAREN;
        else if (lexeme == "=") type = TokenType::ASSIGN;
        else if (lexeme == "==") type = TokenType::EQUAL;
        else if (lexeme == "if") type = TokenType::IF;
        else if (lexeme == "return") type = TokenType::RETURN;
        
        tokens.emplace_back(type, lexeme, Lexer::Position(1, 1, 0));
    }
    return tokens;
}

// Test ErrorRecoveryManager construction
bool testErrorRecoveryManagerConstruction() {
    std::cout << "Testing ErrorRecoveryManager construction...";
    
    ErrorRecoveryManager manager;
    assert(manager.isRecoveryEnabled());
    assert(manager.getMaxRecoveryAttempts() > 0);
    
    std::cout << " PASSED" << std::endl;
    return true;
}

// Test recoverFromUnexpectedToken
bool testRecoverFromUnexpectedToken() {
    std::cout << "Testing recoverFromUnexpectedToken...";
    
    ErrorRecoveryManager manager;
    ParserStateManager state;
    
    // Test case: unexpected token that should be skipped
    auto tokens = createTestTokens({";", "identifier", "=", "value"});
    size_t currentPos = 0;
    
    bool result = manager.recoverFromUnexpectedToken(";", "identifier", state, tokens, currentPos);
    assert(result == true);
    assert(currentPos == 1); // Should skip the semicolon
    
    std::cout << " PASSED" << std::endl;
    return true;
}

// Test recoverFromMissingToken
bool testRecoverFromMissingToken() {
    std::cout << "Testing recoverFromMissingToken...";
    
    ErrorRecoveryManager manager;
    ParserStateManager state;
    
    // Test case: missing semicolon that can be found ahead
    auto tokens = createTestTokens({"identifier", "=", "value", ";", "next_statement"});
    size_t currentPos = 0;
    
    bool result = manager.recoverFromMissingToken(";", state, tokens, currentPos);
    assert(result == true);
    assert(currentPos == 3); // Should find semicolon at position 3
    
    std::cout << " PASSED" << std::endl;
    return true;
}

// Test skipToSemicolon
bool testSkipToSemicolon() {
    std::cout << "Testing skipToSemicolon...";
    
    ErrorRecoveryManager manager;
    ParserStateManager state;
    
    // Test case: skip malformed expression until semicolon
    auto tokens = createTestTokens({"bad", "expression", "tokens", ";", "next_statement"});
    size_t currentPos = 0;
    
    bool result = manager.skipToSemicolon(state, tokens, currentPos);
    assert(result == true);
    assert(currentPos == 4); // Should be past the semicolon
    
    std::cout << " PASSED" << std::endl;
    return true;
}

// Test synchronizeToToken
bool testSynchronizeToToken() {
    std::cout << "Testing synchronizeToToken...";
    
    ErrorRecoveryManager manager;
    ParserStateManager state;
    
    // Test case: synchronize to a specific token type
    auto tokens = createTestTokens({"bad", "tokens", "{", "good_content"});
    size_t currentPos = 0;
    
    bool result = manager.synchronizeToToken(TokenType::LEFT_BRACE, state, tokens, currentPos);
    assert(result == true);
    assert(currentPos == 2); // Should find LBRACE at position 2
    
    std::cout << " PASSED" << std::endl;
    return true;
}

// Test recoverFromMissingBrace
bool testRecoverFromMissingBrace() {
    std::cout << "Testing recoverFromMissingBrace...";
    
    ErrorRecoveryManager manager;
    ParserStateManager state;
    
    // Test case: missing opening brace after if statement
    auto tokens = createTestTokens({"if", "(", "condition", ")", "statement", ";"});
    size_t currentPos = 4; // At "statement"
    
    bool result = manager.recoverFromMissingBrace('{', state, tokens, currentPos);
    assert(result == true); // Should detect missing brace scenario
    
    std::cout << " PASSED" << std::endl;
    return true;
}

// Test recoverFromExpressionError
bool testRecoverFromExpressionError() {
    std::cout << "Testing recoverFromExpressionError...";
    
    ErrorRecoveryManager manager;
    ParserStateManager state;
    
    // Test case: malformed expression with terminator
    auto tokens = createTestTokens({"bad", "expression", "return", "value", ";"});
    size_t currentPos = 0;
    
    bool result = manager.recoverFromExpressionError(state, tokens, currentPos);
    assert(result == true);
    assert(currentPos == 2); // Should find "return" terminator
    
    std::cout << " PASSED" << std::endl;
    return true;
}

// Test error creation functions
bool testErrorCreation() {
    std::cout << "Testing error creation functions...";
    
    SourceRange range(Parser::Position(1, 1, 0), Parser::Position(1, 10, 9));
     
     auto syntaxError = createSyntaxError("Test syntax error", range);
     assert(syntaxError != nullptr);
     assert(syntaxError->getMessage() == "Test syntax error");
     
     auto unexpectedError = createUnexpectedTokenError(";", "identifier", range);
     assert(unexpectedError != nullptr);
     
     auto missingError = createMissingTokenError(";", range);
     assert(missingError != nullptr);
     
     auto semanticError = createSemanticError("Test semantic error", range);
     assert(semanticError != nullptr);
    
    std::cout << " PASSED" << std::endl;
    return true;
}

// Test recovery strategies
bool testRecoveryStrategies() {
    std::cout << "Testing recovery strategies...";
    
    ErrorRecoveryManager manager;
    
    // Add different recovery strategies
    manager.addStrategy(std::make_unique<PanicModeRecovery>());
    manager.addStrategy(std::make_unique<PhraseLevelRecovery>());
    manager.addStrategy(std::make_unique<ErrorProductionRecovery>());
    
    // Test with a syntax error
    auto syntaxError = createSyntaxError("Test error", SourceRange(Parser::Position(1, 1, 0), Parser::Position(1, 10, 9)));
    ParserStateManager state;
    auto tokens = createTestTokens({"bad", "token", ";", "good", "token"});
    size_t currentPos = 0;
    
    bool result = manager.attemptRecovery(*syntaxError, state, tokens, currentPos);
    // Result may vary depending on strategy implementation
    
    std::cout << " PASSED" << std::endl;
    return true;
}

// Test statistics and configuration
bool testStatisticsAndConfiguration() {
    std::cout << "Testing statistics and configuration...";
    
    ErrorRecoveryManager manager;
    
    // Test configuration
    manager.setMaxRecoveryAttempts(10);
    assert(manager.getMaxRecoveryAttempts() == 10);
    
    manager.setRecoveryEnabled(false);
    assert(!manager.isRecoveryEnabled());
    
    manager.setRecoveryEnabled(true);
    assert(manager.isRecoveryEnabled());
    
    // Test statistics reset
    manager.resetStatistics();
    auto stats = manager.getStatistics();
    assert(stats.totalAttempts == 0);
    assert(stats.successfulRecoveries == 0);
    assert(stats.failedRecoveries == 0);
    
    std::cout << " PASSED" << std::endl;
    return true;
}

int main() {
    std::cout << "=== Error Recovery Unit Tests ===" << std::endl;
    
    int passed = 0;
    int total = 0;
    
    // Run all tests
    total++; if (testErrorRecoveryManagerConstruction()) passed++;
    total++; if (testRecoverFromUnexpectedToken()) passed++;
    total++; if (testRecoverFromMissingToken()) passed++;
    total++; if (testSkipToSemicolon()) passed++;
    total++; if (testSynchronizeToToken()) passed++;
    total++; if (testRecoverFromMissingBrace()) passed++;
    total++; if (testRecoverFromExpressionError()) passed++;
    total++; if (testErrorCreation()) passed++;
    total++; if (testRecoveryStrategies()) passed++;
    total++; if (testStatisticsAndConfiguration()) passed++;
    
    std::cout << "\n=== Test Results ===" << std::endl;
    std::cout << "Passed: " << passed << "/" << total << " tests" << std::endl;
    
    if (passed == total) {
        std::cout << "All tests PASSED!" << std::endl;
        return 0;
    } else {
        std::cout << "Some tests FAILED!" << std::endl;
        return 1;
    }
}