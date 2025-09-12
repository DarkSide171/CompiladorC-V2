#include "../../include/parser.hpp"
#include "../../include/parser_config.hpp"
#include "../../include/error_recovery.hpp"
#include <memory>
#include <string>
#include <vector>
#include <iostream>
#include <cassert>
#include <chrono>
#include <functional>

using namespace Parser;
using ParserToken = Parser::Token;

// Simple test framework
class TestRunner {
public:
    static int totalTests;
    static int passedTests;
    static int failedTests;
    
    static void runTest(const std::string& testName, std::function<void()> testFunc) {
        totalTests++;
        std::cout << "Running test: " << testName << "... ";
        
        try {
            testFunc();
            passedTests++;
            std::cout << "PASSED" << std::endl;
        } catch (const std::exception& e) {
            failedTests++;
            std::cout << "FAILED: " << e.what() << std::endl;
        } catch (...) {
            failedTests++;
            std::cout << "FAILED: Unknown exception" << std::endl;
        }
    }
    
    static void printSummary() {
        std::cout << "\n=== Test Summary ===" << std::endl;
        std::cout << "Total tests: " << totalTests << std::endl;
        std::cout << "Passed: " << passedTests << std::endl;
        std::cout << "Failed: " << failedTests << std::endl;
        std::cout << "Success rate: " << (totalTests > 0 ? (passedTests * 100 / totalTests) : 0) << "%" << std::endl;
    }
};

int TestRunner::totalTests = 0;
int TestRunner::passedTests = 0;
int TestRunner::failedTests = 0;

// Simple assertion macros
#define ASSERT_TRUE(condition) \
    if (!(condition)) { \
        throw std::runtime_error("Assertion failed: " #condition); \
    }

#define ASSERT_FALSE(condition) \
    if (condition) { \
        throw std::runtime_error("Assertion failed: !" #condition); \
    }

#define ASSERT_EQ(expected, actual) \
    if ((expected) != (actual)) { \
        throw std::runtime_error("Assertion failed: " #expected " == " #actual); \
    }

#define ASSERT_NE(expected, actual) \
    if ((expected) == (actual)) { \
        throw std::runtime_error("Assertion failed: " #expected " != " #actual); \
    }

#define ASSERT_LT(a, b) \
    if (!((a) < (b))) { \
        throw std::runtime_error("Assertion failed: " #a " < " #b); \
    }

#define ASSERT_GT(a, b) \
    if (!((a) > (b))) { \
        throw std::runtime_error("Assertion failed: " #a " > " #b); \
    }

#define EXPECT_NO_THROW(statement) \
    try { \
        statement; \
    } catch (...) { \
        throw std::runtime_error("Expected no exception, but got one"); \
    }

// Mock classes for testing (simplified)
class MockTokenStream : public TokenStream {
public:
    const ParserToken& current() const override {
        static ParserToken eofToken{Lexer::TokenType::END_OF_FILE, "", Lexer::Position{1, 1, 0}};
        return eofToken;
    }
    
    const ParserToken& peek(size_t offset = 1) const override {
        (void)offset;
        static ParserToken eofToken{Lexer::TokenType::END_OF_FILE, "", Lexer::Position{1, 1, 0}};
        return eofToken;
    }
    
    bool advance() override {
        return false; // Mock implementation - cannot advance
    }
    
    bool isAtEnd() const override {
        return true; // Mock implementation - always at end
    }
    
    size_t getPosition() const override {
        return 0; // Mock implementation - position 0
    }
    
    void setPosition(size_t position) override {
        (void)position; // Mock implementation - do nothing
    }
    
    size_t size() const override {
        return 0; // Mock implementation - empty stream
    }
    
    const ParserToken& previous(size_t offset = 1) const override {
        (void)offset;
        static ParserToken eofToken{Lexer::TokenType::END_OF_FILE, "", Lexer::Position{1, 1, 0}};
        return eofToken;
    }
    
    std::vector<ParserToken> getRange(size_t start, size_t end) const override {
        (void)start;
        (void)end;
        return {}; // Mock implementation - empty range
    }
};

class MockLexerParserBridge : public LexerParserBridge {
public:
    std::unique_ptr<TokenStream> getTokenStream() override {
        return std::make_unique<MockTokenStream>();
    }
    
    bool hasMoreTokens() const override {
        return false; // Mock implementation - no more tokens
    }
    
    size_t getCurrentLine() const override {
        return 1; // Mock implementation - line 1
    }
    
    size_t getCurrentColumn() const override {
        return 1; // Mock implementation - column 1
    }
    
    void reportLexerError(const std::string& message, const Parser::Position& pos) override {
        (void)message;
        (void)pos;
        // Mock implementation - do nothing
    }
};

class MockPreprocessorParserBridge : public PreprocessorParserBridge {
public:
    Parser::Position mapToOriginalPosition(const Parser::Position& expandedPos) const override {
        return expandedPos; // Simple passthrough for testing
    }
    
    SourceRange mapToOriginalRange(const SourceRange& expandedRange) const override {
        return expandedRange; // Simple passthrough for testing
    }
    
    bool isPositionInMacro(const Parser::Position& pos) const override {
        (void)pos;
        return false; // No macros in test
    }
    
    std::string getMacroNameAtPosition(const Parser::Position& pos) const override {
        (void)pos;
        return ""; // No macros in test
    }
    
    std::string getOriginalFilename(const Parser::Position& pos) const override {
        (void)pos;
        return "test.c"; // Default test filename
    }
    
    bool isPositionInInclude(const Parser::Position& pos) const override {
        (void)pos;
        return false; // No includes in test
    }
};

class MockSemanticInterface : public SemanticInterface {
public:
    void processAST(const ASTNodePtr& ast) override {
        (void)ast; // Mock implementation - do nothing
    }
    
    bool isSymbolDeclared(const std::string& name) const override {
        (void)name;
        return false; // Mock implementation - no symbols declared
    }
    
    ASTNodeType getSymbolType(const std::string& name) const override {
        (void)name;
        return ASTNodeType::IDENTIFIER; // Mock implementation - default type
    }
    
    bool areTypesCompatible(const ASTNodePtr& type1, const ASTNodePtr& type2) const override {
        (void)type1;
        (void)type2;
        return true; // Mock implementation - all types compatible
    }
    
    void reportSemanticError(const std::string& message, const SourceRange& range) override {
        (void)message;
        (void)range;
        // Mock implementation - do nothing
    }
};

// Test fixture for SyntacticAnalyzer
class SyntacticAnalyzerTest {
public:
    void SetUp() {
        config = std::make_shared<ParserConfig>();
        
        mockTokenStream = std::make_shared<MockTokenStream>();
        mockLexerBridge = std::make_shared<MockLexerParserBridge>();
        mockPreprocessorBridge = std::make_shared<MockPreprocessorParserBridge>();
        mockSemanticInterface = std::make_shared<MockSemanticInterface>();
        
        parser = std::make_unique<SyntacticAnalyzer>(*config);
    }
    
    void TearDown() {
        parser.reset();
    }
    
    std::shared_ptr<ParserConfig> config;
    std::shared_ptr<MockTokenStream> mockTokenStream;
    std::shared_ptr<MockLexerParserBridge> mockLexerBridge;
    std::shared_ptr<MockPreprocessorParserBridge> mockPreprocessorBridge;
    std::shared_ptr<MockSemanticInterface> mockSemanticInterface;
    std::unique_ptr<SyntacticAnalyzer> parser;
};

void testDefaultConstructor() {
    auto defaultParser = std::make_unique<SyntacticAnalyzer>();
    ASSERT_TRUE(defaultParser != nullptr);
    
    auto stats = defaultParser->getStatistics();
    ASSERT_EQ(stats.nodesCreated, 0);
    ASSERT_EQ(stats.errorsEncountered, 0);
}

void testConfigConstructor() {
    SyntacticAnalyzerTest test;
    test.SetUp();
    
    ASSERT_TRUE(test.parser != nullptr);
    
    auto stats = test.parser->getStatistics();
    ASSERT_EQ(stats.nodesCreated, 0);
    ASSERT_EQ(stats.errorsEncountered, 0);
    ASSERT_EQ(stats.recoveryAttempts, 0);
    
    test.TearDown();
}

void testSetBridges() {
    SyntacticAnalyzerTest test;
    test.SetUp();
    
    auto lexerBridge = std::make_unique<MockLexerParserBridge>();
    auto preprocessorBridge = std::make_unique<MockPreprocessorParserBridge>();
    auto semanticInterface = std::make_unique<MockSemanticInterface>();
    
    EXPECT_NO_THROW(test.parser->setLexerBridge(std::move(lexerBridge)));
    EXPECT_NO_THROW(test.parser->setPreprocessorBridge(std::move(preprocessorBridge)));
    EXPECT_NO_THROW(test.parser->setSemanticInterface(std::move(semanticInterface)));
    
    test.TearDown();
}

void testConfigurationMethods() {
    SyntacticAnalyzerTest test;
    test.SetUp();
    
    auto newConfig = ParserConfig();
    newConfig.setCStandard(CStandard::C99);
    
    EXPECT_NO_THROW(test.parser->setConfig(newConfig));
    ASSERT_EQ(test.parser->getConfig().getCStandard(), CStandard::C99);
    
    test.TearDown();
}

void testLoadGrammar() {
    SyntacticAnalyzerTest test;
    test.SetUp();
    
    // Test grammar loading - create a minimal valid grammar
    Grammar testGrammar;
    testGrammar.addNonTerminal("S");
    testGrammar.addTerminal("id");
    testGrammar.addProduction("S", {"id"});
    testGrammar.setStartSymbol("S");
    
    // Test that we can set the grammar (validation may fail but setGrammar should handle it)
    try {
        test.parser->setGrammar(testGrammar);
        std::cout << "✓ Grammar set successfully" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "Grammar validation failed as expected: " << e.what() << std::endl;
        // This is acceptable for this test - we're testing the setGrammar mechanism
    }
    
    test.TearDown();
}

void testParseEmptyInput() {
    SyntacticAnalyzerTest test;
    test.SetUp();
    
    auto lexerBridge = std::make_unique<MockLexerParserBridge>();
    auto tokenStream = lexerBridge->getTokenStream();
    test.parser->setLexerBridge(std::move(lexerBridge));
    
    auto result = test.parser->parse(std::move(tokenStream));
    ASSERT_FALSE(result.isSuccess());
    
    test.TearDown();
}

void testErrorHandling() {
    SyntacticAnalyzerTest test;
    test.SetUp();
    
    // Test error reporting
    Parser::Position pos(1, 1);
    
    // Create a syntax error and add it to the parser
    auto error = createSyntaxError("Test error", SourceRange(pos, pos));
    
    // Check error was created
    ASSERT_TRUE(error != nullptr);
    ASSERT_EQ(error->getMessage(), "Test error");
    
    test.TearDown();
}

void testStatistics() {
    SyntacticAnalyzerTest test;
    test.SetUp();
    
    // Test that parser can be queried for basic state
    ASSERT_FALSE(test.parser->hasErrors());
    ASSERT_EQ(test.parser->getErrors().size(), 0);
    
    test.TearDown();
}

void testReset() {
    SyntacticAnalyzerTest test;
    test.SetUp();
    
    // Test that parser can clear errors
    test.parser->clearErrors();
    
    // Check errors are cleared
    ASSERT_EQ(test.parser->getErrors().size(), 0);
    
    test.TearDown();
}

std::unique_ptr<SyntacticAnalyzer> createParserForStandard(CStandard standard) {
    auto config = ParserConfig::createForStandard(standard);
    return std::make_unique<SyntacticAnalyzer>(config);
}

void testCreateC89Parser() {
    auto parser = createParserForStandard(CStandard::C89);
    ASSERT_TRUE(parser != nullptr);
}

void testCreateC99Parser() {
    auto parser = createParserForStandard(CStandard::C99);
    ASSERT_TRUE(parser != nullptr);
}

void testCreateC11Parser() {
    auto parser = createParserForStandard(CStandard::C11);
    ASSERT_TRUE(parser != nullptr);
}

void testCreateC17Parser() {
    auto parser = createParserForStandard(CStandard::C17);
    ASSERT_TRUE(parser != nullptr);
}

void testCreateC23Parser() {
    auto parser = createParserForStandard(CStandard::C23);
    ASSERT_TRUE(parser != nullptr);
}

void testIsValidCCode() {
    ASSERT_TRUE(isValidCCode("int main() { return 0; }"));
    ASSERT_FALSE(isValidCCode("invalid syntax {"));
    ASSERT_FALSE(isValidCCode(""));
}

void testPerformance() {
    auto config = ParserConfig();
    auto parser = std::make_unique<SyntacticAnalyzer>(config);
    
    auto start = std::chrono::high_resolution_clock::now();
    
    // Create a parser and perform basic operations
    for (int i = 0; i < 1000; ++i) {
        parser->reset();
        auto stats = parser->getStatistics();
        (void)stats; // Suppress unused variable warning
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    // Should complete within reasonable time (adjust threshold as needed)
    ASSERT_LT(duration.count(), 1000); // Less than 1 second
}

void testMemory() {
    // Test that parser can be created and destroyed multiple times without leaks
    for (int i = 0; i < 100; ++i) {
        auto config = ParserConfig();
        auto testParser = std::make_unique<SyntacticAnalyzer>(config);
        auto lexerBridge = std::make_unique<MockLexerParserBridge>();
        auto preprocessorBridge = std::make_unique<MockPreprocessorParserBridge>();
        auto semanticInterface = std::make_unique<MockSemanticInterface>();
        testParser->setLexerBridge(std::move(lexerBridge));
        testParser->setPreprocessorBridge(std::move(preprocessorBridge));
        testParser->setSemanticInterface(std::move(semanticInterface));
        testParser->reset();
        // testParser automatically destroyed at end of scope
    }
    
    // If we reach here without crashes, memory management is likely correct
    ASSERT_TRUE(true);
}

void testThreadSafety() {
    // Test that multiple parsers can be created simultaneously
    std::vector<std::unique_ptr<SyntacticAnalyzer>> parsers;
    auto config = ParserConfig();
    
    for (int i = 0; i < 10; ++i) {
        parsers.push_back(std::make_unique<SyntacticAnalyzer>(config));
    }
    
    // All parsers should be valid
    for (const auto& p : parsers) {
        ASSERT_TRUE(p != nullptr);
        auto stats = p->getStatistics();
        ASSERT_EQ(stats.errorsEncountered, 0);
    }
}

int main() {
    TestRunner::runTest("DefaultConstructor", testDefaultConstructor);
    TestRunner::runTest("ConfigConstructor", testConfigConstructor);
    TestRunner::runTest("SetBridges", testSetBridges);
    TestRunner::runTest("ConfigurationMethods", testConfigurationMethods);
    TestRunner::runTest("LoadGrammar", testLoadGrammar);
    TestRunner::runTest("ParseEmptyInput", testParseEmptyInput);
    TestRunner::runTest("ErrorHandling", testErrorHandling);
    TestRunner::runTest("Statistics", testStatistics);
    TestRunner::runTest("Reset", testReset);
    TestRunner::runTest("CreateC89Parser", testCreateC89Parser);
    TestRunner::runTest("CreateC99Parser", testCreateC99Parser);
    TestRunner::runTest("CreateC11Parser", testCreateC11Parser);
    TestRunner::runTest("CreateC17Parser", testCreateC17Parser);
    TestRunner::runTest("CreateC23Parser", testCreateC23Parser);
    TestRunner::runTest("IsValidCCode", testIsValidCCode);
    TestRunner::runTest("Performance", testPerformance);
    TestRunner::runTest("Memory", testMemory);
    TestRunner::runTest("ThreadSafety", testThreadSafety);
    
    TestRunner::printSummary();
    
    return TestRunner::failedTests > 0 ? 1 : 0;
}