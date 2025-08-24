// Testes Unitários - Token
// Testes para a classe Token e enumeração TokenType
// Fase 5.1 - Testes Unitários Básicos

#include "../../include/token.hpp"
#include <cassert>
#include <iostream>
#include <sstream>

using namespace Lexer;

// 1. testTokenCreation() - Criação básica de tokens
void testTokenCreation() {
    std::cout << "Testando criação básica de tokens..." << std::endl;
    
    // Teste construtor padrão
    Token defaultToken;
    assert(defaultToken.getType() == TokenType::UNKNOWN);
    assert(defaultToken.getLexeme().empty());
    
    // Teste construtor com tipo, lexeme e posição
    Position pos{1, 5, 10};
    Token identifierToken(TokenType::IDENTIFIER, "variable", pos);
    assert(identifierToken.getType() == TokenType::IDENTIFIER);
    assert(identifierToken.getLexeme() == "variable");
    assert(identifierToken.getPosition().line == 1);
    assert(identifierToken.getPosition().column == 5);
    assert(identifierToken.getPosition().offset == 10);
    
    // Teste construtor com valor
    Value intValue;
    intValue.type = ValueType::INTEGER;
    intValue.int_val = 42;
    Token intToken(TokenType::INTEGER_LITERAL, "42", pos, intValue);
    assert(intToken.getType() == TokenType::INTEGER_LITERAL);
    assert(intToken.getLexeme() == "42");
    assert(intToken.getValue().type == ValueType::INTEGER);
    assert(intToken.getValue().int_val == 42);
    
    std::cout << "✓ Criação básica de tokens passou" << std::endl;
}

// 2. testTokenTypes() - Verificação de tipos
void testTokenTypes() {
    std::cout << "Testando verificação de tipos..." << std::endl;
    
    Position pos{1, 1, 0};
    
    // Teste palavras-chave
    Token ifToken(TokenType::IF, "if", pos);
    assert(ifToken.isKeyword());
    assert(!ifToken.isOperator());
    assert(!ifToken.isLiteral());
    
    Token whileToken(TokenType::WHILE, "while", pos);
    assert(whileToken.isKeyword());
    
    // Teste operadores
    Token plusToken(TokenType::PLUS, "+", pos);
    assert(plusToken.isOperator());
    assert(!plusToken.isKeyword());
    assert(!plusToken.isLiteral());
    
    Token assignToken(TokenType::ASSIGN, "=", pos);
    assert(assignToken.isOperator());
    
    // Teste literais
    Token stringToken(TokenType::STRING_LITERAL, "\"hello\"", pos);
    assert(stringToken.isLiteral());
    assert(!stringToken.isKeyword());
    assert(!stringToken.isOperator());
    
    Token intToken(TokenType::INTEGER_LITERAL, "123", pos);
    assert(intToken.isLiteral());
    
    // Teste identificadores
    Token idToken(TokenType::IDENTIFIER, "myVar", pos);
    assert(!idToken.isKeyword());
    assert(!idToken.isOperator());
    assert(!idToken.isLiteral());
    
    std::cout << "✓ Verificação de tipos passou" << std::endl;
}

// 3. testTokenComparison() - Comparação entre tokens
void testTokenComparison() {
    std::cout << "Testando comparação entre tokens..." << std::endl;
    
    Position pos1{1, 1, 0};
    Position pos2{2, 1, 10};
    
    // Tokens iguais
    Token token1(TokenType::IDENTIFIER, "var", pos1);
    Token token2(TokenType::IDENTIFIER, "var", pos1);
    assert(token1 == token2);
    assert(!(token1 != token2));
    
    // Tokens diferentes por tipo
    Token token3(TokenType::INTEGER_LITERAL, "var", pos1);
    assert(token1 != token3);
    assert(!(token1 == token3));
    
    // Tokens diferentes por lexeme
    Token token4(TokenType::IDENTIFIER, "other", pos1);
    assert(token1 != token4);
    
    // Tokens diferentes por posição
    Token token5(TokenType::IDENTIFIER, "var", pos2);
    assert(token1 != token5);
    
    // Teste com valores
    Value val1, val2;
    val1.type = ValueType::INTEGER;
    val1.int_val = 10;
    val2.type = ValueType::INTEGER;
    val2.int_val = 20;
    
    Token token6(TokenType::INTEGER_LITERAL, "10", pos1, val1);
    Token token7(TokenType::INTEGER_LITERAL, "20", pos1, val2);
    assert(token6 != token7);
    
    std::cout << "✓ Comparação entre tokens passou" << std::endl;
}

// 4. testTokenSerialization() - Serialização para string
void testTokenSerialization() {
    std::cout << "Testando serialização para string..." << std::endl;
    
    Position pos{1, 5, 10};
    
    // Teste toString()
    Token identifierToken(TokenType::IDENTIFIER, "myVar", pos);
    std::string tokenStr = identifierToken.toString();
    assert(!tokenStr.empty());
    assert(tokenStr.find("IDENTIFIER") != std::string::npos);
    assert(tokenStr.find("myVar") != std::string::npos);
    
    // Teste operador de stream
    std::ostringstream oss;
    oss << identifierToken;
    std::string streamStr = oss.str();
    assert(!streamStr.empty());
    
    // Teste tokenTypeToString
    std::string typeStr = tokenTypeToString(TokenType::IF);
    assert(typeStr == "IF");
    
    typeStr = tokenTypeToString(TokenType::PLUS);
    assert(typeStr == "PLUS");
    
    typeStr = tokenTypeToString(TokenType::INTEGER_LITERAL);
    assert(typeStr == "INTEGER_LITERAL");
    
    std::cout << "✓ Serialização para string passou" << std::endl;
}

// 5. testTokenValue() - Valores de literais
void testTokenValue() {
    std::cout << "Testando valores de literais..." << std::endl;
    
    Position pos{1, 1, 0};
    
    // Teste valor inteiro
    Value intValue;
    intValue.type = ValueType::INTEGER;
    intValue.int_val = 42;
    Token intToken(TokenType::INTEGER_LITERAL, "42", pos, intValue);
    assert(intToken.getValue().type == ValueType::INTEGER);
    assert(intToken.getValue().int_val == 42);
    
    // Teste valor float
    Value floatValue;
    floatValue.type = ValueType::FLOAT;
    floatValue.float_val = 3.14f;
    Token floatToken(TokenType::FLOAT_LITERAL, "3.14", pos, floatValue);
    assert(floatToken.getValue().type == ValueType::FLOAT);
    assert(floatToken.getValue().float_val == 3.14f);
    
    // Teste valor char
    Value charValue;
    charValue.type = ValueType::CHAR;
    charValue.char_val = 'A';
    Token charToken(TokenType::CHAR_LITERAL, "'A'", pos, charValue);
    assert(charToken.getValue().type == ValueType::CHAR);
    assert(charToken.getValue().char_val == 'A');
    
    // Teste valor string
    Value stringValue;
    stringValue.type = ValueType::STRING;
    stringValue.string_val = "hello";
    Token stringToken(TokenType::STRING_LITERAL, "\"hello\"", pos, stringValue);
    assert(stringToken.getValue().type == ValueType::STRING);
    assert(stringToken.getValue().string_val == "hello");
    
    // Teste valor NONE
    Token noValueToken(TokenType::IDENTIFIER, "var", pos);
    assert(noValueToken.getValue().type == ValueType::NONE);
    
    std::cout << "✓ Valores de literais passou" << std::endl;
}

// 6. testTokenPosition() - Posicionamento no código
void testTokenPosition() {
    std::cout << "Testando posicionamento no código..." << std::endl;
    
    // Teste posição básica
    Position pos1{10, 25, 150};
    Token token1(TokenType::IDENTIFIER, "var", pos1);
    const Position& tokenPos = token1.getPosition();
    assert(tokenPos.line == 10);
    assert(tokenPos.column == 25);
    assert(tokenPos.offset == 150);
    
    // Teste comparação de posições
    Position pos2{10, 25, 150};
    Position pos3{11, 1, 160};
    assert(pos1 == pos2);
    assert(pos1 != pos3);
    
    // Teste posição inicial
    Position startPos{1, 1, 0};
    Token startToken(TokenType::IF, "if", startPos);
    assert(startToken.getPosition().line == 1);
    assert(startToken.getPosition().column == 1);
    assert(startToken.getPosition().offset == 0);
    
    std::cout << "✓ Posicionamento no código passou" << std::endl;
}

// 7. testTokenUtilities() - Funções utilitárias
void testTokenUtilities() {
    std::cout << "Testando funções utilitárias..." << std::endl;
    
    // Teste isKeywordToken
    assert(isKeywordToken(TokenType::IF));
    assert(isKeywordToken(TokenType::WHILE));
    assert(isKeywordToken(TokenType::INT));
    assert(!isKeywordToken(TokenType::IDENTIFIER));
    assert(!isKeywordToken(TokenType::PLUS));
    
    // Teste isOperatorToken
    assert(isOperatorToken(TokenType::PLUS));
    assert(isOperatorToken(TokenType::ASSIGN));
    assert(isOperatorToken(TokenType::LOGICAL_AND));
    assert(!isOperatorToken(TokenType::IDENTIFIER));
    assert(!isOperatorToken(TokenType::IF));
    
    // Teste isLiteralToken
    assert(isLiteralToken(TokenType::INTEGER_LITERAL));
    assert(isLiteralToken(TokenType::STRING_LITERAL));
    assert(isLiteralToken(TokenType::FLOAT_LITERAL));
    assert(!isLiteralToken(TokenType::IDENTIFIER));
    assert(!isLiteralToken(TokenType::PLUS));
    
    // Teste precedência de operadores
    int plusPrec = getOperatorPrecedence(TokenType::PLUS);
    int multPrec = getOperatorPrecedence(TokenType::MULTIPLY);
    assert(multPrec < plusPrec); // * tem precedência maior que + (valores menores = maior precedência)
    
    int assignPrec = getOperatorPrecedence(TokenType::ASSIGN);
    assert(plusPrec < assignPrec); // + tem precedência maior que = (valores menores = maior precedência)
    
    // Teste associatividade
    Associativity plusAssoc = getOperatorAssociativity(TokenType::PLUS);
    assert(plusAssoc == Associativity::LEFT);
    
    Associativity assignAssoc = getOperatorAssociativity(TokenType::ASSIGN);
    assert(assignAssoc == Associativity::RIGHT);
    
    std::cout << "✓ Funções utilitárias passou" << std::endl;
}

// Função principal para executar todos os testes
int main() {
    std::cout << "=== Executando Testes Unitários - Token (Fase 5.1) ===" << std::endl;
    
    try {
        testTokenCreation();
        testTokenTypes();
        testTokenComparison();
        testTokenSerialization();
        testTokenValue();
        testTokenPosition();
        testTokenUtilities();
        
        std::cout << "\n🎉 Todos os testes de Token passaram com sucesso!" << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "❌ Erro durante os testes: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "❌ Erro desconhecido durante os testes" << std::endl;
        return 1;
    }
}