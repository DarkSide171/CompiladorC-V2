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
// FASE 5.2 - TESTES DE COMPONENTES
// ============================================================================

// Teste de transições de estado
void testStateTransitions() {
    std::cout << "\n=== Testando Transições de Estado (Fase 5.2) ===" << std::endl;
    
    StateMachine sm;
    
    // Teste transições básicas START -> outros estados
    assertEqual(LexerState::IDENTIFIER, sm.transition('a'), "START + 'a' -> IDENTIFIER");
    sm.reset();
    assertEqual(LexerState::INTEGER, sm.transition('1'), "START + '1' -> INTEGER");
    sm.reset();
    assertEqual(LexerState::PLUS, sm.transition('+'), "START + '+' -> PLUS");
    sm.reset();
    assertEqual(LexerState::STRING_START, sm.transition('"'), "START + '\"' -> STRING_START");
    sm.reset();
    assertEqual(LexerState::CHAR_START, sm.transition('\''), "START + '\'' -> CHAR_START");
    sm.reset();
    assertEqual(LexerState::DIVIDE, sm.transition('/'), "START + '/' -> DIVIDE");
    sm.reset();
    
    // Teste transições de identificadores
    sm.transition('a'); // IDENTIFIER
    assertEqual(LexerState::IDENTIFIER, sm.transition('b'), "IDENTIFIER + 'b' -> IDENTIFIER");
    assertEqual(LexerState::IDENTIFIER, sm.transition('1'), "IDENTIFIER + '1' -> IDENTIFIER");
    assertEqual(LexerState::IDENTIFIER, sm.transition('_'), "IDENTIFIER + '_' -> IDENTIFIER");
    assertEqual(LexerState::ACCEPT_IDENTIFIER, sm.transition(' '), "IDENTIFIER + ' ' -> ACCEPT_IDENTIFIER");
    sm.reset();
    
    // Teste transições de números inteiros
    sm.transition('1'); // INTEGER
    assertEqual(LexerState::INTEGER, sm.transition('2'), "INTEGER + '2' -> INTEGER");
    assertEqual(LexerState::FLOAT_DOT, sm.transition('.'), "INTEGER + '.' -> FLOAT_DOT");
    sm.reset();
    sm.transition('1'); // INTEGER
    assertEqual(LexerState::ACCEPT_INTEGER, sm.transition(' '), "INTEGER + ' ' -> ACCEPT_INTEGER");
    sm.reset();
    
    // Teste transições de números float
    sm.transition('1'); // INTEGER
    sm.transition('.'); // FLOAT_DOT
    assertEqual(LexerState::FLOAT_DIGITS, sm.transition('5'), "FLOAT_DOT + '5' -> FLOAT_DIGITS");
    assertEqual(LexerState::ACCEPT_FLOAT, sm.transition(' '), "FLOAT_DIGITS + ' ' -> ACCEPT_FLOAT");
    sm.reset();
    
    // Teste transições de operadores compostos
    sm.transition('+'); // PLUS
    assertEqual(LexerState::INCREMENT, sm.transition('+'), "PLUS + '+' -> INCREMENT");
    assertEqual(LexerState::ACCEPT_OPERATOR, sm.transition(' '), "INCREMENT + ' ' -> ACCEPT_OPERATOR");
    sm.reset();
    
    sm.transition('-'); // MINUS
    assertEqual(LexerState::DECREMENT, sm.transition('-'), "MINUS + '-' -> DECREMENT");
    assertEqual(LexerState::ACCEPT_OPERATOR, sm.transition(' '), "DECREMENT + ' ' -> ACCEPT_OPERATOR");
    sm.reset();
    
    sm.transition('-'); // MINUS
    assertEqual(LexerState::ARROW, sm.transition('>'), "MINUS + '>' -> ARROW");
    assertEqual(LexerState::ACCEPT_OPERATOR, sm.transition(' '), "ARROW + ' ' -> ACCEPT_OPERATOR");
    sm.reset();
    
    // Teste transições de comentários
    sm.transition('/'); // DIVIDE
    assertEqual(LexerState::LINE_COMMENT, sm.transition('/'), "DIVIDE + '/' -> LINE_COMMENT");
    assertEqual(LexerState::LINE_COMMENT, sm.transition('t'), "LINE_COMMENT + 't' -> LINE_COMMENT");
    assertEqual(LexerState::ACCEPT_COMMENT, sm.transition('\n'), "LINE_COMMENT + '\\n' -> ACCEPT_COMMENT");
    sm.reset();
    
    sm.transition('/'); // DIVIDE
    assertEqual(LexerState::BLOCK_COMMENT, sm.transition('*'), "DIVIDE + '*' -> BLOCK_COMMENT");
    assertEqual(LexerState::BLOCK_COMMENT, sm.transition('t'), "BLOCK_COMMENT + 't' -> BLOCK_COMMENT");
    assertEqual(LexerState::BLOCK_COMMENT_END, sm.transition('*'), "BLOCK_COMMENT + '*' -> BLOCK_COMMENT_END");
    assertEqual(LexerState::ACCEPT_COMMENT, sm.transition('/'), "BLOCK_COMMENT_END + '/' -> ACCEPT_COMMENT");
    sm.reset();
    
    // Teste transições de strings
    sm.transition('"'); // STRING_START
    assertEqual(LexerState::STRING_BODY, sm.transition('h'), "STRING_START + 'h' -> STRING_BODY");
    assertEqual(LexerState::STRING_BODY, sm.transition('i'), "STRING_BODY + 'i' -> STRING_BODY");
    assertEqual(LexerState::STRING_END, sm.transition('"'), "STRING_BODY + '\"' -> STRING_END");
    assertEqual(LexerState::ACCEPT_STRING, sm.transition(' '), "STRING_END + ' ' -> ACCEPT_STRING");
    sm.reset();
    
    // Teste transições de caracteres
    sm.transition('\''); // CHAR_START
    assertEqual(LexerState::CHAR_BODY, sm.transition('a'), "CHAR_START + 'a' -> CHAR_BODY");
    assertEqual(LexerState::CHAR_END, sm.transition('\''), "CHAR_BODY + '\'' -> CHAR_END");
    assertEqual(LexerState::ACCEPT_CHAR, sm.transition(' '), "CHAR_END + ' ' -> ACCEPT_CHAR");
    sm.reset();
    
    // Teste transições para estados de erro
    assertEqual(LexerState::ERROR, sm.transition('\x01'), "START + caractere inválido -> ERROR");
    sm.reset();
    
    printTestResult("Transições de Estado (Fase 5.2)", true);
}

// Teste de estados de aceitação
void testAcceptingStates() {
    std::cout << "\n=== Testando Estados de Aceitação (Fase 5.2) ===" << std::endl;
    
    StateMachine sm;
    
    // Teste estado inicial não é de aceitação
    assertTrue(!sm.isAcceptingState(), "Estado START não é de aceitação");
    
    // Teste estados intermediários não são de aceitação
    sm.transition('a'); // IDENTIFIER
    assertTrue(!sm.isAcceptingState(), "Estado IDENTIFIER não é de aceitação");
    sm.reset();
    
    sm.transition('1'); // INTEGER
    assertTrue(!sm.isAcceptingState(), "Estado INTEGER não é de aceitação");
    sm.reset();
    
    sm.transition('+'); // PLUS
    assertTrue(!sm.isAcceptingState(), "Estado PLUS não é de aceitação");
    sm.reset();
    
    sm.transition('"'); // STRING_START
    assertTrue(!sm.isAcceptingState(), "Estado STRING_START não é de aceitação");
    sm.reset();
    
    // Teste estados de aceitação para identificadores
    sm.transition('a'); // IDENTIFIER
    sm.transition(' '); // ACCEPT_IDENTIFIER
    assertTrue(sm.isAcceptingState(), "Estado ACCEPT_IDENTIFIER é de aceitação");
    assertEqual(TokenType::IDENTIFIER, sm.getTokenType(), "Token type correto para ACCEPT_IDENTIFIER");
    sm.reset();
    
    // Teste estados de aceitação para números inteiros
    sm.transition('1'); // INTEGER
    sm.transition(' '); // ACCEPT_INTEGER
    assertTrue(sm.isAcceptingState(), "Estado ACCEPT_INTEGER é de aceitação");
    assertEqual(TokenType::INTEGER_LITERAL, sm.getTokenType(), "Token type correto para ACCEPT_INTEGER");
    sm.reset();
    
    // Teste estados de aceitação para números float
    sm.transition('1'); // INTEGER
    sm.transition('.'); // FLOAT_DOT
    sm.transition('5'); // FLOAT_DIGITS
    sm.transition(' '); // ACCEPT_FLOAT
    assertTrue(sm.isAcceptingState(), "Estado ACCEPT_FLOAT é de aceitação");
    assertEqual(TokenType::FLOAT_LITERAL, sm.getTokenType(), "Token type correto para ACCEPT_FLOAT");
    sm.reset();
    
    // Teste estados de aceitação para strings
    sm.transition('"'); // STRING_START
    sm.transition('h'); // STRING_BODY
    sm.transition('"'); // STRING_END
    sm.transition(' '); // ACCEPT_STRING
    assertTrue(sm.isAcceptingState(), "Estado ACCEPT_STRING é de aceitação");
    assertEqual(TokenType::STRING_LITERAL, sm.getTokenType(), "Token type correto para ACCEPT_STRING");
    sm.reset();
    
    // Teste estados de aceitação para caracteres
    sm.transition('\''); // CHAR_START
    sm.transition('a'); // CHAR_BODY
    sm.transition('\''); // CHAR_END
    sm.transition(' '); // ACCEPT_CHAR
    assertTrue(sm.isAcceptingState(), "Estado ACCEPT_CHAR é de aceitação");
    assertEqual(TokenType::CHAR_LITERAL, sm.getTokenType(), "Token type correto para ACCEPT_CHAR");
    sm.reset();
    
    // Teste estados de aceitação para operadores
    sm.transition('+'); // PLUS
    sm.transition(' '); // ACCEPT_OPERATOR
    assertTrue(sm.isAcceptingState(), "Estado ACCEPT_OPERATOR é de aceitação");
    // Agora ACCEPT_OPERATOR pode determinar o tipo específico baseado no último caractere
    assertTrue(sm.getTokenType() == TokenType::PLUS, "Token type para ACCEPT_OPERATOR é PLUS");
    sm.reset();
    
    // Teste estados de aceitação para delimitadores
    sm.transition('('); // LEFT_PAREN
    sm.transition(' '); // ACCEPT_DELIMITER
    assertTrue(sm.isAcceptingState(), "Estado ACCEPT_DELIMITER é de aceitação");
    assertEqual(TokenType::LEFT_PAREN, sm.getTokenType(), "Token type correto para ACCEPT_DELIMITER");
    sm.reset();
    
    // Teste estados de aceitação para comentários
    sm.transition('/'); // DIVIDE
    sm.transition('/'); // LINE_COMMENT
    sm.transition('\n'); // ACCEPT_COMMENT
    assertTrue(sm.isAcceptingState(), "Estado ACCEPT_COMMENT é de aceitação");
    assertEqual(TokenType::LINE_COMMENT, sm.getTokenType(), "Token type correto para ACCEPT_COMMENT");
    sm.reset();
    
    // Teste estado de aceitação para EOF
    sm.transition('\0'); // END_OF_FILE
    sm.transition(' '); // ACCEPT_EOF
    assertTrue(sm.isAcceptingState(), "Estado ACCEPT_EOF é de aceitação");
    assertEqual(TokenType::END_OF_FILE, sm.getTokenType(), "Token type correto para ACCEPT_EOF");
    sm.reset();
    
    // Teste método getAcceptingStates
    auto accepting_states = sm.getAcceptingStates();
    assertTrue(accepting_states.size() > 0, "getAcceptingStates retorna estados");
    
    // Verificar se todos os estados de aceitação estão na lista
    bool found_accept_identifier = false;
    bool found_accept_integer = false;
    bool found_accept_float = false;
    bool found_accept_string = false;
    bool found_accept_char = false;
    bool found_accept_operator = false;
    bool found_accept_delimiter = false;
    bool found_accept_comment = false;
    bool found_accept_eof = false;
    
    for (const auto& state : accepting_states) {
        if (state == LexerState::ACCEPT_IDENTIFIER) found_accept_identifier = true;
        if (state == LexerState::ACCEPT_INTEGER) found_accept_integer = true;
        if (state == LexerState::ACCEPT_FLOAT) found_accept_float = true;
        if (state == LexerState::ACCEPT_STRING) found_accept_string = true;
        if (state == LexerState::ACCEPT_CHAR) found_accept_char = true;
        if (state == LexerState::ACCEPT_OPERATOR) found_accept_operator = true;
        if (state == LexerState::ACCEPT_DELIMITER) found_accept_delimiter = true;
        if (state == LexerState::ACCEPT_COMMENT) found_accept_comment = true;
        if (state == LexerState::ACCEPT_EOF) found_accept_eof = true;
    }
    
    assertTrue(found_accept_identifier, "getAcceptingStates contém ACCEPT_IDENTIFIER");
    assertTrue(found_accept_integer, "getAcceptingStates contém ACCEPT_INTEGER");
    assertTrue(found_accept_float, "getAcceptingStates contém ACCEPT_FLOAT");
    assertTrue(found_accept_string, "getAcceptingStates contém ACCEPT_STRING");
    assertTrue(found_accept_char, "getAcceptingStates contém ACCEPT_CHAR");
    assertTrue(found_accept_operator, "getAcceptingStates contém ACCEPT_OPERATOR");
    assertTrue(found_accept_delimiter, "getAcceptingStates contém ACCEPT_DELIMITER");
    assertTrue(found_accept_comment, "getAcceptingStates contém ACCEPT_COMMENT");
    assertTrue(found_accept_eof, "getAcceptingStates contém ACCEPT_EOF");
    
    printTestResult("Estados de Aceitação (Fase 5.2)", true);
}

// Teste de estados de erro
void testErrorStates() {
    std::cout << "\n=== Testando Estados de Erro (Fase 5.2) ===" << std::endl;
    
    StateMachine sm;
    
    // Teste estado inicial não é de erro
    assertTrue(!sm.isErrorState(), "Estado START não é de erro");
    
    // Teste estados intermediários não são de erro
    sm.transition('a'); // IDENTIFIER
    assertTrue(!sm.isErrorState(), "Estado IDENTIFIER não é de erro");
    sm.reset();
    
    sm.transition('1'); // INTEGER
    assertTrue(!sm.isErrorState(), "Estado INTEGER não é de erro");
    sm.reset();
    
    sm.transition('+'); // PLUS
    assertTrue(!sm.isErrorState(), "Estado PLUS não é de erro");
    sm.reset();
    
    // Teste estados de aceitação não são de erro
    sm.transition('a'); // IDENTIFIER
    sm.transition(' '); // ACCEPT_IDENTIFIER
    assertTrue(!sm.isErrorState(), "Estado ACCEPT_IDENTIFIER não é de erro");
    sm.reset();
    
    // Teste transições que levam ao estado de erro
    assertEqual(LexerState::ERROR, sm.transition('\x01'), "Caractere inválido -> ERROR");
    assertTrue(sm.isErrorState(), "Estado ERROR é de erro");
    sm.reset();
    
    assertEqual(LexerState::ERROR, sm.transition('\x02'), "Outro caractere inválido -> ERROR");
    assertTrue(sm.isErrorState(), "Estado ERROR é de erro");
    sm.reset();
    
    assertEqual(LexerState::ERROR, sm.transition('\x7F'), "Caractere de controle -> ERROR");
    assertTrue(sm.isErrorState(), "Estado ERROR é de erro");
    sm.reset();
    
    // Teste transições inválidas de estados específicos
    sm.transition('1'); // INTEGER
    assertEqual(LexerState::ERROR, sm.transition('x'), "INTEGER + 'x' -> ERROR (exceto hex)");
    assertTrue(sm.isErrorState(), "Estado ERROR é de erro");
    sm.reset();
    
    // Teste string não terminada
    sm.transition('"'); // STRING_START
    sm.transition('h'); // STRING_BODY
    sm.transition('i'); // STRING_BODY
    assertEqual(LexerState::ERROR, sm.transition('\0'), "String não terminada -> ERROR");
    assertTrue(sm.isErrorState(), "Estado ERROR é de erro");
    sm.reset();
    
    // Teste caractere não terminado
    sm.transition('\''); // CHAR_START
    sm.transition('a'); // CHAR_BODY
    assertEqual(LexerState::ERROR, sm.transition('\0'), "Char não terminado -> ERROR");
    assertTrue(sm.isErrorState(), "Estado ERROR é de erro");
    sm.reset();
    
    // Teste comentário de bloco não terminado
    sm.transition('/'); // DIVIDE
    sm.transition('*'); // BLOCK_COMMENT
    sm.transition('t'); // BLOCK_COMMENT
    sm.transition('e'); // BLOCK_COMMENT
    sm.transition('s'); // BLOCK_COMMENT
    sm.transition('t'); // BLOCK_COMMENT
    assertEqual(LexerState::ERROR, sm.transition('\0'), "Comentário de bloco não terminado -> ERROR");
    assertTrue(sm.isErrorState(), "Estado ERROR é de erro");
    sm.reset();
    
    // Teste método getErrorStates
    auto error_states = sm.getErrorStates();
    assertTrue(error_states.size() >= 1, "getErrorStates retorna pelo menos 1 estado");
    
    bool found_error = false;
    for (const auto& state : error_states) {
        if (state == LexerState::ERROR) {
            found_error = true;
            break;
        }
    }
    assertTrue(found_error, "getErrorStates contém ERROR");
    
    // Teste isErrorState com diferentes estados
    assertTrue(sm.isErrorState(LexerState::ERROR), "isErrorState(ERROR) retorna true");
    assertTrue(!sm.isErrorState(LexerState::START), "isErrorState(START) retorna false");
    assertTrue(!sm.isErrorState(LexerState::IDENTIFIER), "isErrorState(IDENTIFIER) retorna false");
    assertTrue(!sm.isErrorState(LexerState::ACCEPT_IDENTIFIER), "isErrorState(ACCEPT_IDENTIFIER) retorna false");
    
    // Teste recuperação de erro (reset)
    sm.transition('\x01'); // ERROR
    assertTrue(sm.isErrorState(), "Estado ERROR após caractere inválido");
    sm.reset();
    assertTrue(!sm.isErrorState(), "Estado não é de erro após reset");
    assertEqual(LexerState::START, sm.getCurrentState(), "Estado volta para START após reset");
    
    // Teste múltiplos erros consecutivos
    sm.transition('\x01'); // ERROR
    assertTrue(sm.isErrorState(), "Primeiro erro");
    assertEqual(LexerState::ERROR, sm.transition('\x02'), "Segundo erro consecutivo");
    assertTrue(sm.isErrorState(), "Ainda em estado de erro");
    sm.reset();
    
    // Teste transições válidas após reset
    sm.transition('a'); // IDENTIFIER
    assertTrue(!sm.isErrorState(), "Transição válida após reset");
    assertEqual(LexerState::IDENTIFIER, sm.getCurrentState(), "Estado correto após reset");
    
    printTestResult("Estados de Erro (Fase 5.2)", true);
}

// Teste de reset da máquina de estados
void testStateMachineReset() {
    std::cout << "\n=== Testando Reset da Máquina de Estados (Fase 5.2) ===" << std::endl;
    
    StateMachine sm;
    
    // Teste estado inicial
    assertEqual(LexerState::START, sm.getCurrentState(), "Estado inicial é START");
    assertTrue(!sm.isAcceptingState(), "Estado inicial não é de aceitação");
    assertTrue(!sm.isErrorState(), "Estado inicial não é de erro");
    
    // Teste reset do estado START (não deve mudar)
    sm.reset();
    assertEqual(LexerState::START, sm.getCurrentState(), "Reset de START mantém START");
    
    // Teste reset de estado intermediário (IDENTIFIER)
    sm.transition('a'); // IDENTIFIER
    assertEqual(LexerState::IDENTIFIER, sm.getCurrentState(), "Estado mudou para IDENTIFIER");
    sm.reset();
    assertEqual(LexerState::START, sm.getCurrentState(), "Reset de IDENTIFIER volta para START");
    
    // Teste reset de estado de aceitação
    sm.transition('a'); // IDENTIFIER
    sm.transition(' '); // ACCEPT_IDENTIFIER
    assertEqual(LexerState::ACCEPT_IDENTIFIER, sm.getCurrentState(), "Estado mudou para ACCEPT_IDENTIFIER");
    assertTrue(sm.isAcceptingState(), "Estado é de aceitação");
    sm.reset();
    assertEqual(LexerState::START, sm.getCurrentState(), "Reset de ACCEPT_IDENTIFIER volta para START");
    assertTrue(!sm.isAcceptingState(), "Estado não é mais de aceitação após reset");
    
    // Teste reset de estado de erro
    sm.transition('\x01'); // ERROR
    assertEqual(LexerState::ERROR, sm.getCurrentState(), "Estado mudou para ERROR");
    assertTrue(sm.isErrorState(), "Estado é de erro");
    sm.reset();
    assertEqual(LexerState::START, sm.getCurrentState(), "Reset de ERROR volta para START");
    assertTrue(!sm.isErrorState(), "Estado não é mais de erro após reset");
    
    // Teste reset após sequência complexa de transições
    sm.transition('1'); // INTEGER
    sm.transition('2'); // INTEGER
    sm.transition('.'); // FLOAT_DOT
    sm.transition('5'); // FLOAT_DIGITS
    assertEqual(LexerState::FLOAT_DIGITS, sm.getCurrentState(), "Estado após sequência complexa");
    sm.reset();
    assertEqual(LexerState::START, sm.getCurrentState(), "Reset após sequência complexa volta para START");
    
    // Teste reset após operadores compostos
    sm.transition('+'); // PLUS
    sm.transition('+'); // INCREMENT
    assertEqual(LexerState::INCREMENT, sm.getCurrentState(), "Estado mudou para INCREMENT");
    sm.reset();
    assertEqual(LexerState::START, sm.getCurrentState(), "Reset de INCREMENT volta para START");
    
    // Teste reset após comentários
    sm.transition('/'); // DIVIDE
    sm.transition('/'); // LINE_COMMENT
    sm.transition('t'); // LINE_COMMENT
    assertEqual(LexerState::LINE_COMMENT, sm.getCurrentState(), "Estado mudou para LINE_COMMENT");
    sm.reset();
    assertEqual(LexerState::START, sm.getCurrentState(), "Reset de LINE_COMMENT volta para START");
    
    // Teste reset após strings
    sm.transition('"'); // STRING_START
    sm.transition('h'); // STRING_BODY
    sm.transition('i'); // STRING_BODY
    assertEqual(LexerState::STRING_BODY, sm.getCurrentState(), "Estado mudou para STRING_BODY");
    sm.reset();
    assertEqual(LexerState::START, sm.getCurrentState(), "Reset de STRING_BODY volta para START");
    
    // Teste reset após caracteres
    sm.transition('\''); // CHAR_START
    sm.transition('a'); // CHAR_BODY
    assertEqual(LexerState::CHAR_BODY, sm.getCurrentState(), "Estado mudou para CHAR_BODY");
    sm.reset();
    assertEqual(LexerState::START, sm.getCurrentState(), "Reset de CHAR_BODY volta para START");
    
    // Teste múltiplos resets consecutivos
    sm.transition('a'); // IDENTIFIER
    sm.reset();
    sm.reset();
    sm.reset();
    assertEqual(LexerState::START, sm.getCurrentState(), "Múltiplos resets mantêm START");
    
    // Teste funcionalidade após reset
    sm.transition('a'); // IDENTIFIER
    sm.reset();
    sm.transition('1'); // INTEGER
    assertEqual(LexerState::INTEGER, sm.getCurrentState(), "Funcionalidade normal após reset");
    sm.transition(' '); // ACCEPT_INTEGER
    assertTrue(sm.isAcceptingState(), "Estado de aceitação funciona após reset");
    
    // Teste reset preserva configurações da máquina
    auto error_handler = std::make_shared<ErrorHandler>();
    sm.setErrorHandler(error_handler);
    sm.transition('a'); // IDENTIFIER
    sm.reset();
    assertTrue(sm.getErrorHandler() == error_handler, "Reset preserva error handler");
    
    // Teste reset não afeta métodos utilitários
    sm.transition('a'); // IDENTIFIER
    sm.reset();
    auto accepting_states = sm.getAcceptingStates();
    assertTrue(accepting_states.size() > 0, "getAcceptingStates funciona após reset");
    
    auto error_states = sm.getErrorStates();
    assertTrue(error_states.size() > 0, "getErrorStates funciona após reset");
    
    // Teste isValidTransition após reset
    sm.transition('a'); // IDENTIFIER
    sm.reset();
    assertTrue(sm.isValidTransition('a'), "isValidTransition funciona após reset");
    assertTrue(sm.isValidTransition('1'), "isValidTransition funciona após reset");
    assertTrue(!sm.isValidTransition('\x01'), "isValidTransition detecta inválidas após reset");
    
    printTestResult("Reset da Máquina de Estados (Fase 5.2)", true);
}

// Teste da tabela de transições
void testTransitionTable() {
    std::cout << "\n=== Testando Tabela de Transições (Fase 5.2) ===" << std::endl;
    
    StateMachine sm;
    
    // Teste transições básicas do estado START
    assertTrue(sm.isValidTransition('a'), "Transição START -> 'a' é válida");
    assertTrue(sm.isValidTransition('z'), "Transição START -> 'z' é válida");
    assertTrue(sm.isValidTransition('A'), "Transição START -> 'A' é válida");
    assertTrue(sm.isValidTransition('Z'), "Transição START -> 'Z' é válida");
    assertTrue(sm.isValidTransition('_'), "Transição START -> '_' é válida");
    assertTrue(sm.isValidTransition('0'), "Transição START -> '0' é válida");
    assertTrue(sm.isValidTransition('9'), "Transição START -> '9' é válida");
    assertTrue(sm.isValidTransition('+'), "Transição START -> '+' é válida");
    assertTrue(sm.isValidTransition('-'), "Transição START -> '-' é válida");
    assertTrue(sm.isValidTransition('*'), "Transição START -> '*' é válida");
    assertTrue(sm.isValidTransition('/'), "Transição START -> '/' é válida");
    assertTrue(sm.isValidTransition('='), "Transição START -> '=' é válida");
    assertTrue(sm.isValidTransition('<'), "Transição START -> '<' é válida");
    assertTrue(sm.isValidTransition('>'), "Transição START -> '>' é válida");
    assertTrue(sm.isValidTransition('!'), "Transição START -> '!' é válida");
    assertTrue(sm.isValidTransition('&'), "Transição START -> '&' é válida");
    assertTrue(sm.isValidTransition('|'), "Transição START -> '|' é válida");
    assertTrue(sm.isValidTransition('('), "Transição START -> '(' é válida");
    assertTrue(sm.isValidTransition(')'), "Transição START -> ')' é válida");
    assertTrue(sm.isValidTransition('{'), "Transição START -> '{' é válida");
    assertTrue(sm.isValidTransition('}'), "Transição START -> '}' é válida");
    assertTrue(sm.isValidTransition('['), "Transição START -> '[' é válida");
    assertTrue(sm.isValidTransition(']'), "Transição START -> ']' é válida");
    assertTrue(sm.isValidTransition(';'), "Transição START -> ';' é válida");
    assertTrue(sm.isValidTransition(','), "Transição START -> ',' é válida");
    assertTrue(sm.isValidTransition('.'), "Transição START -> '.' é válida");
    assertTrue(sm.isValidTransition('"'), "Transição START -> '\"' é válida");
    assertTrue(sm.isValidTransition('\''), "Transição START -> '\'' é válida");
    assertTrue(sm.isValidTransition(' '), "Transição START -> ' ' é válida");
    assertTrue(sm.isValidTransition('\t'), "Transição START -> '\t' é válida");
    assertTrue(sm.isValidTransition('\n'), "Transição START -> '\n' é válida");
    assertTrue(sm.isValidTransition('\r'), "Transição START -> '\r' é válida");
    
    // Teste transições inválidas do estado START
    assertTrue(!sm.isValidTransition('\x01'), "Transição START -> '\x01' é inválida");
    assertTrue(!sm.isValidTransition('\x02'), "Transição START -> '\x02' é inválida");
    assertTrue(!sm.isValidTransition('\x7F'), "Transição START -> '\x7F' é inválida");
    
    // Teste transições do estado IDENTIFIER
    sm.transition('a'); // IDENTIFIER
    assertEqual(LexerState::IDENTIFIER, sm.getCurrentState(), "Estado mudou para IDENTIFIER");
    assertTrue(sm.isValidTransition('a'), "Transição IDENTIFIER -> 'a' é válida");
    assertTrue(sm.isValidTransition('z'), "Transição IDENTIFIER -> 'z' é válida");
    assertTrue(sm.isValidTransition('A'), "Transição IDENTIFIER -> 'A' é válida");
    assertTrue(sm.isValidTransition('Z'), "Transição IDENTIFIER -> 'Z' é válida");
    assertTrue(sm.isValidTransition('_'), "Transição IDENTIFIER -> '_' é válida");
    assertTrue(sm.isValidTransition('0'), "Transição IDENTIFIER -> '0' é válida");
    assertTrue(sm.isValidTransition('9'), "Transição IDENTIFIER -> '9' é válida");
    assertTrue(sm.isValidTransition(' '), "Transição IDENTIFIER -> ' ' é válida (aceitação)");
    assertTrue(!sm.isValidTransition('+'), "Transição IDENTIFIER -> '+' é inválida");
    assertTrue(!sm.isValidTransition('"'), "Transição IDENTIFIER -> '\"' é inválida");
    
    // Teste transições do estado INTEGER
    sm.reset();
    sm.transition('1'); // INTEGER
    assertEqual(LexerState::INTEGER, sm.getCurrentState(), "Estado mudou para INTEGER");
    assertTrue(sm.isValidTransition('0'), "Transição INTEGER -> '0' é válida");
    assertTrue(sm.isValidTransition('9'), "Transição INTEGER -> '9' é válida");
    assertTrue(sm.isValidTransition('.'), "Transição INTEGER -> '.' é válida (float)");
    assertTrue(sm.isValidTransition(' '), "Transição INTEGER -> ' ' é válida (aceitação)");
    assertTrue(!sm.isValidTransition('a'), "Transição INTEGER -> 'a' é inválida");
    assertTrue(!sm.isValidTransition('+'), "Transição INTEGER -> '+' é inválida");
    
    // Teste transições do estado FLOAT_DOT
    sm.transition('.'); // FLOAT_DOT
    assertEqual(LexerState::FLOAT_DOT, sm.getCurrentState(), "Estado mudou para FLOAT_DOT");
    assertTrue(sm.isValidTransition('0'), "Transição FLOAT_DOT -> '0' é válida");
    assertTrue(sm.isValidTransition('9'), "Transição FLOAT_DOT -> '9' é válida");
    assertTrue(!sm.isValidTransition('.'), "Transição FLOAT_DOT -> '.' é inválida");
    assertTrue(!sm.isValidTransition('a'), "Transição FLOAT_DOT -> 'a' é inválida");
    assertTrue(!sm.isValidTransition(' '), "Transição FLOAT_DOT -> ' ' é inválida (não aceitação)");
    
    // Teste transições do estado FLOAT_DIGITS
    sm.transition('5'); // FLOAT_DIGITS
    assertEqual(LexerState::FLOAT_DIGITS, sm.getCurrentState(), "Estado mudou para FLOAT_DIGITS");
    assertTrue(sm.isValidTransition('0'), "Transição FLOAT_DIGITS -> '0' é válida");
    assertTrue(sm.isValidTransition('9'), "Transição FLOAT_DIGITS -> '9' é válida");
    assertTrue(sm.isValidTransition(' '), "Transição FLOAT_DIGITS -> ' ' é válida (aceitação)");
    assertTrue(!sm.isValidTransition('.'), "Transição FLOAT_DIGITS -> '.' é inválida");
    assertTrue(!sm.isValidTransition('a'), "Transição FLOAT_DIGITS -> 'a' é inválida");
    
    // Teste transições de operadores compostos
    sm.reset();
    sm.transition('+'); // PLUS
    assertEqual(LexerState::PLUS, sm.getCurrentState(), "Estado mudou para PLUS");
    assertTrue(sm.isValidTransition('+'), "Transição PLUS -> '+' é válida (increment)");
    assertTrue(sm.isValidTransition('='), "Transição PLUS -> '=' é válida (plus_assign)");
    assertTrue(sm.isValidTransition(' '), "Transição PLUS -> ' ' é válida (aceitação)");
    assertTrue(!sm.isValidTransition('a'), "Transição PLUS -> 'a' é inválida");
    
    sm.transition('+'); // INCREMENT
    assertEqual(LexerState::INCREMENT, sm.getCurrentState(), "Estado mudou para INCREMENT");
    assertTrue(sm.isValidTransition(' '), "Transição INCREMENT -> ' ' é válida (aceitação)");
    assertTrue(!sm.isValidTransition('+'), "Transição INCREMENT -> '+' é inválida");
    
    // Teste transições de comentários
    sm.reset();
    sm.transition('/'); // DIVIDE
    assertEqual(LexerState::DIVIDE, sm.getCurrentState(), "Estado mudou para DIVIDE");
    assertTrue(sm.isValidTransition('/'), "Transição DIVIDE -> '/' é válida (line comment)");
    assertTrue(sm.isValidTransition('*'), "Transição DIVIDE -> '*' é válida (block comment)");
    assertTrue(sm.isValidTransition('='), "Transição DIVIDE -> '=' é válida (divide_assign)");
    assertTrue(sm.isValidTransition(' '), "Transição DIVIDE -> ' ' é válida (aceitação)");
    
    sm.transition('/'); // LINE_COMMENT
    assertEqual(LexerState::LINE_COMMENT, sm.getCurrentState(), "Estado mudou para LINE_COMMENT");
    assertTrue(sm.isValidTransition('a'), "Transição LINE_COMMENT -> 'a' é válida");
    assertTrue(sm.isValidTransition(' '), "Transição LINE_COMMENT -> ' ' é válida");
    assertTrue(sm.isValidTransition('\n'), "Transição LINE_COMMENT -> '\n' é válida (fim)");
    
    // Teste transições de strings
    sm.reset();
    sm.transition('"'); // STRING_START
    assertEqual(LexerState::STRING_START, sm.getCurrentState(), "Estado mudou para STRING_START");
    assertTrue(sm.isValidTransition('a'), "Transição STRING_START -> 'a' é válida");
    assertTrue(sm.isValidTransition(' '), "Transição STRING_START -> ' ' é válida");
    assertTrue(sm.isValidTransition('\\'), "Transição STRING_START -> '\\' é válida (escape)");
    assertTrue(sm.isValidTransition('"'), "Transição STRING_START -> '\"' é válida (fim)");
    assertTrue(!sm.isValidTransition('\n'), "Transição STRING_START -> '\n' é inválida");
    
    // Teste transições de caracteres
    sm.reset();
    sm.transition('\''); // CHAR_START
    assertEqual(LexerState::CHAR_START, sm.getCurrentState(), "Estado mudou para CHAR_START");
    assertTrue(sm.isValidTransition('a'), "Transição CHAR_START -> 'a' é válida");
    assertTrue(sm.isValidTransition('\\'), "Transição CHAR_START -> '\\' é válida (escape)");
    assertTrue(!sm.isValidTransition('\''), "Transição CHAR_START -> '\'' é inválida (vazio)");
    assertTrue(!sm.isValidTransition('\n'), "Transição CHAR_START -> '\n' é inválida");
    
    // Teste transições de delimitadores
    sm.reset();
    sm.transition('('); // LEFT_PAREN
    assertEqual(LexerState::LEFT_PAREN, sm.getCurrentState(), "Estado mudou para LEFT_PAREN");
    assertTrue(sm.isValidTransition(' '), "Transição LEFT_PAREN -> ' ' é válida (aceitação)");
    assertTrue(!sm.isValidTransition('('), "Transição LEFT_PAREN -> '(' é inválida");
    
    // Teste transições de whitespace
    sm.reset();
    sm.transition(' '); // WHITESPACE
    assertEqual(LexerState::WHITESPACE, sm.getCurrentState(), "Estado mudou para WHITESPACE");
    assertTrue(sm.isValidTransition(' '), "Transição WHITESPACE -> ' ' é válida");
    assertTrue(sm.isValidTransition('\t'), "Transição WHITESPACE -> '\t' é válida");
    assertTrue(sm.isValidTransition('\n'), "Transição WHITESPACE -> '\n' é válida");
    assertTrue(sm.isValidTransition('\r'), "Transição WHITESPACE -> '\r' é válida");
    assertTrue(!sm.isValidTransition('a'), "Transição WHITESPACE -> 'a' é inválida");
    
    // Teste transições do estado ERROR
    sm.reset();
    sm.transition('\x01'); // ERROR
    assertEqual(LexerState::ERROR, sm.getCurrentState(), "Estado mudou para ERROR");
    assertTrue(!sm.isValidTransition('a'), "Transição ERROR -> 'a' é inválida");
    assertTrue(!sm.isValidTransition(' '), "Transição ERROR -> ' ' é inválida");
    assertTrue(!sm.isValidTransition('\x01'), "Transição ERROR -> '\x01' é inválida");
    
    // Teste consistência das transições através de verificação direta
     sm.reset();
     
     // Verificar que transições válidas levam a estados esperados
     sm.transition('a');
     assertEqual(LexerState::IDENTIFIER, sm.getCurrentState(), "'a' leva a IDENTIFIER");
     
     sm.reset();
     sm.transition('1');
     assertEqual(LexerState::INTEGER, sm.getCurrentState(), "'1' leva a INTEGER");
     
     sm.reset();
     sm.transition('+');
     assertEqual(LexerState::PLUS, sm.getCurrentState(), "'+' leva a PLUS");
     
     sm.reset();
     sm.transition('/');
     assertEqual(LexerState::DIVIDE, sm.getCurrentState(), "'/' leva a DIVIDE");
     
     sm.reset();
     sm.transition('"');
     assertEqual(LexerState::STRING_START, sm.getCurrentState(), "'\"' leva a STRING_START");
     
     sm.reset();
     sm.transition('\'');
     assertEqual(LexerState::CHAR_START, sm.getCurrentState(), "'\'' leva a CHAR_START");
     
     sm.reset();
     sm.transition('(');
     assertEqual(LexerState::LEFT_PAREN, sm.getCurrentState(), "'(' leva a LEFT_PAREN");
     
     sm.reset();
     sm.transition(' ');
     assertEqual(LexerState::WHITESPACE, sm.getCurrentState(), "' ' leva a WHITESPACE");
     
     // Verificar transições inválidas levam a ERROR
     sm.reset();
     sm.transition('\x01');
     assertEqual(LexerState::ERROR, sm.getCurrentState(), "Caractere inválido leva a ERROR");
    
    printTestResult("Tabela de Transições (Fase 5.2)", true);
}

// Teste de validação de estados
void testStateValidation() {
    std::cout << "\n=== Testando Validação de Estados (Fase 5.2) ===" << std::endl;
    
    StateMachine sm;
    
    // Teste validação do estado inicial
    assertEqual(LexerState::START, sm.getCurrentState(), "Estado atual é START");
    assertTrue(!sm.isAcceptingState(), "Estado START não é de aceitação");
    assertTrue(!sm.isErrorState(), "Estado START não é de erro");
    
    // Teste validação de estados de identificadores
    sm.transition('a'); // IDENTIFIER
    assertEqual(LexerState::IDENTIFIER, sm.getCurrentState(), "Estado mudou para IDENTIFIER");
    assertTrue(!sm.isAcceptingState(), "Estado IDENTIFIER não é de aceitação");
    assertTrue(!sm.isErrorState(), "Estado IDENTIFIER não é de erro");
    
    sm.transition(' '); // ACCEPT_IDENTIFIER
    assertEqual(LexerState::ACCEPT_IDENTIFIER, sm.getCurrentState(), "Estado mudou para ACCEPT_IDENTIFIER");
    assertTrue(sm.isAcceptingState(), "Estado ACCEPT_IDENTIFIER é de aceitação");
    assertTrue(!sm.isErrorState(), "Estado ACCEPT_IDENTIFIER não é de erro");
    
    // Teste validação de estados de números
    sm.reset();
    sm.transition('1'); // INTEGER
    assertEqual(LexerState::INTEGER, sm.getCurrentState(), "Estado mudou para INTEGER");
    assertTrue(!sm.isAcceptingState(), "Estado INTEGER não é de aceitação");
    assertTrue(!sm.isErrorState(), "Estado INTEGER não é de erro");
    
    sm.transition('.'); // FLOAT_DOT
    assertEqual(LexerState::FLOAT_DOT, sm.getCurrentState(), "Estado mudou para FLOAT_DOT");
    assertTrue(!sm.isAcceptingState(), "Estado FLOAT_DOT não é de aceitação");
    assertTrue(!sm.isErrorState(), "Estado FLOAT_DOT não é de erro");
    
    sm.transition('5'); // FLOAT_DIGITS
    assertEqual(LexerState::FLOAT_DIGITS, sm.getCurrentState(), "Estado mudou para FLOAT_DIGITS");
    assertTrue(!sm.isAcceptingState(), "Estado FLOAT_DIGITS não é de aceitação");
    assertTrue(!sm.isErrorState(), "Estado FLOAT_DIGITS não é de erro");
    
    sm.transition(' '); // ACCEPT_FLOAT
    assertEqual(LexerState::ACCEPT_FLOAT, sm.getCurrentState(), "Estado mudou para ACCEPT_FLOAT");
    assertTrue(sm.isAcceptingState(), "Estado ACCEPT_FLOAT é de aceitação");
    assertTrue(!sm.isErrorState(), "Estado ACCEPT_FLOAT não é de erro");
    
    // Teste validação de estados de operadores
    sm.reset();
    sm.transition('+'); // PLUS
    assertEqual(LexerState::PLUS, sm.getCurrentState(), "Estado mudou para PLUS");
    assertTrue(!sm.isAcceptingState(), "Estado PLUS não é de aceitação");
    assertTrue(!sm.isErrorState(), "Estado PLUS não é de erro");
    
    sm.transition('+'); // INCREMENT
    assertEqual(LexerState::INCREMENT, sm.getCurrentState(), "Estado mudou para INCREMENT");
    assertTrue(!sm.isAcceptingState(), "Estado INCREMENT não é de aceitação");
    assertTrue(!sm.isErrorState(), "Estado INCREMENT não é de erro");
    
    sm.transition(' '); // ACCEPT_OPERATOR
    assertEqual(LexerState::ACCEPT_OPERATOR, sm.getCurrentState(), "Estado mudou para ACCEPT_OPERATOR");
    assertTrue(sm.isAcceptingState(), "Estado ACCEPT_OPERATOR é de aceitação");
    assertTrue(!sm.isErrorState(), "Estado ACCEPT_OPERATOR não é de erro");
    
    // Teste validação de estados de comentários
    sm.reset();
    sm.transition('/'); // DIVIDE
    sm.transition('/'); // LINE_COMMENT
    assertEqual(LexerState::LINE_COMMENT, sm.getCurrentState(), "Estado mudou para LINE_COMMENT");
    assertTrue(!sm.isAcceptingState(), "Estado LINE_COMMENT não é de aceitação");
    assertTrue(!sm.isErrorState(), "Estado LINE_COMMENT não é de erro");
    
    // Teste validação de estados de strings
    sm.reset();
    sm.transition('"'); // STRING_START
    assertEqual(LexerState::STRING_START, sm.getCurrentState(), "Estado mudou para STRING_START");
    assertTrue(!sm.isAcceptingState(), "Estado STRING_START não é de aceitação");
    assertTrue(!sm.isErrorState(), "Estado STRING_START não é de erro");
    
    sm.transition('h'); // STRING_BODY
    assertEqual(LexerState::STRING_BODY, sm.getCurrentState(), "Estado mudou para STRING_BODY");
    assertTrue(!sm.isAcceptingState(), "Estado STRING_BODY não é de aceitação");
    assertTrue(!sm.isErrorState(), "Estado STRING_BODY não é de erro");
    
    sm.transition('"'); // STRING_END
    assertEqual(LexerState::STRING_END, sm.getCurrentState(), "Estado mudou para STRING_END");
    assertTrue(sm.isAcceptingState(), "Estado STRING_END é de aceitação");
    assertTrue(!sm.isErrorState(), "Estado STRING_END não é de erro");
    
    sm.transition(' '); // ACCEPT_STRING
    assertEqual(LexerState::ACCEPT_STRING, sm.getCurrentState(), "Estado mudou para ACCEPT_STRING");
    assertTrue(sm.isAcceptingState(), "Estado ACCEPT_STRING é de aceitação");
    assertTrue(!sm.isErrorState(), "Estado ACCEPT_STRING não é de erro");
    
    // Teste validação de estados de caracteres
    sm.reset();
    sm.transition('\''); // CHAR_START
    assertEqual(LexerState::CHAR_START, sm.getCurrentState(), "Estado mudou para CHAR_START");
    assertTrue(!sm.isAcceptingState(), "Estado CHAR_START não é de aceitação");
    assertTrue(!sm.isErrorState(), "Estado CHAR_START não é de erro");
    
    sm.transition('a'); // CHAR_BODY
    assertEqual(LexerState::CHAR_BODY, sm.getCurrentState(), "Estado mudou para CHAR_BODY");
    assertTrue(!sm.isAcceptingState(), "Estado CHAR_BODY não é de aceitação");
    assertTrue(!sm.isErrorState(), "Estado CHAR_BODY não é de erro");
    
    sm.transition('\''); // CHAR_END
    assertEqual(LexerState::CHAR_END, sm.getCurrentState(), "Estado mudou para CHAR_END");
    assertTrue(sm.isAcceptingState(), "Estado CHAR_END é de aceitação");
    assertTrue(!sm.isErrorState(), "Estado CHAR_END não é de erro");
    
    sm.transition(' '); // ACCEPT_CHAR
    assertEqual(LexerState::ACCEPT_CHAR, sm.getCurrentState(), "Estado mudou para ACCEPT_CHAR");
    assertTrue(sm.isAcceptingState(), "Estado ACCEPT_CHAR é de aceitação");
    assertTrue(!sm.isErrorState(), "Estado ACCEPT_CHAR não é de erro");
    
    // Teste validação de estados de delimitadores
    sm.reset();
    sm.transition('('); // LEFT_PAREN
    assertEqual(LexerState::LEFT_PAREN, sm.getCurrentState(), "Estado mudou para LEFT_PAREN");
    assertTrue(!sm.isAcceptingState(), "Estado LEFT_PAREN não é de aceitação");
    assertTrue(!sm.isErrorState(), "Estado LEFT_PAREN não é de erro");
    
    sm.transition(' '); // ACCEPT_DELIMITER
    assertEqual(LexerState::ACCEPT_DELIMITER, sm.getCurrentState(), "Estado mudou para ACCEPT_DELIMITER");
    assertTrue(sm.isAcceptingState(), "Estado ACCEPT_DELIMITER é de aceitação");
    assertTrue(!sm.isErrorState(), "Estado ACCEPT_DELIMITER não é de erro");
    
    // Teste validação de estados de whitespace
    sm.reset();
    sm.transition(' '); // WHITESPACE
    assertEqual(LexerState::WHITESPACE, sm.getCurrentState(), "Estado mudou para WHITESPACE");
    assertTrue(!sm.isAcceptingState(), "Estado WHITESPACE não é de aceitação");
    assertTrue(!sm.isErrorState(), "Estado WHITESPACE não é de erro");
    
    // Teste validação do estado de erro
    sm.reset();
    sm.transition('\x01'); // ERROR
    assertEqual(LexerState::ERROR, sm.getCurrentState(), "Estado mudou para ERROR");
    assertTrue(!sm.isAcceptingState(), "Estado ERROR não é de aceitação");
    assertTrue(sm.isErrorState(), "Estado ERROR é de erro");
    
    // Teste validação após reset
    sm.reset();
    assertEqual(LexerState::START, sm.getCurrentState(), "Estado após reset é START");
    assertTrue(!sm.isAcceptingState(), "Estado após reset não é de aceitação");
    assertTrue(!sm.isErrorState(), "Estado após reset não é de erro");
    
    // Teste consistência de estados de aceitação e erro
    sm.reset();
    sm.transition('a'); // IDENTIFIER
    sm.transition(' '); // ACCEPT_IDENTIFIER
    assertTrue(sm.isAcceptingState(), "Estado de aceitação é consistente");
    assertTrue(!sm.isErrorState(), "Estado de aceitação não é de erro");
    
    sm.reset();
    sm.transition('\x01'); // ERROR
    assertTrue(!sm.isAcceptingState(), "Estado de erro não é de aceitação");
    assertTrue(sm.isErrorState(), "Estado de erro é consistente");
    
    sm.reset();
    assertEqual(LexerState::START, sm.getCurrentState(), "Estado após reset é START");
    
    // Teste validação de estados de aceitação
    auto accepting_states = sm.getAcceptingStates();
    assertTrue(accepting_states.size() > 0, "Deve haver estados de aceitação");
    
    // Teste validação de estados de erro
    auto error_states = sm.getErrorStates();
    assertTrue(error_states.size() > 0, "Deve haver estados de erro");
    
    // Teste consistência: todos os estados alcançáveis devem ser válidos
    sm.reset();
    
    // Testar sequências de transições válidas
    std::vector<std::string> test_sequences = {
        "abc",      // identificador
        "123",      // inteiro
        "12.34",    // float
        "++",       // increment
        "==",       // equal
        "//test",   // comentário de linha
        "\"hello\"", // string
        "'a'",      // char
        "(){}[]",   // delimitadores
        "   "        // whitespace
    };
    
    for (const auto& sequence : test_sequences) {
        sm.reset();
        for (char c : sequence) {
            if (sm.isValidTransition(c)) {
                LexerState prev_state = sm.getCurrentState();
                sm.transition(c);
                LexerState current_state = sm.getCurrentState();
                // Verifica se a transição foi bem-sucedida (não foi para ERROR)
                assertTrue(current_state != LexerState::ERROR || prev_state == LexerState::ERROR, 
                          "Transição válida não deve levar a erro: " + std::string(1, c));
            }
        }
    }
    
    // Teste validação de transições entre estados
    sm.reset();
    // Teste adicional de validação com transições válidas
    sm.reset();
    sm.transition('a'); // IDENTIFIER
    assertEqual(LexerState::IDENTIFIER, sm.getCurrentState(), "Estado mudou para IDENTIFIER");
    assertTrue(!sm.isAcceptingState(), "Estado IDENTIFIER não é de aceitação");
    assertTrue(!sm.isErrorState(), "Estado IDENTIFIER não é de erro");
    
    if (sm.isValidTransition('b')) {
        sm.transition('b'); // Continua IDENTIFIER
        assertEqual(LexerState::IDENTIFIER, sm.getCurrentState(), "Estado continua IDENTIFIER após 'b'");
        assertTrue(!sm.isAcceptingState(), "Estado IDENTIFIER não é de aceitação");
        assertTrue(!sm.isErrorState(), "Estado IDENTIFIER não é de erro");
    }
    
    if (sm.isValidTransition(' ')) {
        sm.transition(' '); // ACCEPT_IDENTIFIER
        assertEqual(LexerState::ACCEPT_IDENTIFIER, sm.getCurrentState(), "Estado mudou para ACCEPT_IDENTIFIER");
        assertTrue(sm.isAcceptingState(), "Estado é de aceitação");
        assertTrue(!sm.isErrorState(), "Estado de aceitação não é de erro");
    }
    
    // Teste validação após múltiplas transições
    sm.reset();
    sm.transition('1'); // INTEGER
    assertEqual(LexerState::INTEGER, sm.getCurrentState(), "Estado mudou para INTEGER após '1'");
    sm.transition('2'); // INTEGER
    assertEqual(LexerState::INTEGER, sm.getCurrentState(), "Estado continua INTEGER após '2'");
    sm.transition('.'); // FLOAT_DOT
    assertEqual(LexerState::FLOAT_DOT, sm.getCurrentState(), "Estado mudou para FLOAT_DOT após '.'");
    sm.transition('3'); // FLOAT_DIGITS
    assertEqual(LexerState::FLOAT_DIGITS, sm.getCurrentState(), "Estado mudou para FLOAT_DIGITS após '3'");
    sm.transition('4'); // FLOAT_DIGITS
    assertEqual(LexerState::FLOAT_DIGITS, sm.getCurrentState(), "Estado continua FLOAT_DIGITS após '4'");
    
    printTestResult("Validação de Estados (Fase 5.2)", true);
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
        
        // Testes da Fase 5.2
        testStateTransitions();
        testAcceptingStates();
        testErrorStates();
        testStateMachineReset();
        testTransitionTable();
        testStateValidation();
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