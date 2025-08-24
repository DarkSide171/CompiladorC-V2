// Lexer State - Implementação da máquina de estados
// Implementação da classe StateMachine para controle de estados

#include "../include/lexer_state.hpp"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <cctype>

namespace Lexer {

// ============================================================================
// Construtores e Destrutor
// ============================================================================

StateMachine::StateMachine() 
    : current_state_(LexerState::START), error_handler_(nullptr) {
    buildTransitionTable();
}

StateMachine::StateMachine(std::shared_ptr<ErrorHandler> error_handler)
    : current_state_(LexerState::START), error_handler_(error_handler) {
    buildTransitionTable();
}

// ============================================================================
// Métodos principais de controle de estado
// ============================================================================

LexerState StateMachine::getCurrentState() const {
    return current_state_;
}

LexerState StateMachine::transition(char input) {
    LexerState next_state = getNextState(input);
    
    if (next_state == LexerState::ERROR && error_handler_) {
        Position pos{0, 0, 0}; // Posição será fornecida pelo lexer
        error_handler_->reportError(
            ErrorType::INVALID_CHARACTER,
            "Invalid character '" + std::string(1, input) + "' in state " + stateToString(current_state_),
            pos
        );
    }
    
    current_state_ = next_state;
    return current_state_;
}

void StateMachine::reset() {
    current_state_ = LexerState::START;
}

// ============================================================================
// Métodos de verificação de estado
// ============================================================================

bool StateMachine::isAcceptingState() const {
    return isAcceptingState(current_state_);
}

bool StateMachine::isAcceptingState(LexerState state) const {
    return Lexer::isAcceptingState(state);
}

bool StateMachine::isErrorState() const {
    return isErrorState(current_state_);
}

bool StateMachine::isErrorState(LexerState state) const {
    return Lexer::isErrorState(state);
}

bool StateMachine::isValidTransition(char input) const {
    return isValidTransition(current_state_, input);
}

bool StateMachine::isValidTransition(LexerState from_state, char input) const {
    LexerState next_state = getNextState(from_state, input);
    return next_state != LexerState::ERROR;
}

// ============================================================================
// Métodos relacionados a tokens
// ============================================================================

TokenType StateMachine::getTokenType() const {
    return getTokenType(current_state_);
}

TokenType StateMachine::getTokenType(LexerState state) const {
    return stateToTokenType(state);
}

// ============================================================================
// Métodos internos de transição
// ============================================================================

LexerState StateMachine::getNextState(char input) const {
    return getNextState(current_state_, input);
}

LexerState StateMachine::getNextState(LexerState from_state, char input) const {
    // Busca na tabela de transições otimizada
    auto key = std::make_pair(from_state, input);
    auto it = transition_table_.find(key);
    if (it != transition_table_.end()) {
        return it->second;
    }
    
    // Lógica de transição baseada no estado atual e caractere de entrada
    switch (from_state) {
        case LexerState::START:
            if (isAlpha(input) || input == '_') return LexerState::IDENTIFIER;
            if (isDigit(input)) {
                if (input == '0') return LexerState::OCTAL_DIGITS;
                return LexerState::INTEGER;
            }
            if (input == '"') return LexerState::STRING_START;
            if (input == '\'') return LexerState::CHAR_START;
            if (input == '+') return LexerState::PLUS;
            if (input == '-') return LexerState::MINUS;
            if (input == '*') return LexerState::MULTIPLY;
            if (input == '/') return LexerState::DIVIDE;
            if (input == '%') return LexerState::MODULO;
            if (input == '=') return LexerState::ASSIGN;
            if (input == '!') return LexerState::NOT;
            if (input == '<') return LexerState::LESS;
            if (input == '>') return LexerState::GREATER;
            if (input == '&') return LexerState::BITWISE_AND;
            if (input == '|') return LexerState::BITWISE_OR;
            if (input == '^') return LexerState::BITWISE_XOR;
            if (input == '~') return LexerState::BITWISE_NOT;
            if (input == '(') return LexerState::LEFT_PAREN;
            if (input == ')') return LexerState::RIGHT_PAREN;
            if (input == '[') return LexerState::LEFT_BRACKET;
            if (input == ']') return LexerState::RIGHT_BRACKET;
            if (input == '{') return LexerState::LEFT_BRACE;
            if (input == '}') return LexerState::RIGHT_BRACE;
            if (input == ';') return LexerState::SEMICOLON;
            if (input == ',') return LexerState::COMMA;
            if (input == '.') return LexerState::DOT;
            if (input == ':') return LexerState::COLON;
            if (input == '?') return LexerState::QUESTION;
            if (isWhitespace(input)) return LexerState::WHITESPACE;
            if (input == '\n') return LexerState::NEWLINE;
            if (input == '\0') return LexerState::END_OF_FILE;
            break;
            
        case LexerState::IDENTIFIER:
            if (isAlnum(input) || input == '_') return LexerState::IDENTIFIER;
            return LexerState::ACCEPT_IDENTIFIER;
            
        case LexerState::INTEGER:
            if (isDigit(input)) return LexerState::INTEGER;
            if (input == '.') return LexerState::FLOAT_DOT;
            if (input == 'e' || input == 'E') return LexerState::FLOAT_EXP;
            return LexerState::ACCEPT_INTEGER;
            
        case LexerState::OCTAL_DIGITS:
            if (input == 'x' || input == 'X') return LexerState::HEX_PREFIX;
            if (input == 'b' || input == 'B') return LexerState::BINARY_PREFIX;
            if (isOctalDigit(input)) return LexerState::OCTAL_DIGITS;
            if (input == '.') return LexerState::FLOAT_DOT;
            return LexerState::ACCEPT_OCTAL;
            
        case LexerState::HEX_PREFIX:
            if (isHexDigit(input)) return LexerState::HEX_DIGITS;
            break;
            
        case LexerState::HEX_DIGITS:
            if (isHexDigit(input)) return LexerState::HEX_DIGITS;
            return LexerState::ACCEPT_HEX;
            
        case LexerState::BINARY_PREFIX:
            if (isBinaryDigit(input)) return LexerState::BINARY_DIGITS;
            break;
            
        case LexerState::BINARY_DIGITS:
            if (isBinaryDigit(input)) return LexerState::BINARY_DIGITS;
            return LexerState::ACCEPT_BINARY;
            
        case LexerState::FLOAT_DOT:
            if (isDigit(input)) return LexerState::FLOAT_DIGITS;
            break;
            
        case LexerState::FLOAT_DIGITS:
            if (isDigit(input)) return LexerState::FLOAT_DIGITS;
            if (input == 'e' || input == 'E') return LexerState::FLOAT_EXP;
            return LexerState::ACCEPT_FLOAT;
            
        case LexerState::FLOAT_EXP:
            if (input == '+' || input == '-') return LexerState::FLOAT_EXP_SIGN;
            if (isDigit(input)) return LexerState::FLOAT_EXP_DIGITS;
            break;
            
        case LexerState::FLOAT_EXP_SIGN:
            if (isDigit(input)) return LexerState::FLOAT_EXP_DIGITS;
            break;
            
        case LexerState::FLOAT_EXP_DIGITS:
            if (isDigit(input)) return LexerState::FLOAT_EXP_DIGITS;
            return LexerState::ACCEPT_FLOAT;
            
        case LexerState::STRING_START:
            if (input == '"') return LexerState::STRING_END;
            if (input == '\\') return LexerState::STRING_ESCAPE;
            if (input != '\n' && input != '\0') return LexerState::STRING_BODY;
            break;
            
        case LexerState::STRING_BODY:
            if (input == '"') return LexerState::STRING_END;
            if (input == '\\') return LexerState::STRING_ESCAPE;
            if (input != '\n' && input != '\0') return LexerState::STRING_BODY;
            break;
            
        case LexerState::STRING_ESCAPE:
            if (input == 'x') return LexerState::STRING_HEX_ESCAPE;
            if (isOctalDigit(input)) return LexerState::STRING_OCTAL_ESCAPE;
            return LexerState::STRING_BODY;
            
        case LexerState::STRING_END:
            return LexerState::ACCEPT_STRING;
            
        case LexerState::CHAR_START:
            if (input == '\'') return LexerState::CHAR_END;
            if (input == '\\') return LexerState::CHAR_ESCAPE;
            if (input != '\n' && input != '\0') return LexerState::CHAR_BODY;
            break;
            
        case LexerState::CHAR_BODY:
            if (input == '\'') return LexerState::CHAR_END;
            break;
            
        case LexerState::CHAR_ESCAPE:
            return LexerState::CHAR_BODY;
            
        case LexerState::CHAR_END:
            return LexerState::ACCEPT_CHAR;
            
        // Estados de operadores
        case LexerState::PLUS:
            if (input == '+') return LexerState::INCREMENT;
            if (input == '=') return LexerState::ACCEPT_OPERATOR; // +=
            return LexerState::ACCEPT_OPERATOR;
            
        case LexerState::MINUS:
            if (input == '-') return LexerState::DECREMENT;
            if (input == '=') return LexerState::ACCEPT_OPERATOR; // -=
            if (input == '>') return LexerState::ARROW;
            return LexerState::ACCEPT_OPERATOR;
            
        case LexerState::MULTIPLY:
            if (input == '=') return LexerState::ACCEPT_OPERATOR; // *=
            return LexerState::ACCEPT_OPERATOR;
            
        case LexerState::DIVIDE:
            if (input == '=') return LexerState::ACCEPT_OPERATOR; // /=
            if (input == '/') return LexerState::LINE_COMMENT;
            if (input == '*') return LexerState::BLOCK_COMMENT;
            return LexerState::ACCEPT_OPERATOR;
            
        case LexerState::ASSIGN:
            if (input == '=') return LexerState::EQUAL;
            return LexerState::ACCEPT_OPERATOR;
            
        case LexerState::EQUAL:
            return LexerState::ACCEPT_OPERATOR;
            
        case LexerState::NOT:
            if (input == '=') return LexerState::NOT_EQUAL;
            return LexerState::ACCEPT_OPERATOR;
            
        case LexerState::NOT_EQUAL:
            return LexerState::ACCEPT_OPERATOR;
            
        case LexerState::LESS:
            if (input == '=') return LexerState::LESS_EQUAL;
            if (input == '<') return LexerState::LEFT_SHIFT;
            return LexerState::ACCEPT_OPERATOR;
            
        case LexerState::GREATER:
            if (input == '=') return LexerState::GREATER_EQUAL;
            if (input == '>') return LexerState::RIGHT_SHIFT;
            return LexerState::ACCEPT_OPERATOR;
            
        case LexerState::BITWISE_AND:
            if (input == '&') return LexerState::LOGICAL_AND;
            if (input == '=') return LexerState::ACCEPT_OPERATOR; // &=
            return LexerState::ACCEPT_OPERATOR;
            
        case LexerState::BITWISE_OR:
            if (input == '|') return LexerState::LOGICAL_OR;
            if (input == '=') return LexerState::ACCEPT_OPERATOR; // |=
            return LexerState::ACCEPT_OPERATOR;
            
        case LexerState::INCREMENT:
        case LexerState::DECREMENT:
        case LexerState::ARROW:
        case LexerState::LOGICAL_AND:
        case LexerState::LOGICAL_OR:
        case LexerState::LEFT_SHIFT:
        case LexerState::RIGHT_SHIFT:
        case LexerState::LESS_EQUAL:
        case LexerState::GREATER_EQUAL:
            return LexerState::ACCEPT_OPERATOR;
            
        // Estados de comentários
        case LexerState::LINE_COMMENT:
            if (input == '\n') return LexerState::ACCEPT_COMMENT;
            return LexerState::LINE_COMMENT;
            
        case LexerState::BLOCK_COMMENT:
            if (input == '*') return LexerState::BLOCK_COMMENT_END;
            return LexerState::BLOCK_COMMENT;
            
        case LexerState::BLOCK_COMMENT_END:
            if (input == '/') return LexerState::ACCEPT_COMMENT;
            if (input == '*') return LexerState::BLOCK_COMMENT_END;
            return LexerState::BLOCK_COMMENT;
            
        // Estados de delimitadores
        case LexerState::LEFT_PAREN:
        case LexerState::RIGHT_PAREN:
        case LexerState::LEFT_BRACKET:
        case LexerState::RIGHT_BRACKET:
        case LexerState::LEFT_BRACE:
        case LexerState::RIGHT_BRACE:
        case LexerState::SEMICOLON:
        case LexerState::COMMA:
        case LexerState::DOT:
        case LexerState::COLON:
        case LexerState::QUESTION:
            return LexerState::ACCEPT_DELIMITER;
            
        // Estados especiais
        case LexerState::WHITESPACE:
            if (isWhitespace(input)) return LexerState::WHITESPACE;
            return LexerState::START;
            
        case LexerState::NEWLINE:
            return LexerState::START;
            
        case LexerState::END_OF_FILE:
            return LexerState::ACCEPT_EOF;
            
        // Estados de aceitação retornam para START
        case LexerState::ACCEPT_IDENTIFIER:
        case LexerState::ACCEPT_INTEGER:
        case LexerState::ACCEPT_FLOAT:
        case LexerState::ACCEPT_HEX:
        case LexerState::ACCEPT_OCTAL:
        case LexerState::ACCEPT_BINARY:
        case LexerState::ACCEPT_STRING:
        case LexerState::ACCEPT_CHAR:
        case LexerState::ACCEPT_OPERATOR:
        case LexerState::ACCEPT_DELIMITER:
        case LexerState::ACCEPT_COMMENT:
        case LexerState::ACCEPT_EOF:
            return LexerState::START;
            
        default:
            break;
    }
    
    return LexerState::ERROR;
}

// ============================================================================
// Método de otimização
// ============================================================================

void StateMachine::buildTransitionTable() {
    // A tabela de transições será construída dinamicamente conforme necessário
    // Para otimização futura, pode-se pré-computar transições mais comuns
    transition_table_.clear();
    
    // Pré-computar algumas transições comuns para melhor performance
    // Transições do estado START para identificadores
    for (char c = 'a'; c <= 'z'; ++c) {
        transition_table_[{LexerState::START, c}] = LexerState::IDENTIFIER;
    }
    for (char c = 'A'; c <= 'Z'; ++c) {
        transition_table_[{LexerState::START, c}] = LexerState::IDENTIFIER;
    }
    transition_table_[{LexerState::START, '_'}] = LexerState::IDENTIFIER;
    
    // Transições do estado START para números
    for (char c = '1'; c <= '9'; ++c) {
        transition_table_[{LexerState::START, c}] = LexerState::INTEGER;
    }
    transition_table_[{LexerState::START, '0'}] = LexerState::OCTAL_DIGITS;
}

// ============================================================================
// Métodos utilitários
// ============================================================================

std::string StateMachine::stateToString(LexerState state) const {
    return lexerStateToString(state);
}

void StateMachine::setErrorHandler(std::shared_ptr<ErrorHandler> error_handler) {
    error_handler_ = error_handler;
}

std::shared_ptr<ErrorHandler> StateMachine::getErrorHandler() const {
    return error_handler_;
}

// ============================================================================
// Métodos para debug e logging
// ============================================================================

void StateMachine::printCurrentState() const {
    std::cout << "Current State: " << stateToString(current_state_) << std::endl;
}

void StateMachine::printTransitionTable() const {
    std::cout << "Transition Table (" << transition_table_.size() << " entries):" << std::endl;
    for (const auto& entry : transition_table_) {
        std::cout << "  " << lexerStateToString(entry.first.first) 
                  << " + '" << entry.first.second << "' -> " 
                  << lexerStateToString(entry.second) << std::endl;
    }
}

std::vector<LexerState> StateMachine::getAcceptingStates() const {
    return {
        LexerState::ACCEPT_IDENTIFIER,
        LexerState::ACCEPT_INTEGER,
        LexerState::ACCEPT_FLOAT,
        LexerState::ACCEPT_HEX,
        LexerState::ACCEPT_OCTAL,
        LexerState::ACCEPT_BINARY,
        LexerState::ACCEPT_STRING,
        LexerState::ACCEPT_CHAR,
        LexerState::ACCEPT_OPERATOR,
        LexerState::ACCEPT_DELIMITER,
        LexerState::ACCEPT_COMMENT,
        LexerState::ACCEPT_EOF
    };
}

std::vector<LexerState> StateMachine::getErrorStates() const {
    return { LexerState::ERROR };
}

// ============================================================================
// Métodos auxiliares privados
// ============================================================================

std::string StateMachine::stateKey(LexerState state, char input) const {
    return std::to_string(static_cast<int>(state)) + "_" + std::string(1, input);
}

bool StateMachine::isAlpha(char c) const {
    return std::isalpha(static_cast<unsigned char>(c));
}

bool StateMachine::isDigit(char c) const {
    return std::isdigit(static_cast<unsigned char>(c));
}

bool StateMachine::isAlnum(char c) const {
    return std::isalnum(static_cast<unsigned char>(c));
}

bool StateMachine::isHexDigit(char c) const {
    return std::isxdigit(static_cast<unsigned char>(c));
}

bool StateMachine::isOctalDigit(char c) const {
    return c >= '0' && c <= '7';
}

bool StateMachine::isBinaryDigit(char c) const {
    return c == '0' || c == '1';
}

bool StateMachine::isWhitespace(char c) const {
    return c == ' ' || c == '\t' || c == '\r';
}

// ============================================================================
// Funções utilitárias globais
// ============================================================================

std::string lexerStateToString(LexerState state) {
    switch (state) {
        case LexerState::START: return "START";
        case LexerState::IDENTIFIER: return "IDENTIFIER";
        case LexerState::INTEGER: return "INTEGER";
        case LexerState::FLOAT_DOT: return "FLOAT_DOT";
        case LexerState::FLOAT_DIGITS: return "FLOAT_DIGITS";
        case LexerState::FLOAT_EXP: return "FLOAT_EXP";
        case LexerState::FLOAT_EXP_SIGN: return "FLOAT_EXP_SIGN";
        case LexerState::FLOAT_EXP_DIGITS: return "FLOAT_EXP_DIGITS";
        case LexerState::HEX_PREFIX: return "HEX_PREFIX";
        case LexerState::HEX_DIGITS: return "HEX_DIGITS";
        case LexerState::OCTAL_DIGITS: return "OCTAL_DIGITS";
        case LexerState::BINARY_PREFIX: return "BINARY_PREFIX";
        case LexerState::BINARY_DIGITS: return "BINARY_DIGITS";
        case LexerState::STRING_START: return "STRING_START";
        case LexerState::STRING_BODY: return "STRING_BODY";
        case LexerState::STRING_ESCAPE: return "STRING_ESCAPE";
        case LexerState::STRING_HEX_ESCAPE: return "STRING_HEX_ESCAPE";
        case LexerState::STRING_OCTAL_ESCAPE: return "STRING_OCTAL_ESCAPE";
        case LexerState::STRING_END: return "STRING_END";
        case LexerState::CHAR_START: return "CHAR_START";
        case LexerState::CHAR_BODY: return "CHAR_BODY";
        case LexerState::CHAR_ESCAPE: return "CHAR_ESCAPE";
        case LexerState::CHAR_END: return "CHAR_END";
        case LexerState::PLUS: return "PLUS";
        case LexerState::MINUS: return "MINUS";
        case LexerState::MULTIPLY: return "MULTIPLY";
        case LexerState::DIVIDE: return "DIVIDE";
        case LexerState::MODULO: return "MODULO";
        case LexerState::ASSIGN: return "ASSIGN";
        case LexerState::EQUAL: return "EQUAL";
        case LexerState::NOT: return "NOT";
        case LexerState::NOT_EQUAL: return "NOT_EQUAL";
        case LexerState::LESS: return "LESS";
        case LexerState::LESS_EQUAL: return "LESS_EQUAL";
        case LexerState::GREATER: return "GREATER";
        case LexerState::GREATER_EQUAL: return "GREATER_EQUAL";
        case LexerState::LOGICAL_AND_FIRST: return "LOGICAL_AND_FIRST";
        case LexerState::LOGICAL_AND: return "LOGICAL_AND";
        case LexerState::LOGICAL_OR_FIRST: return "LOGICAL_OR_FIRST";
        case LexerState::LOGICAL_OR: return "LOGICAL_OR";
        case LexerState::BITWISE_AND: return "BITWISE_AND";
        case LexerState::BITWISE_OR: return "BITWISE_OR";
        case LexerState::BITWISE_XOR: return "BITWISE_XOR";
        case LexerState::BITWISE_NOT: return "BITWISE_NOT";
        case LexerState::LEFT_SHIFT_FIRST: return "LEFT_SHIFT_FIRST";
        case LexerState::LEFT_SHIFT: return "LEFT_SHIFT";
        case LexerState::RIGHT_SHIFT_FIRST: return "RIGHT_SHIFT_FIRST";
        case LexerState::RIGHT_SHIFT: return "RIGHT_SHIFT";
        case LexerState::INCREMENT_FIRST: return "INCREMENT_FIRST";
        case LexerState::INCREMENT: return "INCREMENT";
        case LexerState::DECREMENT_FIRST: return "DECREMENT_FIRST";
        case LexerState::DECREMENT: return "DECREMENT";
        case LexerState::ARROW_FIRST: return "ARROW_FIRST";
        case LexerState::ARROW: return "ARROW";
        case LexerState::COMMENT_START: return "COMMENT_START";
        case LexerState::LINE_COMMENT: return "LINE_COMMENT";
        case LexerState::BLOCK_COMMENT: return "BLOCK_COMMENT";
        case LexerState::BLOCK_COMMENT_END: return "BLOCK_COMMENT_END";
        case LexerState::LEFT_PAREN: return "LEFT_PAREN";
        case LexerState::RIGHT_PAREN: return "RIGHT_PAREN";
        case LexerState::LEFT_BRACKET: return "LEFT_BRACKET";
        case LexerState::RIGHT_BRACKET: return "RIGHT_BRACKET";
        case LexerState::LEFT_BRACE: return "LEFT_BRACE";
        case LexerState::RIGHT_BRACE: return "RIGHT_BRACE";
        case LexerState::SEMICOLON: return "SEMICOLON";
        case LexerState::COMMA: return "COMMA";
        case LexerState::DOT: return "DOT";
        case LexerState::COLON: return "COLON";
        case LexerState::QUESTION: return "QUESTION";
        case LexerState::WHITESPACE: return "WHITESPACE";
        case LexerState::NEWLINE: return "NEWLINE";
        case LexerState::END_OF_FILE: return "END_OF_FILE";
        case LexerState::ERROR: return "ERROR";
        case LexerState::ACCEPT_IDENTIFIER: return "ACCEPT_IDENTIFIER";
        case LexerState::ACCEPT_INTEGER: return "ACCEPT_INTEGER";
        case LexerState::ACCEPT_FLOAT: return "ACCEPT_FLOAT";
        case LexerState::ACCEPT_HEX: return "ACCEPT_HEX";
        case LexerState::ACCEPT_OCTAL: return "ACCEPT_OCTAL";
        case LexerState::ACCEPT_BINARY: return "ACCEPT_BINARY";
        case LexerState::ACCEPT_STRING: return "ACCEPT_STRING";
        case LexerState::ACCEPT_CHAR: return "ACCEPT_CHAR";
        case LexerState::ACCEPT_OPERATOR: return "ACCEPT_OPERATOR";
        case LexerState::ACCEPT_DELIMITER: return "ACCEPT_DELIMITER";
        case LexerState::ACCEPT_COMMENT: return "ACCEPT_COMMENT";
        case LexerState::ACCEPT_EOF: return "ACCEPT_EOF";
        default: return "UNKNOWN";
    }
}

LexerState stringToLexerState(const std::string& state_str) {
    if (state_str == "START") return LexerState::START;
    if (state_str == "IDENTIFIER") return LexerState::IDENTIFIER;
    if (state_str == "INTEGER") return LexerState::INTEGER;
    if (state_str == "FLOAT_DOT") return LexerState::FLOAT_DOT;
    if (state_str == "FLOAT_DIGITS") return LexerState::FLOAT_DIGITS;
    if (state_str == "FLOAT_EXP") return LexerState::FLOAT_EXP;
    if (state_str == "FLOAT_EXP_SIGN") return LexerState::FLOAT_EXP_SIGN;
    if (state_str == "FLOAT_EXP_DIGITS") return LexerState::FLOAT_EXP_DIGITS;
    if (state_str == "HEX_PREFIX") return LexerState::HEX_PREFIX;
    if (state_str == "HEX_DIGITS") return LexerState::HEX_DIGITS;
    if (state_str == "OCTAL_DIGITS") return LexerState::OCTAL_DIGITS;
    if (state_str == "BINARY_PREFIX") return LexerState::BINARY_PREFIX;
    if (state_str == "BINARY_DIGITS") return LexerState::BINARY_DIGITS;
    if (state_str == "ERROR") return LexerState::ERROR;
    if (state_str == "ACCEPT_IDENTIFIER") return LexerState::ACCEPT_IDENTIFIER;
    if (state_str == "ACCEPT_INTEGER") return LexerState::ACCEPT_INTEGER;
    if (state_str == "ACCEPT_FLOAT") return LexerState::ACCEPT_FLOAT;
    if (state_str == "ACCEPT_HEX") return LexerState::ACCEPT_HEX;
    if (state_str == "ACCEPT_OCTAL") return LexerState::ACCEPT_OCTAL;
    if (state_str == "ACCEPT_BINARY") return LexerState::ACCEPT_BINARY;
    if (state_str == "ACCEPT_STRING") return LexerState::ACCEPT_STRING;
    if (state_str == "ACCEPT_CHAR") return LexerState::ACCEPT_CHAR;
    if (state_str == "ACCEPT_OPERATOR") return LexerState::ACCEPT_OPERATOR;
    if (state_str == "ACCEPT_DELIMITER") return LexerState::ACCEPT_DELIMITER;
    if (state_str == "ACCEPT_COMMENT") return LexerState::ACCEPT_COMMENT;
    if (state_str == "ACCEPT_EOF") return LexerState::ACCEPT_EOF;
    return LexerState::ERROR;
}

bool isAcceptingState(LexerState state) {
    switch (state) {
        case LexerState::ACCEPT_IDENTIFIER:
        case LexerState::ACCEPT_INTEGER:
        case LexerState::ACCEPT_FLOAT:
        case LexerState::ACCEPT_HEX:
        case LexerState::ACCEPT_OCTAL:
        case LexerState::ACCEPT_BINARY:
        case LexerState::ACCEPT_STRING:
        case LexerState::ACCEPT_CHAR:
        case LexerState::ACCEPT_OPERATOR:
        case LexerState::ACCEPT_DELIMITER:
        case LexerState::ACCEPT_COMMENT:
        case LexerState::ACCEPT_EOF:
            return true;
        default:
            return false;
    }
}

bool isErrorState(LexerState state) {
    return state == LexerState::ERROR;
}

TokenType stateToTokenType(LexerState state) {
    switch (state) {
        case LexerState::ACCEPT_IDENTIFIER:
            return TokenType::IDENTIFIER;
        case LexerState::ACCEPT_INTEGER:
            return TokenType::INTEGER_LITERAL;
        case LexerState::ACCEPT_FLOAT:
            return TokenType::FLOAT_LITERAL;
        case LexerState::ACCEPT_HEX:
            return TokenType::INTEGER_LITERAL;
        case LexerState::ACCEPT_OCTAL:
            return TokenType::INTEGER_LITERAL;
        case LexerState::ACCEPT_BINARY:
            return TokenType::INTEGER_LITERAL;
        case LexerState::ACCEPT_STRING:
            return TokenType::STRING_LITERAL;
        case LexerState::ACCEPT_CHAR:
            return TokenType::CHAR_LITERAL;
        case LexerState::ACCEPT_EOF:
            return TokenType::END_OF_FILE;
        case LexerState::ACCEPT_COMMENT:
            return TokenType::LINE_COMMENT; // ou BLOCK_COMMENT dependendo do contexto
        case LexerState::ACCEPT_OPERATOR:
        case LexerState::ACCEPT_DELIMITER:
            // Será determinado pelo contexto específico do operador/delimitador
            return TokenType::UNKNOWN;
        default:
            return TokenType::UNKNOWN;
    }
}

} // namespace Lexer