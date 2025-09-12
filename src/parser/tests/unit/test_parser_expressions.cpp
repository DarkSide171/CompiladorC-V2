#include "../../include/parser.hpp"
#include "../../include/parser_config.hpp"
#include "../../include/ast.hpp"
#include "../../include/token_stream.hpp"
#include "../../../lexer/include/lexer.hpp"
#include <iostream>
#include <cassert>
#include <memory>
#include <vector>

// Mock LexerParserBridge implementation for testing
class MockLexerParserBridge : public Parser::LexerParserBridge {
private:
    std::unique_ptr<Parser::TokenStream> tokenStream;
    
public:
    void setTokenStream(std::unique_ptr<Parser::TokenStream> stream) {
        tokenStream = std::move(stream);
    }
    
    std::unique_ptr<Parser::TokenStream> getTokenStream() override {
        return std::move(tokenStream);
    }
    
    bool hasMoreTokens() const override {
        return tokenStream && !tokenStream->isAtEnd();
    }
    
    size_t getCurrentLine() const override {
        return 1;
    }
    
    size_t getCurrentColumn() const override {
        return 1;
    }
    
    void reportLexerError(const std::string& message, const Parser::Position& pos) override {
        std::cout << "Lexer Error: " << message << " at line " << pos.line << ", column " << pos.column << std::endl;
    }
};

using namespace Parser;
using namespace Lexer;

// Mock TokenStream implementation for testing
class MockTokenStream : public TokenStream {
private:
    std::vector<Token> tokens;
    size_t position;
    
public:
    MockTokenStream(const std::vector<std::pair<TokenType, std::string>>& tokenData) : position(0) {
        for (size_t i = 0; i < tokenData.size(); ++i) {
            const auto& [type, lexeme] = tokenData[i];
            Lexer::Position pos{1, static_cast<int>(i + 1), static_cast<int>(i)};
            Lexer::Value value;
            if (type == TokenType::INTEGER_LITERAL) {
                value.type = ValueType::INTEGER;
                value.int_val = std::stoi(lexeme);
            } else {
                value.type = ValueType::STRING;
                value.string_val = lexeme;
            }
            tokens.emplace_back(type, lexeme, pos, value);
        }
        
        // Add EOF token
        Lexer::Position eofPos{1, static_cast<int>(tokenData.size() + 1), static_cast<int>(tokenData.size())};
        Lexer::Value eofValue;
        eofValue.type = ValueType::STRING;
        tokens.emplace_back(TokenType::END_OF_FILE, "", eofPos, eofValue);
    }
    
    const Token& current() const override {
        if (position < tokens.size()) {
            return tokens[position];
        }
        return tokens.back(); // EOF token
    }
    
    const Token& peek(size_t offset = 1) const override {
        size_t peekPos = position + offset;
        if (peekPos < tokens.size()) {
            return tokens[peekPos];
        }
        return tokens.back(); // EOF token
    }
    
    bool advance() override {
        if (position < tokens.size() - 1) {
            position++;
            return true;
        }
        return false;
    }
    
    bool isAtEnd() const override {
        return position >= tokens.size() - 1 || tokens[position].getType() == TokenType::END_OF_FILE;
    }
    
    size_t getPosition() const override {
        return position;
    }
    
    void setPosition(size_t pos) override {
        if (pos < tokens.size()) {
            position = pos;
        }
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
        if (start >= tokens.size() || end > tokens.size() || start >= end) {
            return {};
        }
        return std::vector<Token>(tokens.begin() + start, tokens.begin() + end);
    }
};

std::unique_ptr<MockTokenStream> createTokenStream(const std::vector<std::pair<TokenType, std::string>>& tokens) {
    return std::make_unique<MockTokenStream>(tokens);
}

// Test helper function that directly tests expression parsing methods
bool testExpressionParsing(const std::vector<std::pair<TokenType, std::string>>& tokens, const std::string& description) {
    std::cout << "Testing: " << description << std::endl;
    
    try {
        SyntacticAnalyzer parser;
        ParserConfig config;
        config.setCStandard(CStandard::C99);
        parser.setConfig(config);
        
        // DEBUG: Creating minimal expression grammar
        // Create a minimal grammar for expressions
        Grammar grammar;
        
        // Add terminals
        grammar.addTerminal("IDENTIFIER");
        grammar.addTerminal("NUMBER");
        grammar.addTerminal("+");
        grammar.addTerminal("-");
        grammar.addTerminal("*");
        grammar.addTerminal("/");
        grammar.addTerminal("(");
        grammar.addTerminal(")");
        grammar.addTerminal(";");
        grammar.addTerminal("EOF");
        
        // Add non-terminals
    grammar.addNonTerminal("program");
    grammar.addNonTerminal("expression");
    grammar.addNonTerminal("expression_rest");
    grammar.addNonTerminal("term");
    grammar.addNonTerminal("term_rest");
    grammar.addNonTerminal("factor");
        
        // Set start symbol
        grammar.setStartSymbol("program");
        
        // Add productions (without left recursion)
    grammar.addProduction("program", {"expression", "EOF"});
    grammar.addProduction("expression", {"term", "expression_rest"});
    grammar.addProduction("expression_rest", {"+", "term", "expression_rest"});
    grammar.addProduction("expression_rest", {"-", "term", "expression_rest"});
    grammar.addProduction("expression_rest", {});
    grammar.addProduction("term", {"factor", "term_rest"});
    grammar.addProduction("term_rest", {"*", "factor", "term_rest"});
    grammar.addProduction("term_rest", {"/", "factor", "term_rest"});
    grammar.addProduction("term_rest", {});
    grammar.addProduction("factor", {"(", "expression", ")"});
    grammar.addProduction("factor", {"IDENTIFIER"});
    grammar.addProduction("factor", {"NUMBER"});
        
        // DEBUG: Grammar created, checking validity
        
        if (!grammar.validate()) {
            // DEBUG: Grammar is invalid!
            auto errors = grammar.getValidationErrors();
            for (const auto& error : errors) {
                // DEBUG: Grammar error: [error]
            }
        } else {
            // DEBUG: Grammar is valid
        }
        
        // DEBUG: Setting grammar on parser
        parser.setGrammar(grammar);
        
        // Create and set lexer bridge
        // DEBUG: Creating lexer bridge
        auto lexerBridge = std::make_unique<MockLexerParserBridge>();
        
        // Create token stream
        // DEBUG: Creating token stream with [N] tokens
        auto tokenStream = createTokenStream(tokens);
        lexerBridge->setTokenStream(std::move(tokenStream));
        
        // DEBUG: Setting lexer bridge on parser
        parser.setLexerBridge(std::move(lexerBridge));
        
        // Parse using the main parse method
        // DEBUG: Starting to parse
        auto result = parser.parse();
        
        // DEBUG: Parse result - success: [true/false]
        if (!result.isSuccess()) {
            // DEBUG: Error message: [error]
        }
        
        if (result.isSuccess()) {
            std::cout << "  ✓ Successfully parsed expression" << std::endl;
            return true;
        } else {
            std::cout << "  ✗ Failed to parse expression: " << result.getError()->getMessage() << std::endl;
            return false;
        }
    } catch (const std::exception& e) {
        std::cout << "  ✗ Exception: " << e.what() << std::endl;
        return false;
    }
}

int main() {
    std::cout << "=== Parser Expressions Tests (Phase 3.5) ===" << std::endl;
    
    int passed = 0;
    int total = 0;
    
    // Test simple primary expressions
    total++; if (testExpressionParsing({{TokenType::INTEGER_LITERAL, "42"}}, "Integer literal")) passed++;
    total++; if (testExpressionParsing({{TokenType::FLOAT_LITERAL, "3.14"}}, "Float literal")) passed++;
    total++; if (testExpressionParsing({{TokenType::CHAR_LITERAL, "'A'"}}, "Character literal")) passed++;
    total++; if (testExpressionParsing({{TokenType::STRING_LITERAL, "\"hello\""}}, "String literal")) passed++;
    total++; if (testExpressionParsing({{TokenType::IDENTIFIER, "identifier"}}, "Identifier")) passed++;
    
    // Test arithmetic expressions
    total++; if (testExpressionParsing({{TokenType::IDENTIFIER, "a"}, {TokenType::PLUS, "+"}, {TokenType::IDENTIFIER, "b"}}, "Addition")) passed++;
    total++; if (testExpressionParsing({{TokenType::IDENTIFIER, "a"}, {TokenType::MINUS, "-"}, {TokenType::IDENTIFIER, "b"}}, "Subtraction")) passed++;
    total++; if (testExpressionParsing({{TokenType::IDENTIFIER, "a"}, {TokenType::MULTIPLY, "*"}, {TokenType::IDENTIFIER, "b"}}, "Multiplication")) passed++;
    total++; if (testExpressionParsing({{TokenType::IDENTIFIER, "a"}, {TokenType::DIVIDE, "/"}, {TokenType::IDENTIFIER, "b"}}, "Division")) passed++;
    total++; if (testExpressionParsing({{TokenType::IDENTIFIER, "a"}, {TokenType::MODULO, "%"}, {TokenType::IDENTIFIER, "b"}}, "Modulo")) passed++;
    
    // Test relational expressions
    total++; if (testExpressionParsing({{TokenType::IDENTIFIER, "a"}, {TokenType::LESS_THAN, "<"}, {TokenType::IDENTIFIER, "b"}}, "Less than")) passed++;
    total++; if (testExpressionParsing({{TokenType::IDENTIFIER, "a"}, {TokenType::GREATER_THAN, ">"}, {TokenType::IDENTIFIER, "b"}}, "Greater than")) passed++;
    total++; if (testExpressionParsing({{TokenType::IDENTIFIER, "a"}, {TokenType::LESS_EQUAL, "<="}, {TokenType::IDENTIFIER, "b"}}, "Less than or equal")) passed++;
    total++; if (testExpressionParsing({{TokenType::IDENTIFIER, "a"}, {TokenType::GREATER_EQUAL, ">="}, {TokenType::IDENTIFIER, "b"}}, "Greater than or equal")) passed++;
    
    // Test equality expressions
    total++; if (testExpressionParsing({{TokenType::IDENTIFIER, "a"}, {TokenType::EQUAL, "=="}, {TokenType::IDENTIFIER, "b"}}, "Equality")) passed++;
    total++; if (testExpressionParsing({{TokenType::IDENTIFIER, "a"}, {TokenType::NOT_EQUAL, "!="}, {TokenType::IDENTIFIER, "b"}}, "Inequality")) passed++;
    
    // Test logical expressions
    total++; if (testExpressionParsing({{TokenType::IDENTIFIER, "a"}, {TokenType::LOGICAL_AND, "&&"}, {TokenType::IDENTIFIER, "b"}}, "Logical AND")) passed++;
    total++; if (testExpressionParsing({{TokenType::IDENTIFIER, "a"}, {TokenType::LOGICAL_OR, "||"}, {TokenType::IDENTIFIER, "b"}}, "Logical OR")) passed++;
    
    // Test bitwise expressions
    total++; if (testExpressionParsing({{TokenType::IDENTIFIER, "a"}, {TokenType::BITWISE_AND, "&"}, {TokenType::IDENTIFIER, "b"}}, "Bitwise AND")) passed++;
    total++; if (testExpressionParsing({{TokenType::IDENTIFIER, "a"}, {TokenType::BITWISE_OR, "|"}, {TokenType::IDENTIFIER, "b"}}, "Bitwise OR")) passed++;
    total++; if (testExpressionParsing({{TokenType::IDENTIFIER, "a"}, {TokenType::BITWISE_XOR, "^"}, {TokenType::IDENTIFIER, "b"}}, "Bitwise XOR")) passed++;
    
    // Test shift expressions
    total++; if (testExpressionParsing({{TokenType::IDENTIFIER, "a"}, {TokenType::LEFT_SHIFT, "<<"}, {TokenType::INTEGER_LITERAL, "2"}}, "Left shift")) passed++;
    total++; if (testExpressionParsing({{TokenType::IDENTIFIER, "a"}, {TokenType::RIGHT_SHIFT, ">>"}, {TokenType::INTEGER_LITERAL, "3"}}, "Right shift")) passed++;
    
    // Test assignment expressions
    total++; if (testExpressionParsing({{TokenType::IDENTIFIER, "x"}, {TokenType::ASSIGN, "="}, {TokenType::INTEGER_LITERAL, "5"}}, "Simple assignment")) passed++;
    total++; if (testExpressionParsing({{TokenType::IDENTIFIER, "x"}, {TokenType::PLUS_ASSIGN, "+="}, {TokenType::INTEGER_LITERAL, "10"}}, "Addition assignment")) passed++;
    
    // Test unary expressions
    total++; if (testExpressionParsing({{TokenType::PLUS, "+"}, {TokenType::IDENTIFIER, "x"}}, "Unary plus")) passed++;
    total++; if (testExpressionParsing({{TokenType::MINUS, "-"}, {TokenType::IDENTIFIER, "x"}}, "Unary minus")) passed++;
    total++; if (testExpressionParsing({{TokenType::LOGICAL_NOT, "!"}, {TokenType::IDENTIFIER, "x"}}, "Logical NOT")) passed++;
    total++; if (testExpressionParsing({{TokenType::BITWISE_NOT, "~"}, {TokenType::IDENTIFIER, "x"}}, "Bitwise NOT")) passed++;
    
    // Test parenthesized expressions
    total++; if (testExpressionParsing({{TokenType::LEFT_PAREN, "("}, {TokenType::IDENTIFIER, "a"}, {TokenType::PLUS, "+"}, {TokenType::IDENTIFIER, "b"}, {TokenType::RIGHT_PAREN, ")"}}, "Parenthesized expression")) passed++;
    
    // Test complex expressions with precedence
    total++; if (testExpressionParsing({{TokenType::IDENTIFIER, "a"}, {TokenType::PLUS, "+"}, {TokenType::IDENTIFIER, "b"}, {TokenType::MULTIPLY, "*"}, {TokenType::IDENTIFIER, "c"}}, "Precedence: multiplication before addition")) passed++;
    total++; if (testExpressionParsing({{TokenType::LEFT_PAREN, "("}, {TokenType::IDENTIFIER, "a"}, {TokenType::PLUS, "+"}, {TokenType::IDENTIFIER, "b"}, {TokenType::RIGHT_PAREN, ")"}, {TokenType::MULTIPLY, "*"}, {TokenType::IDENTIFIER, "c"}}, "Parentheses override precedence")) passed++;
    
    std::cout << std::endl;
    std::cout << "=== Test Results ===" << std::endl;
    std::cout << "Passed: " << passed << "/" << total << " tests" << std::endl;
    
    if (passed == total) {
        std::cout << "✓ All expression parsing tests passed!" << std::endl;
        return 0;
    } else {
        std::cout << "✗ Some expression parsing tests failed." << std::endl;
        return 1;
    }
}