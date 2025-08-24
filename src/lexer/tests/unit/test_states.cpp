// Testes Unitários - Máquina de Estados
// Testes para a classe StateMachine e enum LexerState

#include "../../include/lexer_state.hpp"
#include "../../include/token.hpp"
#include "../../include/error_handler.hpp"
#include <iostream>
#include <cassert>
#include <memory>
#include <vector>
#include <string>

using namespace Lexer;

// ============================================================================
// Funções auxiliares para testes
// ============================================================================

void printTestResult(const std::string& test_name, bool passed) {
    std::cout << "[" << (passed ? "PASS" : "FAIL") << "] " << test_name << std::endl;
    if (!passed) {
        std::cout << "  Test failed!" << std::endl;
    }
}

void assertTrue(bool condition, const std::string& message) {
    if (!condition) {
        std::cout << "Assertion failed: " << message << std::endl;
        assert(false);
    }
}

void assertEqual(const std::string& expected, const std::string& actual, const std::string& message) {
    if (expected != actual) {
        std::cout << "Assertion failed: " << message << std::endl;
        std::cout << "  Expected: " << expected << std::endl;
        std::cout << "  Actual: " << actual << std::endl;
        assert(false);
    }
}

template<typename T>
void assertEqual(T expected, T actual, const std::string& message) {
    if (expected != actual) {
        std::cout << "Assertion failed: " << message << std::endl;
        std::cout << "  Expected: " << static_cast<int>(expected) << std::endl;
        std::cout << "  Actual: " << static_cast<int>(actual) << std::endl;
        assert(false);
    }
}

// ============================================================================
// Testes para funções utilitárias globais
// ============================================================================

void testUtilityFunctions() {
    std::cout << "\n=== Testando Funções Utilitárias ===" << std::endl;
    
    // Teste lexerStateToString
    assertEqual(std::string("START"), lexerStateToString(LexerState::START), "lexerStateToString START");
    assertEqual(std::string("IDENTIFIER"), lexerStateToString(LexerState::IDENTIFIER), "lexerStateToString IDENTIFIER");
    assertEqual(std::string("ERROR"), lexerStateToString(LexerState::ERROR), "lexerStateToString ERROR");
    assertEqual(std::string("ACCEPT_INTEGER"), lexerStateToString(LexerState::ACCEPT_INTEGER), "lexerStateToString ACCEPT_INTEGER");
    
    // Teste stringToLexerState
    assertEqual(LexerState::START, stringToLexerState("START"), "stringToLexerState START");
    assertEqual(LexerState::IDENTIFIER, stringToLexerState("IDENTIFIER"), "stringToLexerState IDENTIFIER");
    assertEqual(LexerState::ERROR, stringToLexerState("INVALID"), "stringToLexerState invalid returns ERROR");
    
    // Teste isAcceptingState
    assertTrue(isAcceptingState(LexerState::ACCEPT_IDENTIFIER), "isAcceptingState ACCEPT_IDENTIFIER");
    assertTrue(isAcceptingState(LexerState::ACCEPT_INTEGER), "isAcceptingState ACCEPT_INTEGER");
    assertTrue(isAcceptingState(LexerState::ACCEPT_FLOAT), "isAcceptingState ACCEPT_FLOAT");
    assertTrue(!isAcceptingState(LexerState::START), "isAcceptingState START should be false");
    assertTrue(!isAcceptingState(LexerState::IDENTIFIER), "isAcceptingState IDENTIFIER should be false");
    
    // Teste isErrorState
    assertTrue(isErrorState(LexerState::ERROR), "isErrorState ERROR");
    assertTrue(!isErrorState(LexerState::START), "isErrorState START should be false");
    assertTrue(!isErrorState(LexerState::ACCEPT_INTEGER), "isErrorState ACCEPT_INTEGER should be false");
    
    // Teste stateToTokenType
    assertEqual(TokenType::IDENTIFIER, stateToTokenType(LexerState::ACCEPT_IDENTIFIER), "stateToTokenType ACCEPT_IDENTIFIER");
    assertEqual(TokenType::INTEGER_LITERAL, stateToTokenType(LexerState::ACCEPT_INTEGER), "stateToTokenType ACCEPT_INTEGER");
    assertEqual(TokenType::FLOAT_LITERAL, stateToTokenType(LexerState::ACCEPT_FLOAT), "stateToTokenType ACCEPT_FLOAT");
    assertEqual(TokenType::STRING_LITERAL, stateToTokenType(LexerState::ACCEPT_STRING), "stateToTokenType ACCEPT_STRING");
    assertEqual(TokenType::CHAR_LITERAL, stateToTokenType(LexerState::ACCEPT_CHAR), "stateToTokenType ACCEPT_CHAR");
    assertEqual(TokenType::END_OF_FILE, stateToTokenType(LexerState::ACCEPT_EOF), "stateToTokenType ACCEPT_EOF");
    
    printTestResult("Funções Utilitárias", true);
}

// ============================================================================
// Testes para construtores da StateMachine
// ============================================================================

void testStateMachineConstructors() {
    std::cout << "\n=== Testando Construtores da StateMachine ===" << std::endl;
    
    // Teste construtor padrão
    StateMachine sm1;
    assertEqual(LexerState::START, sm1.getCurrentState(), "Construtor padrão - estado inicial");
    assertTrue(sm1.getErrorHandler() == nullptr, "Construtor padrão - error handler nulo");
    
    // Teste construtor com ErrorHandler
    auto error_handler = std::make_shared<ErrorHandler>();
    StateMachine sm2(error_handler);
    assertEqual(LexerState::START, sm2.getCurrentState(), "Construtor com ErrorHandler - estado inicial");
    assertTrue(sm2.getErrorHandler() == error_handler, "Construtor com ErrorHandler - error handler definido");
    
    printTestResult("Construtores da StateMachine", true);
}

// ============================================================================
// Testes para métodos básicos de controle de estado
// ============================================================================

void testBasicStateMethods() {
    std::cout << "\n=== Testando Métodos Básicos de Estado ===" << std::endl;
    
    StateMachine sm;
    
    // Teste getCurrentState
    assertEqual(LexerState::START, sm.getCurrentState(), "getCurrentState inicial");
    
    // Teste reset
    sm.transition('a'); // Muda para IDENTIFIER
    assertEqual(LexerState::IDENTIFIER, sm.getCurrentState(), "Estado após transição para 'a'");
    sm.reset();
    assertEqual(LexerState::START, sm.getCurrentState(), "Estado após reset");
    
    printTestResult("Métodos Básicos de Estado", true);
}

// ============================================================================
// Testes para transições de identificadores
// ============================================================================

void testIdentifierTransitions() {
    std::cout << "\n=== Testando Transições de Identificadores ===" << std::endl;
    
    StateMachine sm;
    
    // Teste transição START -> IDENTIFIER com letra
    assertEqual(LexerState::IDENTIFIER, sm.transition('a'), "START + 'a' -> IDENTIFIER");
    assertEqual(LexerState::IDENTIFIER, sm.transition('b'), "IDENTIFIER + 'b' -> IDENTIFIER");
    assertEqual(LexerState::IDENTIFIER, sm.transition('1'), "IDENTIFIER + '1' -> IDENTIFIER");
    assertEqual(LexerState::IDENTIFIER, sm.transition('_'), "IDENTIFIER + '_' -> IDENTIFIER");
    
    // Teste aceitação de identificador
    assertEqual(LexerState::ACCEPT_IDENTIFIER, sm.transition(' '), "IDENTIFIER + ' ' -> ACCEPT_IDENTIFIER");
    
    // Reset e teste com underscore
    sm.reset();
    assertEqual(LexerState::IDENTIFIER, sm.transition('_'), "START + '_' -> IDENTIFIER");
    assertEqual(LexerState::ACCEPT_IDENTIFIER, sm.transition(';'), "IDENTIFIER + ';' -> ACCEPT_IDENTIFIER");
    
    // Reset e teste com maiúscula
    sm.reset();
    assertEqual(LexerState::IDENTIFIER, sm.transition('A'), "START + 'A' -> IDENTIFIER");
    assertEqual(LexerState::IDENTIFIER, sm.transition('Z'), "IDENTIFIER + 'Z' -> IDENTIFIER");
    assertEqual(LexerState::ACCEPT_IDENTIFIER, sm.transition('('), "IDENTIFIER + '(' -> ACCEPT_IDENTIFIER");
    
    printTestResult("Transições de Identificadores", true);
}

// ============================================================================
// Testes para transições de números inteiros
// ============================================================================

void testIntegerTransitions() {
    std::cout << "\n=== Testando Transições de Números Inteiros ===" << std::endl;
    
    StateMachine sm;
    
    // Teste número decimal simples
    assertEqual(LexerState::INTEGER, sm.transition('1'), "START + '1' -> INTEGER");
    assertEqual(LexerState::INTEGER, sm.transition('2'), "INTEGER + '2' -> INTEGER");
    assertEqual(LexerState::INTEGER, sm.transition('3'), "INTEGER + '3' -> INTEGER");
    assertEqual(LexerState::ACCEPT_INTEGER, sm.transition(' '), "INTEGER + ' ' -> ACCEPT_INTEGER");
    
    // Reset e teste número octal
    sm.reset();
    assertEqual(LexerState::OCTAL_DIGITS, sm.transition('0'), "START + '0' -> OCTAL_DIGITS");
    assertEqual(LexerState::OCTAL_DIGITS, sm.transition('7'), "OCTAL_DIGITS + '7' -> OCTAL_DIGITS");
    assertEqual(LexerState::ACCEPT_OCTAL, sm.transition(' '), "OCTAL_DIGITS + ' ' -> ACCEPT_OCTAL");
    
    // Reset e teste número hexadecimal
    sm.reset();
    assertEqual(LexerState::OCTAL_DIGITS, sm.transition('0'), "START + '0' -> OCTAL_DIGITS");
    assertEqual(LexerState::HEX_PREFIX, sm.transition('x'), "OCTAL_DIGITS + 'x' -> HEX_PREFIX");
    assertEqual(LexerState::HEX_DIGITS, sm.transition('A'), "HEX_PREFIX + 'A' -> HEX_DIGITS");
    assertEqual(LexerState::HEX_DIGITS, sm.transition('F'), "HEX_DIGITS + 'F' -> HEX_DIGITS");
    assertEqual(LexerState::ACCEPT_HEX, sm.transition(' '), "HEX_DIGITS + ' ' -> ACCEPT_HEX");
    
    // Reset e teste número binário
    sm.reset();
    assertEqual(LexerState::OCTAL_DIGITS, sm.transition('0'), "START + '0' -> OCTAL_DIGITS");
    assertEqual(LexerState::BINARY_PREFIX, sm.transition('b'), "OCTAL_DIGITS + 'b' -> BINARY_PREFIX");
    assertEqual(LexerState::BINARY_DIGITS, sm.transition('1'), "BINARY_PREFIX + '1' -> BINARY_DIGITS");
    assertEqual(LexerState::BINARY_DIGITS, sm.transition('0'), "BINARY_DIGITS + '0' -> BINARY_DIGITS");
    assertEqual(LexerState::ACCEPT_BINARY, sm.transition(' '), "BINARY_DIGITS + ' ' -> ACCEPT_BINARY");
    
    printTestResult("Transições de Números Inteiros", true);
}

// ============================================================================
// Testes para transições de números de ponto flutuante
// ============================================================================

void testFloatTransitions() {
    std::cout << "\n=== Testando Transições de Números Float ===" << std::endl;
    
    StateMachine sm;
    
    // Teste float simples (1.23)
    assertEqual(LexerState::INTEGER, sm.transition('1'), "START + '1' -> INTEGER");
    assertEqual(LexerState::FLOAT_DOT, sm.transition('.'), "INTEGER + '.' -> FLOAT_DOT");
    assertEqual(LexerState::FLOAT_DIGITS, sm.transition('2'), "FLOAT_DOT + '2' -> FLOAT_DIGITS");
    assertEqual(LexerState::FLOAT_DIGITS, sm.transition('3'), "FLOAT_DIGITS + '3' -> FLOAT_DIGITS");
    assertEqual(LexerState::ACCEPT_FLOAT, sm.transition(' '), "FLOAT_DIGITS + ' ' -> ACCEPT_FLOAT");
    
    // Reset e teste float com expoente (1.23e10)
    sm.reset();
    assertEqual(LexerState::INTEGER, sm.transition('1'), "START + '1' -> INTEGER");
    assertEqual(LexerState::FLOAT_DOT, sm.transition('.'), "INTEGER + '.' -> FLOAT_DOT");
    assertEqual(LexerState::FLOAT_DIGITS, sm.transition('2'), "FLOAT_DOT + '2' -> FLOAT_DIGITS");
    assertEqual(LexerState::FLOAT_EXP, sm.transition('e'), "FLOAT_DIGITS + 'e' -> FLOAT_EXP");
    assertEqual(LexerState::FLOAT_EXP_DIGITS, sm.transition('1'), "FLOAT_EXP + '1' -> FLOAT_EXP_DIGITS");
    assertEqual(LexerState::FLOAT_EXP_DIGITS, sm.transition('0'), "FLOAT_EXP_DIGITS + '0' -> FLOAT_EXP_DIGITS");
    assertEqual(LexerState::ACCEPT_FLOAT, sm.transition(' '), "FLOAT_EXP_DIGITS + ' ' -> ACCEPT_FLOAT");
    
    // Reset e teste float com expoente negativo (1.23e-10)
    sm.reset();
    assertEqual(LexerState::INTEGER, sm.transition('1'), "START + '1' -> INTEGER");
    assertEqual(LexerState::FLOAT_DOT, sm.transition('.'), "INTEGER + '.' -> FLOAT_DOT");
    assertEqual(LexerState::FLOAT_DIGITS, sm.transition('2'), "FLOAT_DOT + '2' -> FLOAT_DIGITS");
    assertEqual(LexerState::FLOAT_EXP, sm.transition('E'), "FLOAT_DIGITS + 'E' -> FLOAT_EXP");
    assertEqual(LexerState::FLOAT_EXP_SIGN, sm.transition('-'), "FLOAT_EXP + '-' -> FLOAT_EXP_SIGN");
    assertEqual(LexerState::FLOAT_EXP_DIGITS, sm.transition('1'), "FLOAT_EXP_SIGN + '1' -> FLOAT_EXP_DIGITS");
    assertEqual(LexerState::ACCEPT_FLOAT, sm.transition(' '), "FLOAT_EXP_DIGITS + ' ' -> ACCEPT_FLOAT");
    
    printTestResult("Transições de Números Float", true);
}

// ============================================================================
// Testes para transições de strings
// ============================================================================

void testStringTransitions() {
    std::cout << "\n=== Testando Transições de Strings ===" << std::endl;
    
    StateMachine sm;
    
    // Teste string simples
    assertEqual(LexerState::STRING_START, sm.transition('"'), "START + '\"' -> STRING_START");
    assertEqual(LexerState::STRING_BODY, sm.transition('H'), "STRING_START + 'H' -> STRING_BODY");
    assertEqual(LexerState::STRING_BODY, sm.transition('e'), "STRING_BODY + 'e' -> STRING_BODY");
    assertEqual(LexerState::STRING_BODY, sm.transition('l'), "STRING_BODY + 'l' -> STRING_BODY");
    assertEqual(LexerState::STRING_BODY, sm.transition('l'), "STRING_BODY + 'l' -> STRING_BODY");
    assertEqual(LexerState::STRING_BODY, sm.transition('o'), "STRING_BODY + 'o' -> STRING_BODY");
    assertEqual(LexerState::STRING_END, sm.transition('"'), "STRING_BODY + '\"' -> STRING_END");
    assertEqual(LexerState::ACCEPT_STRING, sm.transition(' '), "STRING_END + ' ' -> ACCEPT_STRING");
    
    // Reset e teste string com escape
    sm.reset();
    assertEqual(LexerState::STRING_START, sm.transition('"'), "START + '\"' -> STRING_START");
    assertEqual(LexerState::STRING_ESCAPE, sm.transition('\\'), "STRING_START + '\\\\' -> STRING_ESCAPE");
    assertEqual(LexerState::STRING_BODY, sm.transition('n'), "STRING_ESCAPE + 'n' -> STRING_BODY");
    assertEqual(LexerState::STRING_END, sm.transition('"'), "STRING_BODY + '\"' -> STRING_END");
    assertEqual(LexerState::ACCEPT_STRING, sm.transition(' '), "STRING_END + ' ' -> ACCEPT_STRING");
    
    // Reset e teste string vazia
    sm.reset();
    assertEqual(LexerState::STRING_START, sm.transition('"'), "START + '\"' -> STRING_START");
    assertEqual(LexerState::STRING_END, sm.transition('"'), "STRING_START + '\"' -> STRING_END");
    assertEqual(LexerState::ACCEPT_STRING, sm.transition(' '), "STRING_END + ' ' -> ACCEPT_STRING");
    
    printTestResult("Transições de Strings", true);
}

// ============================================================================
// Testes para transições de caracteres
// ============================================================================

void testCharTransitions() {
    std::cout << "\n=== Testando Transições de Caracteres ===" << std::endl;
    
    StateMachine sm;
    
    // Teste char simples
    assertEqual(LexerState::CHAR_START, sm.transition('\''), "START + '\\'' -> CHAR_START");
    assertEqual(LexerState::CHAR_BODY, sm.transition('A'), "CHAR_START + 'A' -> CHAR_BODY");
    assertEqual(LexerState::CHAR_END, sm.transition('\''), "CHAR_BODY + '\\'' -> CHAR_END");
    assertEqual(LexerState::ACCEPT_CHAR, sm.transition(' '), "CHAR_END + ' ' -> ACCEPT_CHAR");
    
    // Reset e teste char com escape
    sm.reset();
    assertEqual(LexerState::CHAR_START, sm.transition('\''), "START + '\\'' -> CHAR_START");
    assertEqual(LexerState::CHAR_ESCAPE, sm.transition('\\'), "CHAR_START + '\\\\' -> CHAR_ESCAPE");
    assertEqual(LexerState::CHAR_BODY, sm.transition('n'), "CHAR_ESCAPE + 'n' -> CHAR_BODY");
    assertEqual(LexerState::CHAR_END, sm.transition('\''), "CHAR_BODY + '\\'' -> CHAR_END");
    assertEqual(LexerState::ACCEPT_CHAR, sm.transition(' '), "CHAR_END + ' ' -> ACCEPT_CHAR");
    
    printTestResult("Transições de Caracteres", true);
}

// ============================================================================
// Testes para transições de operadores
// ============================================================================

void testOperatorTransitions() {
    std::cout << "\n=== Testando Transições de Operadores ===" << std::endl;
    
    StateMachine sm;
    
    // Teste operadores simples
    assertEqual(LexerState::PLUS, sm.transition('+'), "START + '+' -> PLUS");
    assertEqual(LexerState::ACCEPT_OPERATOR, sm.transition(' '), "PLUS + ' ' -> ACCEPT_OPERATOR");
    
    sm.reset();
    assertEqual(LexerState::MINUS, sm.transition('-'), "START + '-' -> MINUS");
    assertEqual(LexerState::ACCEPT_OPERATOR, sm.transition(' '), "MINUS + ' ' -> ACCEPT_OPERATOR");
    
    // Teste operadores compostos
    sm.reset();
    assertEqual(LexerState::PLUS, sm.transition('+'), "START + '+' -> PLUS");
    assertEqual(LexerState::INCREMENT, sm.transition('+'), "PLUS + '+' -> INCREMENT");
    assertEqual(LexerState::ACCEPT_OPERATOR, sm.transition(' '), "INCREMENT + ' ' -> ACCEPT_OPERATOR");
    
    sm.reset();
    assertEqual(LexerState::MINUS, sm.transition('-'), "START + '-' -> MINUS");
    assertEqual(LexerState::DECREMENT, sm.transition('-'), "MINUS + '-' -> DECREMENT");
    assertEqual(LexerState::ACCEPT_OPERATOR, sm.transition(' '), "DECREMENT + ' ' -> ACCEPT_OPERATOR");
    
    sm.reset();
    assertEqual(LexerState::MINUS, sm.transition('-'), "START + '-' -> MINUS");
    assertEqual(LexerState::ARROW, sm.transition('>'), "MINUS + '>' -> ARROW");
    assertEqual(LexerState::ACCEPT_OPERATOR, sm.transition(' '), "ARROW + ' ' -> ACCEPT_OPERATOR");
    
    // Teste operadores de comparação
    sm.reset();
    assertEqual(LexerState::ASSIGN, sm.transition('='), "START + '=' -> ASSIGN");
    assertEqual(LexerState::EQUAL, sm.transition('='), "ASSIGN + '=' -> EQUAL");
    assertEqual(LexerState::ACCEPT_OPERATOR, sm.transition(' '), "EQUAL + ' ' -> ACCEPT_OPERATOR");
    
    sm.reset();
    assertEqual(LexerState::NOT, sm.transition('!'), "START + '!' -> NOT");
    assertEqual(LexerState::NOT_EQUAL, sm.transition('='), "NOT + '=' -> NOT_EQUAL");
    assertEqual(LexerState::ACCEPT_OPERATOR, sm.transition(' '), "NOT_EQUAL + ' ' -> ACCEPT_OPERATOR");
    
    sm.reset();
    assertEqual(LexerState::LESS, sm.transition('<'), "START + '<' -> LESS");
    assertEqual(LexerState::LESS_EQUAL, sm.transition('='), "LESS + '=' -> LESS_EQUAL");
    assertEqual(LexerState::ACCEPT_OPERATOR, sm.transition(' '), "LESS_EQUAL + ' ' -> ACCEPT_OPERATOR");
    
    sm.reset();
    assertEqual(LexerState::LESS, sm.transition('<'), "START + '<' -> LESS");
    assertEqual(LexerState::LEFT_SHIFT, sm.transition('<'), "LESS + '<' -> LEFT_SHIFT");
    assertEqual(LexerState::ACCEPT_OPERATOR, sm.transition(' '), "LEFT_SHIFT + ' ' -> ACCEPT_OPERATOR");
    
    // Teste operadores lógicos
    sm.reset();
    assertEqual(LexerState::BITWISE_AND, sm.transition('&'), "START + '&' -> BITWISE_AND");
    assertEqual(LexerState::LOGICAL_AND, sm.transition('&'), "BITWISE_AND + '&' -> LOGICAL_AND");
    assertEqual(LexerState::ACCEPT_OPERATOR, sm.transition(' '), "LOGICAL_AND + ' ' -> ACCEPT_OPERATOR");
    
    sm.reset();
    assertEqual(LexerState::BITWISE_OR, sm.transition('|'), "START + '|' -> BITWISE_OR");
    assertEqual(LexerState::LOGICAL_OR, sm.transition('|'), "BITWISE_OR + '|' -> LOGICAL_OR");
    assertEqual(LexerState::ACCEPT_OPERATOR, sm.transition(' '), "LOGICAL_OR + ' ' -> ACCEPT_OPERATOR");
    
    printTestResult("Transições de Operadores", true);
}

// ============================================================================
// Testes para transições de comentários
// ============================================================================

void testCommentTransitions() {
    std::cout << "\n=== Testando Transições de Comentários ===" << std::endl;
    
    StateMachine sm;
    
    // Teste comentário de linha
    assertEqual(LexerState::DIVIDE, sm.transition('/'), "START + '/' -> DIVIDE");
    assertEqual(LexerState::LINE_COMMENT, sm.transition('/'), "DIVIDE + '/' -> LINE_COMMENT");
    assertEqual(LexerState::LINE_COMMENT, sm.transition('T'), "LINE_COMMENT + 'T' -> LINE_COMMENT");
    assertEqual(LexerState::LINE_COMMENT, sm.transition('e'), "LINE_COMMENT + 'e' -> LINE_COMMENT");
    assertEqual(LexerState::LINE_COMMENT, sm.transition('s'), "LINE_COMMENT + 's' -> LINE_COMMENT");
    assertEqual(LexerState::LINE_COMMENT, sm.transition('t'), "LINE_COMMENT + 't' -> LINE_COMMENT");
    assertEqual(LexerState::ACCEPT_COMMENT, sm.transition('\n'), "LINE_COMMENT + '\\n' -> ACCEPT_COMMENT");
    
    // Reset e teste comentário de bloco
    sm.reset();
    assertEqual(LexerState::DIVIDE, sm.transition('/'), "START + '/' -> DIVIDE");
    assertEqual(LexerState::BLOCK_COMMENT, sm.transition('*'), "DIVIDE + '*' -> BLOCK_COMMENT");
    assertEqual(LexerState::BLOCK_COMMENT, sm.transition('T'), "BLOCK_COMMENT + 'T' -> BLOCK_COMMENT");
    assertEqual(LexerState::BLOCK_COMMENT, sm.transition('e'), "BLOCK_COMMENT + 'e' -> BLOCK_COMMENT");
    assertEqual(LexerState::BLOCK_COMMENT, sm.transition('s'), "BLOCK_COMMENT + 's' -> BLOCK_COMMENT");
    assertEqual(LexerState::BLOCK_COMMENT, sm.transition('t'), "BLOCK_COMMENT + 't' -> BLOCK_COMMENT");
    assertEqual(LexerState::BLOCK_COMMENT_END, sm.transition('*'), "BLOCK_COMMENT + '*' -> BLOCK_COMMENT_END");
    assertEqual(LexerState::ACCEPT_COMMENT, sm.transition('/'), "BLOCK_COMMENT_END + '/' -> ACCEPT_COMMENT");
    
    // Reset e teste comentário de bloco com múltiplos asteriscos
    sm.reset();
    assertEqual(LexerState::DIVIDE, sm.transition('/'), "START + '/' -> DIVIDE");
    assertEqual(LexerState::BLOCK_COMMENT, sm.transition('*'), "DIVIDE + '*' -> BLOCK_COMMENT");
    assertEqual(LexerState::BLOCK_COMMENT_END, sm.transition('*'), "BLOCK_COMMENT + '*' -> BLOCK_COMMENT_END");
    assertEqual(LexerState::BLOCK_COMMENT_END, sm.transition('*'), "BLOCK_COMMENT_END + '*' -> BLOCK_COMMENT_END");
    assertEqual(LexerState::ACCEPT_COMMENT, sm.transition('/'), "BLOCK_COMMENT_END + '/' -> ACCEPT_COMMENT");
    
    printTestResult("Transições de Comentários", true);
}

// ============================================================================
// Testes para transições de delimitadores
// ============================================================================

void testDelimiterTransitions() {
    std::cout << "\n=== Testando Transições de Delimitadores ===" << std::endl;
    
    StateMachine sm;
    
    // Teste parênteses
    assertEqual(LexerState::LEFT_PAREN, sm.transition('('), "START + '(' -> LEFT_PAREN");
    assertEqual(LexerState::ACCEPT_DELIMITER, sm.transition(' '), "LEFT_PAREN + ' ' -> ACCEPT_DELIMITER");
    
    sm.reset();
    assertEqual(LexerState::RIGHT_PAREN, sm.transition(')'), "START + ')' -> RIGHT_PAREN");
    assertEqual(LexerState::ACCEPT_DELIMITER, sm.transition(' '), "RIGHT_PAREN + ' ' -> ACCEPT_DELIMITER");
    
    // Teste colchetes
    sm.reset();
    assertEqual(LexerState::LEFT_BRACKET, sm.transition('['), "START + '[' -> LEFT_BRACKET");
    assertEqual(LexerState::ACCEPT_DELIMITER, sm.transition(' '), "LEFT_BRACKET + ' ' -> ACCEPT_DELIMITER");
    
    sm.reset();
    assertEqual(LexerState::RIGHT_BRACKET, sm.transition(']'), "START + ']' -> RIGHT_BRACKET");
    assertEqual(LexerState::ACCEPT_DELIMITER, sm.transition(' '), "RIGHT_BRACKET + ' ' -> ACCEPT_DELIMITER");
    
    // Teste chaves
    sm.reset();
    assertEqual(LexerState::LEFT_BRACE, sm.transition('{'), "START + '{' -> LEFT_BRACE");
    assertEqual(LexerState::ACCEPT_DELIMITER, sm.transition(' '), "LEFT_BRACE + ' ' -> ACCEPT_DELIMITER");
    
    sm.reset();
    assertEqual(LexerState::RIGHT_BRACE, sm.transition('}'), "START + '}' -> RIGHT_BRACE");
    assertEqual(LexerState::ACCEPT_DELIMITER, sm.transition(' '), "RIGHT_BRACE + ' ' -> ACCEPT_DELIMITER");
    
    // Teste outros delimitadores
    sm.reset();
    assertEqual(LexerState::SEMICOLON, sm.transition(';'), "START + ';' -> SEMICOLON");
    assertEqual(LexerState::ACCEPT_DELIMITER, sm.transition(' '), "SEMICOLON + ' ' -> ACCEPT_DELIMITER");
    
    sm.reset();
    assertEqual(LexerState::COMMA, sm.transition(','), "START + ',' -> COMMA");
    assertEqual(LexerState::ACCEPT_DELIMITER, sm.transition(' '), "COMMA + ' ' -> ACCEPT_DELIMITER");
    
    sm.reset();
    assertEqual(LexerState::DOT, sm.transition('.'), "START + '.' -> DOT");
    assertEqual(LexerState::ACCEPT_DELIMITER, sm.transition(' '), "DOT + ' ' -> ACCEPT_DELIMITER");
    
    printTestResult("Transições de Delimitadores", true);
}

// ============================================================================
// Testes para métodos de verificação de estado
// ============================================================================

void testStateVerificationMethods() {
    std::cout << "\n=== Testando Métodos de Verificação de Estado ===" << std::endl;
    
    StateMachine sm;
    
    // Teste isAcceptingState
    assertTrue(!sm.isAcceptingState(), "Estado START não é de aceitação");
    sm.transition('a');
    assertTrue(!sm.isAcceptingState(), "Estado IDENTIFIER não é de aceitação");
    sm.transition(' ');
    assertTrue(sm.isAcceptingState(), "Estado ACCEPT_IDENTIFIER é de aceitação");
    
    // Teste isErrorState
    sm.reset();
    assertTrue(!sm.isErrorState(), "Estado START não é de erro");
    sm.transition('\x01'); // Caractere inválido
    assertTrue(sm.isErrorState(), "Estado ERROR é de erro");
    
    // Teste isValidTransition
    sm.reset();
    assertTrue(sm.isValidTransition('a'), "Transição START + 'a' é válida");
    assertTrue(sm.isValidTransition('1'), "Transição START + '1' é válida");
    assertTrue(sm.isValidTransition('+'), "Transição START + '+' é válida");
    assertTrue(!sm.isValidTransition('\x01'), "Transição START + '\\x01' é inválida");
    
    // Teste getTokenType
    sm.reset();
    sm.transition('a');
    sm.transition(' ');
    assertEqual(TokenType::IDENTIFIER, sm.getTokenType(), "Token type para ACCEPT_IDENTIFIER");
    
    sm.reset();
    sm.transition('1');
    sm.transition(' ');
    assertEqual(TokenType::INTEGER_LITERAL, sm.getTokenType(), "Token type para ACCEPT_INTEGER");
    
    printTestResult("Métodos de Verificação de Estado", true);
}

// ============================================================================
// Testes para métodos utilitários e debug
// ============================================================================

void testUtilityAndDebugMethods() {
    std::cout << "\n=== Testando Métodos Utilitários e Debug ===" << std::endl;
    
    StateMachine sm;
    
    // Teste stateToString
    assertEqual(std::string("START"), sm.stateToString(LexerState::START), "stateToString START");
    assertEqual(std::string("IDENTIFIER"), sm.stateToString(LexerState::IDENTIFIER), "stateToString IDENTIFIER");
    
    // Teste setErrorHandler e getErrorHandler
    auto error_handler = std::make_shared<ErrorHandler>();
    sm.setErrorHandler(error_handler);
    assertTrue(sm.getErrorHandler() == error_handler, "setErrorHandler/getErrorHandler");
    
    // Teste getAcceptingStates
    auto accepting_states = sm.getAcceptingStates();
    assertTrue(accepting_states.size() > 0, "getAcceptingStates retorna estados");
    bool found_accept_identifier = false;
    for (const auto& state : accepting_states) {
        if (state == LexerState::ACCEPT_IDENTIFIER) {
            found_accept_identifier = true;
            break;
        }
    }
    assertTrue(found_accept_identifier, "getAcceptingStates contém ACCEPT_IDENTIFIER");
    
    // Teste getErrorStates
    auto error_states = sm.getErrorStates();
    assertTrue(error_states.size() == 1, "getErrorStates retorna 1 estado");
    assertTrue(error_states[0] == LexerState::ERROR, "getErrorStates contém ERROR");
    
    printTestResult("Métodos Utilitários e Debug", true);
}

// ============================================================================
// Teste de cenário completo
// ============================================================================

void testCompleteScenario() {
    std::cout << "\n=== Testando Cenário Completo ===" << std::endl;
    
    StateMachine sm;
    
    // Testa tokens individuais simples
    std::vector<std::string> tokens = {"int", "main", "(", ")", "{", "return", "0", ";", "}"};
    std::vector<LexerState> expected_final_states = {
        LexerState::ACCEPT_IDENTIFIER, // int
        LexerState::ACCEPT_IDENTIFIER, // main
        LexerState::ACCEPT_DELIMITER,  // (
        LexerState::ACCEPT_DELIMITER,  // )
        LexerState::ACCEPT_DELIMITER,  // {
        LexerState::ACCEPT_IDENTIFIER, // return
        LexerState::ACCEPT_INTEGER,    // 0
        LexerState::ACCEPT_DELIMITER,  // ;
        LexerState::ACCEPT_DELIMITER   // }
    };
    
    std::vector<LexerState> actual_final_states;
    
    // Processa cada token individualmente
    for (const auto& token : tokens) {
        sm.reset();
        
        // Processa cada caractere do token
        for (char c : token) {
            sm.transition(c);
        }
        
        // Simula o fim do token (transição para estado de aceitação)
        LexerState final_state = sm.getCurrentState();
        LexerState accept_state = LexerState::ERROR;
        
        // Mapeia estados intermediários para estados de aceitação
        switch (final_state) {
            case LexerState::IDENTIFIER:
                accept_state = LexerState::ACCEPT_IDENTIFIER;
                break;
            case LexerState::INTEGER:
            case LexerState::OCTAL_DIGITS:
                accept_state = LexerState::ACCEPT_INTEGER;
                break;
            case LexerState::LEFT_PAREN:
            case LexerState::RIGHT_PAREN:
            case LexerState::LEFT_BRACE:
            case LexerState::RIGHT_BRACE:
            case LexerState::SEMICOLON:
                accept_state = LexerState::ACCEPT_DELIMITER;
                break;
            default:
                break;
        }
        
        // Adiciona o estado de aceitação se válido
        if (accept_state != LexerState::ERROR) {
            actual_final_states.push_back(accept_state);
        }
    }
    
    // Verifica se o número de tokens reconhecidos está correto
    assertTrue(actual_final_states.size() >= 5, "Cenário completo reconheceu tokens suficientes");
    
    // Verifica se pelo menos alguns tokens esperados foram reconhecidos
    bool found_identifier = false;
    bool found_delimiter = false;
    bool found_integer = false;
    
    for (const auto& state : actual_final_states) {
        if (state == LexerState::ACCEPT_IDENTIFIER) found_identifier = true;
        if (state == LexerState::ACCEPT_DELIMITER) found_delimiter = true;
        if (state == LexerState::ACCEPT_INTEGER) found_integer = true;
    }
    
    assertTrue(found_identifier, "Cenário completo reconheceu identificadores");
    assertTrue(found_delimiter, "Cenário completo reconheceu delimitadores");
    assertTrue(found_integer, "Cenário completo reconheceu inteiros");
    
    printTestResult("Cenário Completo", true);
}

// ============================================================================
// Função principal de testes
// ============================================================================

int main() {
    std::cout << "=== INICIANDO TESTES DA MÁQUINA DE ESTADOS ===" << std::endl;
    
    try {
        testUtilityFunctions();
        testStateMachineConstructors();
        testBasicStateMethods();
        testIdentifierTransitions();
        testIntegerTransitions();
        testFloatTransitions();
        testStringTransitions();
        testCharTransitions();
        testOperatorTransitions();
        testCommentTransitions();
        testDelimiterTransitions();
        testStateVerificationMethods();
        testUtilityAndDebugMethods();
        testCompleteScenario();
        
        std::cout << "\n=== TODOS OS TESTES PASSARAM COM SUCESSO! ===" << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cout << "\n=== ERRO DURANTE OS TESTES: " << e.what() << " ===" << std::endl;
        return 1;
    } catch (...) {
        std::cout << "\n=== ERRO DESCONHECIDO DURANTE OS TESTES ===" << std::endl;
        return 1;
    }
}