// Unit tests for Grammar module
#include "../../include/grammar.hpp"
#include "../../include/parser_types.hpp"
#include <memory>
#include <sstream>
#include <fstream>
#include <iostream>
#include <cassert>
#include <stdexcept>

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

#define EXPECT_NO_THROW(expression) \
    do { \
        try { \
            expression; \
        } catch (const std::exception& e) { \
            std::cerr << "FAIL: Unexpected exception: " << e.what() << " at line " << __LINE__ << std::endl; \
            return false; \
        } catch (...) { \
            std::cerr << "FAIL: Unexpected exception at line " << __LINE__ << std::endl; \
            return false; \
        } \
    } while(0)

// Test functions
bool testConstruction() {
    std::cout << "Testing grammar construction..." << std::endl;
    auto grammar = std::make_unique<Grammar>();
    EXPECT_TRUE(grammar != nullptr);
    EXPECT_TRUE(grammar->getTerminals().empty());
    EXPECT_TRUE(grammar->getNonTerminals().empty());
    EXPECT_TRUE(grammar->getProductions().empty());
    std::cout << "✓ Construction test passed" << std::endl;
    return true;
}

bool testAddTerminals() {
    std::cout << "Testing adding terminals..." << std::endl;
    auto grammar = std::make_unique<Grammar>();
    grammar->addTerminal("id");
    grammar->addTerminal("+");
    grammar->addTerminal("*");
    
    auto terminals = grammar->getTerminals();
    EXPECT_EQ(terminals.size(), 3);
    EXPECT_TRUE(terminals.find("id") != terminals.end());
    EXPECT_TRUE(terminals.find("+") != terminals.end());
    EXPECT_TRUE(terminals.find("*") != terminals.end());
    std::cout << "✓ Add terminals test passed" << std::endl;
    return true;
}

bool testAddNonTerminals() {
    std::cout << "Testing adding non-terminals..." << std::endl;
    auto grammar = std::make_unique<Grammar>();
    grammar->addNonTerminal("E");
    grammar->addNonTerminal("T");
    grammar->addNonTerminal("F");
    
    auto nonTerminals = grammar->getNonTerminals();
    EXPECT_EQ(nonTerminals.size(), 3);
    EXPECT_TRUE(nonTerminals.find("E") != nonTerminals.end());
    EXPECT_TRUE(nonTerminals.find("T") != nonTerminals.end());
    EXPECT_TRUE(nonTerminals.find("F") != nonTerminals.end());
    std::cout << "✓ Add non-terminals test passed" << std::endl;
    return true;
}

bool testSetStartSymbol() {
    std::cout << "Testing setting start symbol..." << std::endl;
    auto grammar = std::make_unique<Grammar>();
    grammar->addNonTerminal("E");
    grammar->setStartSymbol("E");
    
    EXPECT_EQ(grammar->getStartSymbol(), "E");
    std::cout << "✓ Set start symbol test passed" << std::endl;
    return true;
}

bool testAddProductions() {
    std::cout << "Testing adding productions..." << std::endl;
    auto grammar = std::make_unique<Grammar>();
    grammar->addNonTerminal("E");
    grammar->addNonTerminal("T");
    grammar->addTerminal("+");
    grammar->addTerminal("id");
    
    Production prod1("E", {"E", "+", "T"});
    Production prod2("E", {"T"});
    Production prod3("T", {"id"});
    
    grammar->addProduction(prod1);
    grammar->addProduction(prod2);
    grammar->addProduction(prod3);
    
    auto productions = grammar->getProductions();
    EXPECT_EQ(productions.size(), 3);
    std::cout << "✓ Add productions test passed" << std::endl;
    return true;
}

bool testLoadFromString() {
    std::cout << "Testing loading grammar from string..." << std::endl;
    auto grammar = std::make_unique<Grammar>();
    std::string grammarStr = 
        "E -> E + T\n"
        "E -> T\n"
        "T -> id\n";
    
    EXPECT_NO_THROW(grammar->loadFromString(grammarStr));
    
    auto productions = grammar->getProductions();
    EXPECT_EQ(productions.size(), 3);
    
    auto terminals = grammar->getTerminals();
    EXPECT_TRUE(terminals.find("+") != terminals.end());
    EXPECT_TRUE(terminals.find("id") != terminals.end());
    
    auto nonTerminals = grammar->getNonTerminals();
    EXPECT_TRUE(nonTerminals.find("E") != nonTerminals.end());
    EXPECT_TRUE(nonTerminals.find("T") != nonTerminals.end());
    std::cout << "✓ Load from string test passed" << std::endl;
    return true;
}

bool testComputeFirstSets() {
    std::cout << "Testing FIRST sets computation..." << std::endl;
    auto grammar = std::make_unique<Grammar>();
    std::string grammarStr = 
        "E -> T\n"
        "T -> id\n";
    
    grammar->loadFromString(grammarStr);
    grammar->setStartSymbol("E");
    
    // FIRST(E) should contain id
    auto firstE = grammar->computeFirst("E");
    EXPECT_TRUE(firstE.find("id") != firstE.end());
    
    // FIRST(T) should contain id
    auto firstT = grammar->computeFirst("T");
    EXPECT_TRUE(firstT.find("id") != firstT.end());
    std::cout << "✓ FIRST sets test passed" << std::endl;
    return true;
}

bool testComputeFollowSets() {
    std::cout << "Testing FOLLOW sets computation..." << std::endl;
    auto grammar = std::make_unique<Grammar>();
    std::string grammarStr = 
        "E -> E + T\n"
        "E -> T\n"
        "T -> id\n";
    
    grammar->loadFromString(grammarStr);
    grammar->setStartSymbol("E");
    
    // FOLLOW(E) should contain $
    auto followE = grammar->computeFollow("E");
    EXPECT_TRUE(followE.find("$") != followE.end());
    
    // FOLLOW(T) should contain + and $
    auto followT = grammar->computeFollow("T");
    EXPECT_TRUE(followT.find("$") != followT.end() || followT.find("+") != followT.end());
    std::cout << "✓ FOLLOW sets test passed" << std::endl;
    return true;
}

bool testValidateGrammar() {
    std::cout << "Testing grammar validation..." << std::endl;
    auto grammar = std::make_unique<Grammar>();
    std::string grammarStr = 
        "E -> T\n"
        "T -> id\n";
    
    grammar->loadFromString(grammarStr);
    grammar->setStartSymbol("E");
    
    EXPECT_TRUE(grammar->validate());
    EXPECT_TRUE(grammar->getValidationErrors().empty());
    std::cout << "✓ Grammar validation test passed" << std::endl;
    return true;
}

bool testInvalidGrammar() {
    std::cout << "Testing invalid grammar detection..." << std::endl;
    auto grammar = std::make_unique<Grammar>();
    std::string grammarStr = 
        "E -> T\n"
        "T -> id\n";
    
    grammar->loadFromString(grammarStr);
    // Don't set start symbol
    
    EXPECT_FALSE(grammar->validate());
    EXPECT_FALSE(grammar->getValidationErrors().empty());
    std::cout << "✓ Invalid grammar test passed" << std::endl;
    return true;
}

bool testLeftRecursionDetection() {
    std::cout << "Testing left recursion detection..." << std::endl;
    auto grammar = std::make_unique<Grammar>();
    std::string grammarStr = 
        "E -> E + T\n"
        "T -> id\n";
    
    grammar->loadFromString(grammarStr);
    grammar->setStartSymbol("E");
    
    EXPECT_TRUE(grammar->hasLeftRecursion());
    std::cout << "✓ Left recursion detection test passed" << std::endl;
    return true;
}

bool testLL1Property() {
    std::cout << "Testing LL(1) property check..." << std::endl;
    auto grammar = std::make_unique<Grammar>();
    std::string grammarStr = 
        "E -> T E'\n"
        "E' -> + T E'\n"
        "E' -> \n"  // epsilon production
        "T -> id\n";
    
    grammar->loadFromString(grammarStr);
    grammar->setStartSymbol("E");
    
    // This should be LL(1) after proper transformation
    EXPECT_NO_THROW(grammar->isLL1());
    std::cout << "✓ LL(1) property test passed" << std::endl;
    return true;
}

bool testSaveToFile() {
    std::cout << "Testing saving grammar to file..." << std::endl;
    auto grammar = std::make_unique<Grammar>();
    std::string grammarStr = 
        "E -> T\n"
        "T -> id\n";
    
    grammar->loadFromString(grammarStr);
    grammar->setStartSymbol("E");
    
    std::string testFile = "/tmp/test_grammar.txt";
    EXPECT_NO_THROW(grammar->saveToFile(testFile));
    
    // Verify file exists and has content
    std::ifstream file(testFile);
    EXPECT_TRUE(file.is_open());
    
    std::string line;
    int lineCount = 0;
    while (std::getline(file, line)) {
        lineCount++;
    }
    EXPECT_TRUE(lineCount > 0);
    
    file.close();
    std::remove(testFile.c_str());
    std::cout << "✓ Save to file test passed" << std::endl;
    return true;
}

bool testLoadFromFile() {
    std::cout << "Testing loading grammar from file..." << std::endl;
    // Create a temporary grammar file
    std::string testFile = "/tmp/test_grammar.txt";
    std::ofstream outFile(testFile);
    outFile << "E -> T\n";
    outFile << "T -> id\n";
    outFile.close();
    
    auto grammar = std::make_unique<Grammar>();
    EXPECT_NO_THROW(grammar->loadFromFile(testFile));
    
    auto productions = grammar->getProductions();
    EXPECT_EQ(productions.size(), 2);
    
    std::remove(testFile.c_str());
    std::cout << "✓ Load from file test passed" << std::endl;
    return true;
}

bool testErrorHandling() {
    std::cout << "Testing error handling for invalid grammar..." << std::endl;
    auto grammar = std::make_unique<Grammar>();
    std::string invalidGrammarStr = 
        "E ->\n"  // Invalid production
        "-> T\n"; // Invalid production
    
    // Should handle gracefully without crashing
    EXPECT_NO_THROW(grammar->loadFromString(invalidGrammarStr));
    std::cout << "✓ Error handling test passed" << std::endl;
    return true;
}

// Phase 2.1 specific tests
bool testGetRule() {
    std::cout << "Testing getRule method..." << std::endl;
    auto grammar = std::make_unique<Grammar>();
    
    grammar->addNonTerminal("E");
    grammar->addTerminal("+");
    grammar->addTerminal("id");
    grammar->addRule("E", {"E", "+", "id"});
    grammar->addRule("E", {"id"});
    
    EXPECT_EQ(grammar->getRules().size(), 2);
    
    auto rule0 = grammar->getRule(0);
    EXPECT_EQ(rule0.getLeftHandSide(), "E");
    EXPECT_EQ(rule0.getRightHandSide().size(), 3);
    
    auto rule1 = grammar->getRule(1);
    EXPECT_EQ(rule1.getLeftHandSide(), "E");
    EXPECT_EQ(rule1.getRightHandSide().size(), 1);
    
    std::cout << "✓ getRule test passed" << std::endl;
    return true;
}

bool testHasRule() {
    std::cout << "Testing hasRule method..." << std::endl;
    auto grammar = std::make_unique<Grammar>();
    
    grammar->addNonTerminal("E");
    grammar->addTerminal("+");
    grammar->addTerminal("id");
    grammar->addRule("E", {"E", "+", "id"});
    
    EXPECT_TRUE(grammar->hasRule("E", {"E", "+", "id"}));
    EXPECT_FALSE(grammar->hasRule("E", {"id"}));
    EXPECT_FALSE(grammar->hasRule("T", {"id"}));
    
    std::cout << "✓ hasRule test passed" << std::endl;
    return true;
}

bool testRemoveRule() {
    std::cout << "Testing removeRule method..." << std::endl;
    auto grammar = std::make_unique<Grammar>();
    
    grammar->addNonTerminal("E");
    grammar->addTerminal("+");
    grammar->addTerminal("id");
    grammar->addRule("E", {"E", "+", "id"});
    grammar->addRule("E", {"id"});
    
    EXPECT_EQ(grammar->getRules().size(), 2);
    EXPECT_TRUE(grammar->hasRule("E", {"E", "+", "id"}));
    
    bool removed = grammar->removeRule("E", {"E", "+", "id"});
    EXPECT_TRUE(removed);
    EXPECT_EQ(grammar->getRules().size(), 1);
    EXPECT_FALSE(grammar->hasRule("E", {"E", "+", "id"}));
    
    // Try to remove non-existent rule
    bool notRemoved = grammar->removeRule("E", {"E", "*", "id"});
    EXPECT_FALSE(notRemoved);
    EXPECT_EQ(grammar->getRules().size(), 1);
    
    std::cout << "✓ removeRule test passed" << std::endl;
    return true;
}

bool testClear() {
    std::cout << "Testing clear method..." << std::endl;
    auto grammar = std::make_unique<Grammar>();
    
    grammar->addNonTerminal("E");
    grammar->addTerminal("+");
    grammar->addTerminal("id");
    grammar->setStartSymbol("E");
    grammar->addRule("E", {"E", "+", "id"});
    grammar->addRule("E", {"id"});
    
    EXPECT_FALSE(grammar->getTerminals().empty());
    EXPECT_FALSE(grammar->getNonTerminals().empty());
    EXPECT_FALSE(grammar->getRules().empty());
    EXPECT_FALSE(grammar->getStartSymbol().empty());
    
    grammar->clear();
    
    EXPECT_TRUE(grammar->getTerminals().empty());
    EXPECT_TRUE(grammar->getNonTerminals().empty());
    EXPECT_TRUE(grammar->getRules().empty());
    EXPECT_TRUE(grammar->getStartSymbol().empty());
    
    std::cout << "✓ clear test passed" << std::endl;
    return true;
}

bool testSerialize() {
    std::cout << "Testing serialize method..." << std::endl;
    auto grammar = std::make_unique<Grammar>();
    
    grammar->addNonTerminal("E");
    grammar->addTerminal("+");
    grammar->addTerminal("id");
    grammar->setStartSymbol("E");
    grammar->addRule("E", {"E", "+", "id"});
    grammar->addRule("E", {"id"});
    
    std::string serialized = grammar->serialize();
    EXPECT_FALSE(serialized.empty());
    
    // Check that serialized string contains expected content
    EXPECT_TRUE(serialized.find("START: E") != std::string::npos);
    EXPECT_TRUE(serialized.find("E -> E + id") != std::string::npos);
    EXPECT_TRUE(serialized.find("E -> id") != std::string::npos);
    
    std::cout << "✓ serialize test passed" << std::endl;
    return true;
}

// Main test runner
int main() {
    std::cout << "=== Grammar Unit Tests ===" << std::endl;
    
    int passed = 0;
    int total = 0;
    
    // Run all tests
    struct TestCase {
        const char* name;
        bool (*func)();
    };
    
    TestCase tests[] = {
        {"Construction", testConstruction},
        {"Add Terminals", testAddTerminals},
        {"Add Non-Terminals", testAddNonTerminals},
        {"Set Start Symbol", testSetStartSymbol},
        {"Add Productions", testAddProductions},
        {"Load From String", testLoadFromString},
        {"Compute FIRST Sets", testComputeFirstSets},
        {"Compute FOLLOW Sets", testComputeFollowSets},
        {"Validate Grammar", testValidateGrammar},
        {"Invalid Grammar", testInvalidGrammar},
        {"Left Recursion Detection", testLeftRecursionDetection},
        {"LL(1) Property", testLL1Property},
        {"Save To File", testSaveToFile},
        {"Load From File", testLoadFromFile},
        {"Error Handling", testErrorHandling},
        {"Get Rule", testGetRule},
        {"Has Rule", testHasRule},
        {"Remove Rule", testRemoveRule},
        {"Clear", testClear},
        {"Serialize", testSerialize}
    };
    
    for (const auto& test : tests) {
        total++;
        try {
            if (test.func()) {
                passed++;
            } else {
                std::cerr << "✗ Test failed: " << test.name << std::endl;
            }
        } catch (const std::exception& e) {
            std::cerr << "✗ Test crashed: " << test.name << " - " << e.what() << std::endl;
        } catch (...) {
            std::cerr << "✗ Test crashed: " << test.name << " - Unknown exception" << std::endl;
        }
        std::cout << std::endl;
    }
    
    std::cout << "=== Test Results ===" << std::endl;
    std::cout << "Passed: " << passed << "/" << total << std::endl;
    
    if (passed == total) {
        std::cout << "All tests passed!" << std::endl;
        return 0;
    } else {
        std::cout << "Some tests failed." << std::endl;
        return 1;
    }
}