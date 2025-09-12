// Integration tests for Parser module
#include "../../include/parser_config.hpp"
#include "../../include/parser_state.hpp"
#include "../../include/grammar.hpp"
#include "../../include/error_recovery.hpp"
#include "../../include/parser_logger.hpp"
#include "../../include/ast.hpp"
#include <iostream>
#include <memory>
#include <cassert>
#include <sstream>
#include <vector>

using namespace Parser;

// Test framework macros
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

// Test integration between Grammar and ParserConfig
bool testGrammarConfigIntegration() {
    std::cout << "Testing Grammar-Config integration...";
    
    try {
        // Create config and grammar
        ParserConfig config;
        Grammar grammar;
        
        // Test basic configuration
        config.setMaxErrors(10);
        EXPECT_EQ(10, config.getMaxErrors());
        
        // Test grammar loading with basic string
        EXPECT_TRUE(grammar.loadFromString("S -> 'a' 'b'"));
        EXPECT_TRUE(grammar.validate());
        
        std::cout << " PASSED" << std::endl;
        return true;
    } catch (const std::exception& e) {
        std::cerr << " FAILED: " << e.what() << std::endl;
        return false;
    }
}

// Test integration between ParserStateManager and ErrorRecovery
bool testStateErrorRecoveryIntegration() {
    std::cout << "Testing ParserStateManager-ErrorRecovery integration...";
    
    try {
        ParserStateManager stateManager;
        ErrorRecoveryManager errorManager;
        
        // Test state management
        stateManager.addState(ParserState::IN_FUNCTION);
        EXPECT_TRUE(stateManager.hasState(ParserState::IN_FUNCTION));
        
        // Test error recovery setup
        EXPECT_TRUE(errorManager.isRecoveryEnabled());
        
        std::cout << " PASSED" << std::endl;
        return true;
    } catch (const std::exception& e) {
        std::cerr << " FAILED: " << e.what() << std::endl;
        return false;
    }
}

// Test integration between all major components
bool testFullComponentIntegration() {
    std::cout << "Testing full component integration...";
    
    try {
        // Create all components
        ParserConfig config;
        Grammar grammar;
        ParserStateManager stateManager;
        ErrorRecoveryManager errorManager;
        ParserLogger& logger = ParserLogger::getInstance();
        
        // Configure logger
        logger.setLogLevel(LogLevel::DEBUG);
        
        // Load grammar
        EXPECT_TRUE(grammar.loadFromString("S -> 'a' 'b'"));
        
        // Configure components
        config.setMaxErrors(10);
        config.setRecoveryEnabled(true);
        
        // Initialize state
        stateManager.addState(ParserState::NORMAL);
        
        // Verify integration
        EXPECT_EQ(10, config.getMaxErrors());
        EXPECT_TRUE(config.isRecoveryEnabled());
        EXPECT_EQ(ParserState::NORMAL, stateManager.getState());
        EXPECT_TRUE(errorManager.isRecoveryEnabled());
        
        std::cout << " PASSED" << std::endl;
        return true;
    } catch (const std::exception& e) {
        std::cerr << " FAILED: " << e.what() << std::endl;
        return false;
    }
}

// Test AST integration with other components
bool testASTIntegration() {
    std::cout << "Testing AST integration...";
    
    try {
        AST ast;
        
        // Create AST nodes
        auto root = ast.createNode<TranslationUnit>();
        EXPECT_TRUE(root != nullptr);
        
        // Add a declaration to make the TranslationUnit valid
        auto func = ast.createNode<FunctionDeclaration>("testFunc");
        root->addDeclaration(std::move(func));
        
        ast.setRoot(std::move(root));
        EXPECT_TRUE(ast.validate());
        
        std::cout << " PASSED" << std::endl;
        return true;
    } catch (const std::exception& e) {
        std::cerr << " FAILED: " << e.what() << std::endl;
        return false;
    }
}

// Test configuration propagation across components
bool testConfigurationPropagation() {
    std::cout << "Testing configuration propagation...";
    
    try {
        ParserConfig config;
        
        // Set various configurations
        config.setVerboseErrors(true);
        config.setMaxErrors(10);
        config.setRecoveryEnabled(true);
        
        // Verify configurations
        EXPECT_TRUE(config.hasFlag(ParserFlags::VERBOSE_ERRORS));
        EXPECT_EQ(10, config.getMaxErrors());
        EXPECT_TRUE(config.isRecoveryEnabled());
        
        // Test with error recovery manager
        ErrorRecoveryManager errorManager;
        errorManager.setRecoveryEnabled(config.isRecoveryEnabled());
        EXPECT_TRUE(errorManager.isRecoveryEnabled());
        
        std::cout << " PASSED" << std::endl;
        return true;
    } catch (const std::exception& e) {
        std::cerr << " FAILED: " << e.what() << std::endl;
        return false;
    }
}

int main() {
    std::cout << "=== Parser Integration Tests ===" << std::endl;
    
    int passed = 0;
    int total = 0;
    
    // Run integration tests
    total++; if (testGrammarConfigIntegration()) passed++;
    total++; if (testStateErrorRecoveryIntegration()) passed++;
    total++; if (testFullComponentIntegration()) passed++;
    total++; if (testASTIntegration()) passed++;
    total++; if (testConfigurationPropagation()) passed++;
    
    std::cout << "\n=== Integration Test Results ===" << std::endl;
    std::cout << "Passed: " << passed << "/" << total << std::endl;
    
    if (passed == total) {
        std::cout << "All integration tests PASSED!" << std::endl;
        return 0;
    } else {
        std::cout << "Some integration tests FAILED!" << std::endl;
        return 1;
    }
}